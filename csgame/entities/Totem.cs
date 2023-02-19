namespace Totem;
using Slate2D;

enum Frames
{
    Idle,
    Walk1,
    Walk2,
    Half1,
    Half2,
    Pain,
    Chunk,
}

[Spawnable]
class Totem : Entity
{
    static Frames[] WalkAnim = new[] { Frames.Walk1, Frames.Walk2 };
    static Frames[] HalfWalkAnim = new[] { Frames.Half1, Frames.Half2 };
    bool Half = false;

    public Totem(LDTKEntity ent) : base(ent)
    {
        Sprite = Assets.Find("totem");
        Vel = (-0.25f, 0);
        DrawOfs = (-2, -2);
    }

    public override void Die()
    {
        base.Die();
        Main.World.SpawnDeathParticle(this, (uint)Frames.Pain);
    }

    public override void Update(uint ticks, float dt)
    {
        var frames = Half ? HalfWalkAnim : WalkAnim;
        var animSpeed = Half ? 4 : 8;
        Frame = (uint)frames[Ticks / animSpeed % frames.Length];
        MoveX(Vel.X);
        FlipBits = (byte)(Vel.X > 0 ? 1 : 0);
    }

    public override CollisionType CanCollide(Entity other, Dir dir) => StandardEnemyCanCollide(other, dir);

    public override void Collide(Entity other, Dir dir)
    {
        if (HandlePlayerStomp(other, dir, false))
        {
            if (Half)
            {
                Die();
                return;
            }

            var wood = Main.World.SpawnDeathParticle(this, (uint)Frames.Chunk);
            wood.Size = (13, 13);

            Half = true;
            Vel.X *= 3;
            Size.H -= 6;
            Pos.Y += 6;
            DrawOfs.Y -= 6;
        }
        else
        {
            Vel.X *= -1;
        }
    }
}