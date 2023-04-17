using Slate2D;

[Spawnable]
class Spring : Entity {
  bool Activated = false;
  uint ActivateTicks = 0;
  uint Delay = 12;

  public Spring(LDTKEntity ent) : base(ent) {
    Collidable = CollisionType.Platform;
    Sprite = Assets.Find("spring");
    Layer = Layer.Background;
  }

  public override void PreUpdate(uint ticks, float dt) {
    if (!Activated) return;
    if (Ticks < ActivateTicks) return;

    foreach (var other in GetRidingEntities()) {
      if (other is Player) {
        var player = (Player)other;
        player.DisableControls = false;
        player.DisableMovement = false;
        player.Vel.Y += Input.ButtonPressed((int)Buttons.Jump) ? -Phys.SpringJumpHeld : -Phys.SpringJump;
        player.JumpHeld = true;
      }
      else {
        other.Vel.Y += -Phys.SpringJump;
      }
    }

    Activated = false;
  }

  public override void Update(uint ticks, float dt) {
    var oldFrame = Frame;
    Frame = Activated ? (ticks - (ActivateTicks - Delay)) / 3 : 0;
    Frame = Frame >= 3 ? 1 : Frame;

    var amt = (int)(Frame - oldFrame) * 4;
    MoveSolid(0, amt);
    Size.H -= amt;
  }

  public override void Collide(Entity other, Dir dir) {
    if (dir != Dir.Up) return;
    if (!other.WorldCollide) return;

    if (!Activated) {
      Activated = true;
      ActivateTicks = Ticks + Delay;
    }

    if (other is Player) {
      var player = (Player)other;
      player.DisableControls = true;
      player.DisableMovement = true;
    }
  }
}
