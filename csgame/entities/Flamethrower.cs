using Slate2D;

[Spawnable]
class Flamethrower : Entity
{
    bool StartOn = false;
    bool Spawned = false;

    public Flamethrower(LDTKEntity ent) : base(ent)
    {
        Collidable = CollisionType.Enabled;
        Sprite = Assets.Find("flamethrower");

        StartOn = ent.Properties?["StartOn"]?.Bool ?? false;
    }

    public override void PreUpdate(uint ticks, float dt)
    {
        if (Spawned) return;

        Spawned = true;
        var ent = Main.World.SpawnEntity("Flame") as Flame;
        if (ent == null) return;

        ent.StartOn = StartOn;
        ent.Pos = Pos;
        ent.Pos.Y -= ent.Size.H;
    }
}