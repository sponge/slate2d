using Slate2D;

[Spawnable]
public class Player : Entity
{
    // entity state
    (int X, int Y) SpawnPos;
    bool Dead = false;
    uint DeadTime = 0;
    public bool DisableControls = false;
    public bool DisableMovement = false;
    int PMeter = 0;
    uint FallingFrames = 0;
    public bool JumpHeld = false;
    uint JumpHeldFrames = 0;
    int Facing = 1;
    bool Stunned = false;
    uint StunTime = 0;
    public int Health = 3;
    public int MaxHealth { get; private set; } = 3;

    public Player(LDTKEntity ent) : base(ent)
    {
        Sprite = Assets.Find("dogspr");
        DrawOfs = (-5, -4);
        Layer = Layer.Foreground;
        SpawnPos = Pos;
    }

    public override void Hurt(int amt)
    {
        if (Stunned) return;
        Health -= amt;
        if (Health <= 0)
        {
            PMeter = 0;
            Die();
        }
        else
        {
            Stunned = true;
            StunTime = Ticks + 120;
        }
    }

    public override void Die()
    {
        if (Dead) return;
        Dead = true;
        DeadTime = Ticks + 120;
        var part = Main.World.SpawnDeathParticle(this, 0);
        part.Vel.Y = -5;
    }

    float GetJumpHeight(float speed)
    {
        speed = MathF.Abs(speed);
        foreach (var check in Phys.JumpHeights)
        {
            if (speed >= check.Speed)
            {
                return check.Height;
            }
        }

        return Phys.JumpHeights[0].Height;
    }

    public void StompEnemy()
    {
        Vel.Y = Input.ButtonPressed((int)Buttons.Jump) ? -Phys.EnemyJumpHeld : -Phys.EnemyJump;
        JumpHeld = true;
    }

    public override void Update(uint ticks, float dt)
    {
        if (Dead)
        {
            if (ticks > DeadTime) Main.World.FailLevel();
            return;
        }

        var grounded = Vel.Y >= 0 && CollideAt(Pos.X, Pos.Y + 1, Dir.Down);

        if (Main.World.GameState.LevelComplete)
        {
            DisableControls = true;
        }

        // reenable controls if not on spring, usually due to squishing an enemy
        else if (DisableControls && CollideEnt?.Type != "Spring")
        {
            DisableControls = false;
            DisableMovement = false;
        }

        var dir = DisableControls ? 0 : Input.ButtonPressed((int)Buttons.Left) ? -1 : Input.ButtonPressed((int)Buttons.Right) ? 1 : 0;
        var jumpPress = DisableControls ? false : Input.ButtonPressed((int)Buttons.Jump);
        var shootPress = DisableControls ? false : Input.ButtonPressed((int)Buttons.Shoot);
        var slidePress = DisableControls ? false : Input.ButtonPressed((int)Buttons.Down);

        // remove stun effect if it's time
        if (Stunned && ticks > StunTime)
        {
            Stunned = false;
            StunTime = 0;
        }

        // checking again because if we're standing on something it should trigger
        if (grounded && CollideEnt != null)
        {
            CollideEnt.Collide(this, Dir.Up);
            grounded = Vel.Y >= 0 && CollideAt(Pos.X, Pos.Y + 1, Dir.Down);
        }

        // if we're still on the ground, blank out the decimal
        if (grounded)
        {
            Remainder.Y = 0;
        }

        // set direction for bullets and sprite drawing
        Facing = dir != 0 ? dir : Facing;

        // track frames since leaving platform for late jump presses
        FallingFrames = grounded ? 0 : FallingFrames + 1;

        // let players jump a few frames early but don't let them hold the button down
        JumpHeldFrames = jumpPress ? JumpHeldFrames + 1 : 0;
        if (!jumpPress && JumpHeld)
        {
            JumpHeld = false;
        }

        // apply gravity if not on the ground. different gravity values depending on holding jump
        Vel.Y = grounded ? 0 : Vel.Y + (JumpHeld ? Phys.HeldGravity : Phys.Gravity);

        // slide on ground if holding down
        // y vel handled by slope snapping near end of move
        // this tileAt is used because otherwise collideTile is one pixel below
        // your feet, which may be the tile underneath the slope
        Tile tid = TileAt(BottomMiddle.X, BottomMiddle.Y);
        var canSlide = tid == Tile.SlopeL || tid == Tile.SlopeR;

        if (!JumpHeld && slidePress && canSlide)
        {
            var slideDir = tid == Tile.SlopeL ? -1 : 1;
            Vel.X += slideDir * Phys.SlideAccel;
            Remainder = (0, 0);
        }
        else if (slidePress || (dir == 0 && Vel.X != 0 && grounded))
        {
            // if not pushing anything, slow down if on the ground
            Vel.X += Phys.Friction * -Math.Sign(Vel.X);
            if (Math.Abs(Vel.X) < Phys.Friction)
            {
                Vel.X = 0;
            }
        }
        else if (dir != 0)
        {
            // if holding a direction, figure out how fast we should try and go
            var speed = Math.Sign(dir * Vel.X) == -1 ? Phys.SkidAccel : Phys.Accel;
            Vel.X += speed * dir;
        }

        // if jump is held, and player has let go of it since last jump
        if (jumpPress && !JumpHeld)
        {
            // allow the jump if:
            // - they're on the ground, and haven't been holding for too long
            // - they're not on the ground, but have recently been on the ground
            if ((grounded && JumpHeldFrames < Phys.EarlyJumpFrames) || (!grounded && FallingFrames < Phys.LateJumpFrames))
            {
                var height = GetJumpHeight(Vel.X);
                Vel.Y = -height;
                JumpHeld = true;
                grounded = false;
                // this.jumpHnd = SLT.sndPlay(this.jumpSound); // TODO: audio
            }
        }

        // increment the p-meter if you're on the ground and going fast enough
        if (Math.Abs(Vel.X) >= Phys.RunSpeed && grounded)
        {
            PMeter += 2;
        }
        else if (grounded || PMeter != Phys.PMeterCapacity)
        {
            // tick down the p-meter, but don't if you're at 100% and midair
            PMeter -= 1;
        }

        PMeter = Math.Clamp(PMeter, 0, Phys.PMeterCapacity);

        // hard cap speed values
        if (PMeter == Phys.PMeterCapacity)
        {
            Vel.X = Math.Clamp(Vel.X, -Phys.MaxSpeed, Phys.MaxSpeed);
        }
        else
        {
            Vel.X = Math.Clamp(Vel.X, -Phys.RunSpeed, Phys.RunSpeed);
        }

        Vel.Y = Math.Min(Vel.Y, Phys.TerminalVelocity);

        // move x first, then move y
        if (!DisableMovement)
        {
            if (!MoveX(Vel.X))
            {
                Vel.X = Remainder.X = 0;
            }

            // snap to ground if in a slope
            CollideAt(Pos.X, Pos.Y + 1, Dir.Down);
            var velY = grounded && AnyInSlope ? 10 : Vel.Y;
            // this.moveY may alter our velocity, so double check vel before zeroing it out
            if (!MoveY(velY) && Math.Sign(velY) == Math.Sign(Vel.Y))
            {
                Vel.Y = Remainder.Y = 0;
            }
        }

        // set animation
        var animSpeed = PMeter == Phys.PMeterCapacity ? 4 : 8;
        Frame = (uint)(Vel.X == 0 ? 0 : ticks / animSpeed % 6);
        FlipBits = (byte)(Facing < 0 ? 1 : 0);
    }

    public override void Draw()
    {
        if (Dead) return;
        DC.SetColor(255, 255, 255, (byte)(Stunned ? 128 : 255));
        DC.Sprite(Sprite, Frame, Pos.X + DrawOfs.X, Pos.Y + DrawOfs.Y, 1, FlipBits, 1, 1);
        DC.SetColor(255, 255, 255, 255);
    }
}