namespace Daikon;
using Slate2D;

enum Frames
{
    Idle,
    Rise,
    Fall,
    Pain
}

enum States
{
    Default,
    Wait,
    Pop,
    Recharge
}

[Spawnable]
class Daikon : FSMEntity<States>
{
    public Daikon(LDTKEntity ent) : base(ent)
    {
        Sprite = Assets.Find("daikon");
        DrawOfs = (-2, -3);
        FSMTransitionTo(States.Wait);
    }

    public override void Die()
    {
        base.Die();
        Main.World.SpawnDeathParticle(this, (uint)Frames.Pain);
    }

    void Default_Collide(Entity other, Dir dir)
    {
        HandlePlayerStomp(other, dir);
        if (other is WorldEnt) {
            FSMTransitionTo(States.Recharge);
        }
    }

    void Wait_Enter() => Frame = (uint)Frames.Idle;
    void Wait_Update(uint ticks, float dt)
    {
        if (Math.Abs(Main.World.Player.Center.X - Center.X) < 40)
        {
            FSMTransitionTo(States.Pop);
        }
    }
    CollisionType Wait_CanCollide(Entity other, Dir dir) => CollisionType.Disabled;

    void Pop_Enter()
    {
        Vel.Y = -3;
        Frame = (uint)Frames.Rise;
    }
    void Pop_Update(uint ticks, float dt)
    {
        Vel.Y += Phys.EnemyGravity;
        MoveY(Vel.Y);
    }
    CollisionType Pop_CanCollide(Entity other, Dir dir)
    {
        if (other is Player && dir == Dir.Up) return CollisionType.Platform;
        else if (other is Player) return CollisionType.Trigger;
        else return CollisionType.Disabled;
    }

    void Recharge_Enter()
    {
        FSMTimer(States.Wait, 120);
        Frame = (uint)Frames.Idle;
    }
    void Recharge_Collide(Entity other, Dir dir)
    {
        HandlePlayerStomp(other, dir);
        if (other is WorldEnt) FSMTransitionTo(States.Recharge);
    }
}