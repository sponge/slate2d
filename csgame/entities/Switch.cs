using Slate2D;

[Spawnable]
class Switch : Entity
{
    String Target;

    public Switch(LDTKEntity ent) : base(ent)
    {
        Collidable = CollisionType.Platform;
        Layer = Layer.Background;
        Target = ent.Properties.GetValueOrDefault("Target")?.EntRef ?? "";
    }

    public override void Draw()
    {
        DC.SetColor(172, 50, 50, 255);
        DC.Rect(Pos.X, Pos.Y, Size.W, Size.H, false);

    }

    public override void Collide(Entity other, Dir dir)
    {
        if (dir != Dir.Up) return;

        foreach (var ent in Main.World.GameState.Entities)
        {
            if (ent.Id == Target) ent.Activate(this);
        }

        Destroyed = true;
    }
}