using Slate2D;

[Spawnable]
class Cannonball : Entity
{
    public Cannonball()
    {
        Type = "Cannonball";
        Sprite = Assets.Find("launcher");
        Frame = 1;
        Size = (14, 14);
        DrawOfs = (-2, -2);
    }

    public override CollisionType CanCollide(Entity other, Dir dir) => StandardEnemyCanCollide(other, dir);

    public override void Collide(Entity other, Dir dir)
    {
        HandlePlayerStomp(other, dir);
        Die();
    }

    public override void Die()
    {
        base.Die();
        var deathEnt = Main.World.SpawnDeathParticle(this, 1);
        deathEnt.Vel = (0, 0);
        deathEnt.Rotate = false;
    }

    public override void Update(uint ticks, float dt)
    {
        MoveX(Vel.X);
        MoveY(Vel.Y);
    }
}