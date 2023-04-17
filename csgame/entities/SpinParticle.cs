using Slate2D;

public class SpinParticle : Entity {
  public uint Start = 0;
  public bool Rotate = true;

  public SpinParticle() : base() {
    Type = "SpinParticle";
    Collidable = CollisionType.Disabled;
    WorldCollide = false;
    Sprite = Assets.Find("puff");
    Vel = (1, -3);
  }

  public override void Update(uint ticks, float dt) {
    if (ticks >= Start + 120) Die();
    Pos.X += (int)Vel.X;
    Pos.Y += (int)Vel.Y;
    Vel.Y = MathF.Min(Vel.Y + 0.2f, Phys.TerminalVelocity);
  }

  public override void Draw() {
    var rotate = 8 * (Ticks - Start);
    DC.Translate(Center.X, Center.Y);
    if (Rotate) DC.Rotate(rotate);
    DC.Sprite(Sprite, Frame, -Size.W / 2 + DrawOfs.X, -Size.H / 2 + DrawOfs.Y, 1, FlipBits, 1, 1);
    if (Rotate) DC.Rotate(-rotate);
    DC.Translate(-Center.X, -Center.Y);
  }
}
