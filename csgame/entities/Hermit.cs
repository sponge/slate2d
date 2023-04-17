namespace Hermit;
using Slate2D;

enum Frames {
  Walk1,
  Walk2,
  Shell,
}

enum States {
  Default,
  Walk,
  Shell,
  ShellSpin,
}

[Spawnable]
class Hermit : FSMEntity<States> {
  float SpinSpeed = 3;
  float WalkSpeed = 0.25f;

  public Hermit(LDTKEntity ent) : base(ent) {
    DrawOfs = (-4, -1);
    Sprite = Assets.Find("hermit");
    FSMTransitionTo(States.Walk);
  }

  public override void Die() {
    base.Die();
    Main.World.SpawnDeathParticle(this, (uint)Frames.Shell);
  }

  public override void Update(uint ticks, float dt) {
    base.Update(ticks, dt);

    var grounded = Vel.Y >= 0 && CollideAt(Pos.X, Pos.Y + 1, Dir.Down);
    Vel.Y = grounded ? 0 : Pos.Y + Phys.EnemyGravity;

    MoveX(Vel.X);
    MoveY(Vel.Y);
    if (Vel.X != 0) FlipBits = (byte)(Vel.X < 0 ? 1 : 0);
  }

  void Walk_Update(uint ticks, float dt) {
    Frame = (ticks / 8) % 2;
    if (Math.Abs(Vel.X) != WalkSpeed) Vel.X = -WalkSpeed;
  }
  CollisionType Walk_CanCollide(Entity other, Dir dir) => StandardEnemyCanCollide(other, dir);
  void Walk_Collide(Entity other, Dir dir) {
    if (HandlePlayerStomp(other, dir, false)) {
      FSMTransitionTo(States.Shell);
    }

    if (other is Player == false) {
      if (Vel.X < 0 && dir == Dir.Left) Vel.X *= -1;
      else if (Vel.X > 0 && dir == Dir.Right) Vel.X *= -1;
      if (dir == Dir.Up || dir == Dir.Down) Vel.Y = 0;
    }
  }

  void Shell_Enter() {
    FSMTimer(States.Walk, 300);
    Frame = (uint)Frames.Shell;
    Vel.X = 0;
  }
  CollisionType Shell_CanCollide(Entity other, Dir dir) => CollisionType.Enabled;
  void Shell_Collide(Entity other, Dir dir) {
    // FIXME: this is still not good, see handleEnemyStomp
    if (other is Player && other.Max.Y <= Min.Y) ((Player)other).StompEnemy();
    Vel.X = Vel.X == 0 ? SpinSpeed : 0;
    Vel.X *= Center.X - other.Center.X > 0 ? 1 : -1;
    FSMTransitionTo(States.ShellSpin);
  }

  void ShellSpin_Enter() => Frame = (uint)Frames.Shell;
  CollisionType ShellSpin_CanCollide(Entity other, Dir dir) => StandardEnemyCanCollide(other, dir);
  void ShellSpin_Collide(Entity other, Dir dir) {
    if (other is Player && other.Max.Y <= Min.Y) {
      ((Player)other).StompEnemy();
      FSMTransitionTo(States.Shell);
      return;
    }

    other.Hurt(1);
    // if we can't destroy it, bounce back
    if (!other.Destroyed && other is Player == false && (dir == Dir.Left || dir == Dir.Right)) {
      Vel.X *= -1;
    }
  }
}
