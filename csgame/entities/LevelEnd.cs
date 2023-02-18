using Slate2D;

[Spawnable]
class LevelEnd : Entity
{
    public LevelEnd(LDTKEntity ent) : base(ent)
    {
        Collidable = CollisionType.Trigger;
        Sprite = Assets.Find("levelend");
        Layer = Layer.Background;
        DrawOfs = (-2, -2);
    }

    public override void Collide(Entity other, Dir dir)
    {
        if (other is Player == false) return;

        Destroyed = true;
        Main.World.CompleteLevel();
    }

    public override void Update(uint ticks, float dt)
    {
        Frame = ticks / 8 % 8;
        DrawOfs.Y = Frame < 4 ? -3 : -4;
    }
}