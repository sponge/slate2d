using Slate2D;

[Spawnable]
class CrumblingBlock : Entity
{
    enum Frames
    {
        Uncracked, Cracked1, Cracked2, Cracked3, Cracked4
    }
    Frames[] anim = new[] { Frames.Cracked1, Frames.Cracked2, Frames.Cracked3, Frames.Cracked4 };

    bool Broken = false;
    uint BreakTime = 0;
    const uint CrumbleTime = 40;

    public override void Die() { }

    public CrumblingBlock(LDTKEntity ent) : base(ent)
    {
        Sprite = Assets.Find("crumblingblock");
        Collidable = CollisionType.Enabled;
        Layer = Layer.Background;

    }

    public override void Update(uint ticks, float dt)
    {
        if (!Broken) return;

        Frame = Animate(anim, BreakTime - CrumbleTime, BreakTime);
        if (Ticks < BreakTime) return;

        Destroyed = true;
        var part = Main.World.SpawnDeathParticle(this, 4);
        part.Vel = (0, 0);
    }

    public override void Collide(Entity other, Dir dir)
    {
        if (other is Player && dir == Dir.Up && !Broken) {
            Broken = true;
            BreakTime = Ticks + CrumbleTime;
        }
    }
}