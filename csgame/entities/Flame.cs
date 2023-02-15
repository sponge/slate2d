using Slate2D;

[Spawnable]
class Flame : Entity
{
    public bool StartOn = false;

    public Flame()
    {
        Collidable = CollisionType.Trigger;
        Size = (16, 47);
        Sprite = Assets.Find("flame");
        StartOn = false;
    }

    public override void Die() { }

    public override void Collide(Entity other, Dir dir)
    {
        if (other is Player == false) return;
        if (Frame != 5 && Frame != 6) return;

        other.Hurt(1);
    }

    public override void Update(uint ticks, float dt)
    {
        var cyc = (Ticks + (StartOn ? 0 : 240)) % 480;
        uint frame = (uint)(cyc % 8 < 4 ? 0 : 1);
        var offset = Math.Abs(270 - cyc);

        if (offset < 60)
        {
            Frame = 5 + frame;
        }
        else if (offset < 70)
        {
            Frame = 3 + frame;
        }
        else if (offset < 100)
        {
            Frame = 1 + frame;
        }
        else
        {
            Frame = 0;
        }
    }
}