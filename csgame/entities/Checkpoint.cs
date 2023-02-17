namespace Checkpoint;
using Slate2D;

enum Frames
{
    Idle1 = 0,
    Idle2,
    Idle3,
    Activate1,
    Activate2,
    Activate3,
    Active1,
    Active2,
    Active3,
}

enum States
{
    None,
    Idle,
    Bounce,
    Active,
}

[Spawnable]
class Checkpoint : FSMEntity<States>
{
    static Frames[] IdleAnim = new[] { Frames.Idle1, Frames.Idle2, Frames.Idle3 };
    static Frames[] BounceAnim = new[] { Frames.Activate1, Frames.Activate2, Frames.Activate3 };
    static Frames[] ActiveAnim = new[] { Frames.Active1, Frames.Active2, Frames.Active3 };

    bool Activated = false;
    public Checkpoint(LDTKEntity ent) : base(ent)
    {
        Sprite = Assets.Find("checkpoint");
        Collidable = CollisionType.Trigger;
        DrawOfs = (-1, 0);
        Layer = Layer.Background;
        FSMTransitionTo(States.Idle);
    }

    void Idle_Update(uint ticks, float dt) => Frame = (uint)IdleAnim[ticks / 8 % IdleAnim.Length];
    void Idle_Collide(Entity other, Dir dir)
    {
        if (other is Player == false) return;

        Activated = true;
        Main.World.GameState.CheckpointActive = true;
        Main.World.GameState.CheckpointPos = (Pos.X, Pos.Y + Size.H);
        Main.World.SpawnPuffParticle(Pos.X, Pos.Y);
        FSMTransitionTo(States.Bounce);
    }

    void Bounce_Enter() => FSMTimer(States.Active, 12);
    void Bounce_Update(uint ticks, float dt) => Frame = Animate(BounceAnim);

    void Active_Update(uint ticks, float dt) => Frame = (uint)ActiveAnim[ticks / 8 % ActiveAnim.Length];
}