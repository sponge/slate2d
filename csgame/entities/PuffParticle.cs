using Slate2D;

public class PuffParticle : Entity {
  public uint Start = 0;

  public PuffParticle() : base() {
    Type = "PuffParticle";
    Collidable = CollisionType.Disabled;
    WorldCollide = false;
    Sprite = Assets.Find("puff");
  }

  public override void Update(uint ticks, float dt) {
    Frame = (ticks - Start) / 4;
    if (Frame > 7) Destroyed = true;
  }
}
