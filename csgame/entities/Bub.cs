using Slate2D;
namespace Bub;

enum Frames
{
    Idle = 0,
    Pain,
    Run1,
    Run2,
    Run3,
    Run4,
    Run5,
    Run6,
    Run7,
}

[Spawnable]
class Bub : Entity
{
    protected bool Spikey = false;
    protected bool Chargin = false;

    public Bub(LDTKEntity ent) : base(ent)
    {
        Sprite = Assets.Find("bub");
        DrawOfs = (-2, -5);

        Vel.X = (ent.Properties["GoRight"]?.Bool ?? true) ? 0.25f : -0.25f;
        FlipBits = (byte)(Vel.X > 0 ? 1 : 0);
    }

    public override void Die()
    {
        base.Die();
        Main.World.SpawnDeathParticle(this, (uint)Frames.Idle);
    }

    public override CollisionType CanCollide(Entity other, Dir dir) => StandardEnemyCanCollide(other, dir);

    public override void Update(uint ticks, float dt)
    {
        var grounded = Vel.Y >= 0 && CollideAt(Pos.X, Pos.Y + 1, Dir.Down);

        Vel.Y = grounded ? 0 : Vel.Y + Phys.EnemyGravity;

        var extra = Chargin && Math.Abs(Center.Y - Main.World.Player.Center.Y) < 10 ? 3 : 1;

        MoveX(Vel.X * extra);
        MoveY(Vel.Y);

        FlipBits = (byte)(Vel.X > 0 ? 1 : 0);

        if (Vel.Y != 0)
        {
            Frame = (uint)Frames.Pain;
        }
        else if (grounded)
        {
            var frameSpeed = extra > 1 ? 4 : 8;
            Frame = (uint)((ticks / frameSpeed) % ((uint)Frames.Run7 - (uint)Frames.Run1) + (uint)Frames.Run1);
        }
    }

    public override void Collide(Entity other, Dir dir)
    {
        if (other is Player)
        {
            if (!Spikey) HandlePlayerStomp(other, dir);
            else other.Hurt(1);
        }
        else
        {
            if (Vel.X < 0 && dir == Dir.Left) Vel.X *= -1;
            else if (Vel.X > 0 && dir == Dir.Right) Vel.X *= -1;
            if (dir == Dir.Up || dir == Dir.Down) Vel.Y = 0;
        }
    }
}

[Spawnable]
class SpikeyBub : Bub
{
    public SpikeyBub(LDTKEntity ent) : base(ent)
    {
        Spikey = true;
        Sprite = Assets.Find("spikeybub");
    }

    public override CollisionType CanCollide(Entity other, Dir dir)
    {
        if (other is Player) return CollisionType.Trigger;
        else return CollisionType.Enabled;
    }
}

[Spawnable]
class CharginBub : Bub
{
    public CharginBub(LDTKEntity ent) : base(ent)
    {
        Chargin = true;
        Sprite = Assets.Find("charginbub");
    }
}