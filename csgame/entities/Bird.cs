using Slate2D;

[Spawnable]
class Bird : Entity
{
    (int X, int Y) Delta = (0, 0);
    (float X, float Y) MoveDir = (0, 0);
    (float X, float Y) MoveAmt = (0, 0);

    public Bird(LDTKEntity ent) : base(ent)
    {
        Sprite = Assets.Find("bird");
        DrawOfs = (-1, -2);

        (int X, int Y) dest = ((int X, int Y))((ent.Properties["Destination"]?.Point) ?? (Pos.X, Pos.Y));
        (int X, int Y) tilePos = ((int X, int Y))(MathF.Floor(Pos.X / 16), MathF.Floor(Pos.Y / 16));

        Delta.X = (dest.X - tilePos.X) * 16;
        Delta.Y = (dest.Y - tilePos.Y) * 16;

        (float X, float Y) absDelta = (Math.Abs(Delta.X), Math.Abs(Delta.Y));

        float dx = absDelta.X > absDelta.Y ? 1 : absDelta.X / absDelta.Y;
        float dy = absDelta.Y > absDelta.X ? 1 : absDelta.Y / absDelta.X;

        dx *= Math.Sign(Delta.X) / 2f;
        dy *= Math.Sign(Delta.Y) / 2f;

        MoveDir = (dx, dy);
        WorldCollide = false;
    }

    public override void Die()
    {
        base.Die();
        Main.World.SpawnDeathParticle(this, 3);
    }

    public override CollisionType CanCollide(Entity other, Dir dir)
    {
        if (other is Player && dir == Dir.Up) return CollisionType.Platform;
        else return CollisionType.Trigger;
    }

    public override void Update(uint ticks, float dt)
    {
        Frame = ticks / 8 % 4;
        Frame = Frame == 3 ? 1 : Frame;

        MoveX(MoveDir.X);
        MoveY(MoveDir.Y);
        MoveAmt.X += MoveDir.X;
        MoveAmt.Y += MoveDir.Y;

        var dim = Delta.X > Delta.Y ? 0 : 1;
        ref var moveAmt = ref (dim == 0 ? ref MoveAmt.X : ref MoveAmt.Y);
        ref var delta = ref (dim == 0 ? ref Delta.X : ref Delta.Y);

        if (Math.Abs(moveAmt) >= Math.Abs(delta))
        {
            MoveDir.X *= -1;
            MoveDir.Y *= -1;
            MoveAmt = (0, 0);
        }
    }

    public override void Collide(Entity other, Dir dir)
    {
        HandlePlayerStomp(other, dir);
    }

}