using Slate2D;

[Spawnable]
class Health : Entity {
  public Health(LDTKEntity ent) : base(ent) {
    Collidable = CollisionType.Trigger;
    Sprite = Assets.Find("health");
    Layer = Layer.Background;
    DrawOfs = (0, -3);
  }

  public override void Collide(Entity other, Dir dir) {
    if (other is Player == false) return;

    var player = (Player)other;
    if (player.Health >= player.MaxHealth) {
      return;
    }

    Destroyed = true;
    player.Health += 1;
  }

  public override void Update(uint ticks, float dt) {
    var cycle = ticks % 40;
    if (cycle == 0 || cycle == 10) {
      Pos.Y += 1;
    }
    else if (cycle == 20 || cycle == 30) {
      Pos.Y -= 1;
    }
  }
}
