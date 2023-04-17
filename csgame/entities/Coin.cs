using Slate2D;

[Spawnable]
class Coin : Entity {
  public Coin(LDTKEntity ent) : base(ent) {
    Collidable = CollisionType.Trigger;
    Sprite = Assets.Find("coin");
    Layer = Layer.Background;
  }

  public override void Collide(Entity other, Dir dir) {
    if (other is Player == false) return;

    Destroyed = true;
    Main.World.GameState.CurrentCoins++;
  }

  public override void Update(uint ticks, float dt) => Frame = ticks / 8 % 4;
}
