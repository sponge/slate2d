using Slate2D;

[Spawnable]
class Platform : Entity
{
    bool Moving;
    uint Dim;
    float Speed;
    int Start, End;
    bool OneShot;

    public Platform(LDTKEntity ent) : base(ent)
    {
        Layer = Layer.Background;



        Moving = ent.Properties.GetValueOrDefault("Moving", null)?.Bool ?? true;
        Name = ent.Properties.GetValueOrDefault("Name", null)?.Str ?? "";
        Dim = ent.Properties.GetValueOrDefault("Direction", null)?.Str == "Horizontal" ? 0u : 1u;
        Speed = ent.Properties.GetValueOrDefault("Speed", null)?.Num ?? 1;
        OneShot = ent.Properties.GetValueOrDefault("OneShot", null)?.Bool ?? false;

        int a = Dim == 0 ? Pos.X : Pos.Y;
        int b = a + (ent.Properties["Distance"]?.NumI ?? 100);
        Speed *= a > b ? -1 : 1;
        Start = Math.Min(a, b);
        End = Math.Max(a, b);
    }

    public override void Die() { }
    public override void Hurt(int number) { }

    public override void Update(uint ticks, float dt)
    {
        if (!Moving) return;

        float x = Dim == 0 ? Speed : 0;
        float y = Dim == 1 ? Speed : 0;
        MoveSolid(x, y);

        ref var pos = ref (Dim == 0 ? ref Pos.X : ref Pos.Y);
        if (pos <= Start || pos >= End)
        {
            Speed *= -1;
            if (OneShot) Moving = false;
        }
    }

    public override void Activate(Entity other)
    {
        Moving = !Moving;
    }

    public override void Draw()
    {
        DC.SetColor(251, 242, 54, 255);
        DC.Rect(Pos.X, Pos.Y, Size.W, Size.H, false);
    }
}