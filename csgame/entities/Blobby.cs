using Slate2D;

enum BlobbyStates
{
    None,
    Idle,
    Sink,
    Rise,
    Move
}

[Spawnable("Blobby")]
class Blobby : FSMEntity<BlobbyStates>
{
    enum Frames
    {
        Idle1,
        Idle2,
        Sink1,
        Sink2,
        Sunk,
        Pain
    }

    public Blobby(LDTKEntity ent) : base(ent)
    {
        float LastVelX = -1.5f;

        Sprite = Assets.Find("blobby");

        Handlers = new()
        {
            {None, new FSMState {
                Enter = () => FSMDefaultTransitionTo(BlobbyStates.Idle),
                CanCollide = StandardEnemyCanCollide,
                Collide = (Entity other, Dir dir) => HandlePlayerStomp(other, dir)
            }},

            {BlobbyStates.Idle, new FSMState {
                Enter = () => FSMTransitionAtTime(BlobbyStates.Sink, 40),
                Update = (uint ticks) => { Frame = (uint)(ticks % 40 <= 20 ? Frames.Idle1 : Frames.Idle2); },
            }},

            // FIXME: port over animation frame stuff
            {BlobbyStates.Sink, new FSMState {
                Enter = () => FSMTransitionAtTime(BlobbyStates.Move, 20),
                Update = (uint ticks) => { Frame = (uint)Frames.Sink1; },
            }},

            {BlobbyStates.Rise, new FSMState {
                Enter = () => FSMTransitionAtTime(BlobbyStates.Idle, 20),
                Update = (uint ticks) => { Frame = (uint)Frames.Sink2; },
            }},

            {BlobbyStates.Move, new FSMState {
                Enter = () =>
                {
                    FSMTransitionAtTime(BlobbyStates.Rise, 60);
                    Vel.X = LastVelX;
                    Frame = (uint)Frames.Sunk;
                },
                Exit = () =>
                {
                    LastVelX = Vel.X;
                    Vel.X = 0;
                },
                CanCollide = (Entity other, Dir dir) => other is Player ? CollisionType.Trigger : CollisionType.Enabled,
                Collide = (Entity other, Dir dir) =>
                {
                    if (other is Player)
                    {
                        other.Hurt(1);
                        return;
                    }

                    if (Vel.X < 0 && dir == Dir.Left) Vel.X *= -1;
                    else if (Vel.X > 0 && dir == Dir.Right) Vel.X *= -1;

                    if (dir == Dir.Up || dir == Dir.Down) Vel.Y = 0;
                }
            }},
        };
    }

    public override void Die()
    {
        base.Die();
        Main.World.SpawnDeathParticle(this, (uint)Frames.Pain);
    }

    public override void Update(uint ticks, float dt)
    {
        var grounded = Vel.Y >= 0 && CollideAt(Pos.X, Pos.Y + 1, Dir.Down);
        Vel.Y = grounded ? 0 : Vel.Y + Phys.EnemyGravity;

        FSMUpdate(ticks);

        MoveX(Vel.X);
        MoveY(Vel.Y);
    }

    public override CollisionType CanCollide(Entity other, Dir dir)
    {
        return FSMCanCollide(other, dir);
    }

    public override void Collide(Entity other, Dir dir)
    {
        FSMCollide(other, dir);
    }
}