using Slate2D;

[Spawnable]
class Balloon : Entity
{
    public Balloon(LDTKEntity ent) : base(ent)
    {
        Sprite = Assets.Find("balloon");

    }

    public override void Update(uint ticks, float dt)
    {
        Frame = (uint)(ticks % 26 < 13 ? 0 : 1);
        var cycle = ticks % 120;
        if (cycle == 0 || cycle == 30)
        {
            MoveY(1);
        }
        else if (cycle == 60 || cycle == 90)
        {
            MoveY(-1);
        }
    }

    public override CollisionType CanCollide(Entity other, Dir dir)
    {
        if (other is Player) return CollisionType.Platform;
        else return CollisionType.Disabled;
    }

    public override void Collide(Entity other, Dir dir)
    {
        if (other is Player && dir == Dir.Up)
        {
            Die();
            (other as Player)?.StompEnemy();
            Main.World.SpawnPuffParticle(Pos.X, Pos.Y);
        }
    }
}
