namespace Ghost;
using Slate2D;

enum Frames {
  Idle,
  Float1,
  Float2,
  Float3,
  Float4,
  Pain,
}

enum States {
  Default,
  Idle,
  Float,
}

[Spawnable]
class Ghost : FSMEntity<States> {
  public Ghost(LDTKEntity ent) : base(ent) {
    Sprite = Assets.Find("ghost");
    FSMTransitionTo(States.Idle);
    DrawOfs = (-2, -1);
    WorldCollide = false;
  }

  public override void Die() {
    base.Die();
    Main.World.SpawnDeathParticle(this, (uint)Frames.Pain);
  }

  CollisionType Default_CanCollide(Entity other, Dir dir) {
    if (other is Player && dir == Dir.Up) return CollisionType.Platform;
    else return CollisionType.Trigger;
  }
  void Default_Collide(Entity other, Dir dir) => HandlePlayerStomp(other, dir);

  void Idle_Enter() {
    FSMTimer(States.Float, 90);
    Vel = (0, 0);
    Frame = (uint)Frames.Idle;
  }
  void Idle_Update(uint ticks, float dt) => FlipBits = (byte)(Center.X < Main.World.Player.Center.X ? 1 : 0);

  void Float_Enter() => FSMTimer(States.Idle, 180);
  void Float_Update(uint ticks, float dt) {
    var player = Main.World.Player;
    Vel.X += Math.Sign(player.Center.X - Center.X) * 0.03f;
    Vel.Y += Math.Sign(player.Center.Y - Center.Y) * 0.03f;
    Vel.X = Math.Clamp(Vel.X, -0.6f, 0.6f);
    Vel.Y = Math.Clamp(Vel.Y, -0.6f, 0.6f);

    Frame = (ticks / 8 % 4) + (uint)Frames.Float1;
    if (Vel.X != 0) {
      FlipBits = (byte)(Vel.X < 0 ? 1 : 0);
    }
  }

  public override void Update(uint ticks, float dt) {
    base.Update(ticks, dt);
    MoveX(Vel.X);
    MoveY(Vel.Y);
  }
}
