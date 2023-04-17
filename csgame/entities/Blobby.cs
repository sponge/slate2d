namespace Blobby;
using Slate2D;

enum Frames {
  Idle1,
  Idle2,
  Sink1,
  Sink2,
  Sunk,
  Pain
}

enum States {
  Default,
  Idle,
  Sink,
  Rise,
  Move
}

[Spawnable]
class Blobby : FSMEntity<States> {
  static Frames[] SinkAnim = new[] { Frames.Sink1, Frames.Sink2 };
  static Frames[] RiseAnim = new[] { Frames.Sink2, Frames.Sink1 };

  float LastVelX = -1.5f;

  public Blobby(LDTKEntity ent) : base(ent) {
    Sprite = Assets.Find("blobby");
    FSMTransitionTo(States.Idle);
  }

  public override void Die() {
    base.Die();
    Main.World.SpawnDeathParticle(this, (uint)Frames.Pain);
  }

  public override void Update(uint ticks, float dt) {
    var grounded = Vel.Y >= 0 && CollideAt(Pos.X, Pos.Y + 1, Dir.Down);
    Vel.Y = grounded ? 0 : Vel.Y + Phys.EnemyGravity;

    base.Update(ticks, dt);

    MoveX(Vel.X);
    MoveY(Vel.Y);
  }

  CollisionType Default_CanCollide(Entity other, Dir dir) => StandardEnemyCanCollide(other, dir);
  void Default_Collide(Entity other, Dir dir) => HandlePlayerStomp(other, dir);

  void Idle_Enter() => FSMTimer(States.Sink, 40);
  void Idle_Update(uint ticks, float dt) => Frame = (uint)(ticks % 40 <= 20 ? Frames.Idle1 : Frames.Idle2);

  void Sink_Enter() => FSMTimer(States.Move, 20);
  void Sink_Update(uint ticks, float dt) => Frame = Animate(SinkAnim);

  void Rise_Enter() => FSMTimer(States.Idle, 20);
  void Rise_Update(uint ticks, float dt) => Frame = Animate(RiseAnim);

  void Move_Enter() {
    FSMTimer(States.Rise, 60);
    Vel.X = LastVelX;
    Frame = (uint)Frames.Sunk;
  }
  void Move_Exit() {
    LastVelX = Vel.X;
    Vel.X = 0;
  }
  CollisionType Move_CanCollide(Entity other, Dir dir) => other is Player ? CollisionType.Trigger : CollisionType.Enabled;
  void Move_Collide(Entity other, Dir dir) {
    if (other is Player) {
      other.Hurt(1);
      return;
    }

    if (Vel.X < 0 && dir == Dir.Left) Vel.X *= -1;
    else if (Vel.X > 0 && dir == Dir.Right) Vel.X *= -1;

    if (dir == Dir.Up || dir == Dir.Down) Vel.Y = 0;
  }

}
