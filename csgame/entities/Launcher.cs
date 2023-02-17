using Slate2D;

[Spawnable]
class Launcher : Entity
{
    uint LaunchTime = 180;
    uint LaunchWait = 180;

    public Launcher(LDTKEntity ent) : base(ent)
    {
        Collidable = CollisionType.Enabled;
        Sprite = Assets.Find("launcher");
        Layer = Layer.Background;
    }

    public override void Die() { }

    public override void Update(uint ticks, float dt)
    {
        if (ticks < LaunchTime)
        {
            return;
        }

        Main.World.SpawnPuffParticle(Pos.X - 8, Pos.Y);
        var ent = Main.World.SpawnEntity("Cannonball");
        if (ent == null) return;
        ent.Vel = (-1, 0);
        ent.Pos = (Pos.X - 16, Pos.Y + 1);
        LaunchTime = ticks + LaunchWait;
    }
}