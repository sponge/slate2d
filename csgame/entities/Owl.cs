namespace Owl;
using Slate2D;

enum Frames
{
    Idle1 = 0,
    Idle2,
    FlapUp,
    FlapMid,
    FlapDown,
    Pain
}

enum States
{
    Default,
    Idle,
    Rise,
    Float,
    Fall
}

[Spawnable]
class Owl : FSMEntity<States>
{
    static Frames[] FlapAnim = new[] { Frames.FlapMid, Frames.FlapUp, Frames.FlapMid, Frames.FlapDown };
    static Frames[] FallAnim = new[] { Frames.FlapMid, Frames.FlapDown };

    public Owl(LDTKEntity ent) : base(ent)
    {
        Sprite = Assets.Find("owl");
        DrawOfs = (-3, -1);
        FSMTransitionTo(States.Idle);
    }

    public override void Die()
    {
        base.Die();
        Main.World.SpawnDeathParticle(this, (uint)Frames.Pain);
    }

    CollisionType Default_CanCollide(Entity other, Dir dir) => StandardEnemyCanCollide(other, dir);
    void Default_Collide(Entity other, Dir dir) => HandlePlayerStomp(other, dir);

    void Idle_Enter() => FSMTimer(States.Rise, 120);
    void Idle_Update(uint ticks, float dt) => Frame = (uint)(ticks % 40 < 30 ? Frames.Idle1 : Frames.Idle2);

    void Rise_Enter() => FSMTimer(States.Float, 60);
    void Rise_Update(uint ticks, float dt)
    {
        Frame = (uint)FlapAnim[ticks / 4 % FlapAnim.Length];
        MoveY(-1);
    }

    void Float_Enter() => FSMTimer(States.Fall, 90);
    void Float_Update(uint ticks, float dt) => Frame = (uint)FlapAnim[ticks / 8 % FlapAnim.Length];

    void Fall_Update(uint ticks, float dt)
    {
        if (!MoveY(0.5f))
        {
            FSMTransitionTo(States.Idle);
        }

        Frame = (uint)FallAnim[ticks / 16 % FallAnim.Length];
    }
}