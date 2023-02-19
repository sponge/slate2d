namespace Slime;
using Slate2D;

enum Frames
{
    Idle,
    Blink,
    Pain,
    Crouch,
    Jump,
    Fall
}

[Spawnable]
class Slime : Entity
{
    uint NextJump = 120;
    bool Jumping = false;
    uint LandTime = 0;
    int MoveDir;

    public Slime(LDTKEntity ent) : base(ent)
    {
        Sprite = Assets.Find("slime");
        DrawOfs = (-1, -6);
        FlipBits = 1;
        MoveDir = ent.Properties.GetValueOrDefault("GoRight", null)?.Bool ?? true ? 1 : -1;
    }

    public override void Die()
    {
        base.Die();
        Main.World.SpawnDeathParticle(this, (uint)Frames.Pain);
    }

    public override CollisionType CanCollide(Entity other, Dir dir) => StandardEnemyCanCollide(other, dir);

    public override void Update(uint ticks, float dt)
    {
        var grounded = Vel.Y >= 0 && CollideAt(Pos.X, Pos.Y + 1, Dir.Down);
        Vel.Y = grounded ? 0 : Vel.Y + Phys.EnemyGravity;

        if (grounded)
        {
            if (Jumping)
            {
                NextJump = ticks + 120;
                LandTime = ticks;
            }

            Jumping = false;
            Vel.X = 0;
        }
        else
        {
            Jumping = true;
        }

        if (!Jumping && ticks > NextJump)
        {
            Vel = (MoveDir, -3);
            Jumping = true;
        }

        MoveX(Vel.X);
        MoveY(Vel.Y);

        FlipBits = (byte)(MoveDir < 0 ? 1 : 0);

        if (Vel.Y < 0)
        {
            Frame = (uint)Frames.Jump;
        }
        else if (Vel.Y > 0)
        {
            Frame = (uint)Frames.Fall;
        }
        else if (grounded && (NextJump - 20 <= ticks || ticks <= LandTime + 10))
        {
            Frame = (uint)Frames.Crouch;
        }
        else if (grounded)
        {
            Frame = (uint)(ticks % 40 > 30 ? Frames.Blink : Frames.Idle);
        }
    }

    public override void Collide(Entity other, Dir dir)
    {
        if (HandlePlayerStomp(other, dir)) return;
        if (dir != Dir.Left && dir != Dir.Right) return;

        MoveDir *= -1;
        Vel.X *= -0.5f;
    }


}