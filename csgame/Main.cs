// skipped
// fsmentity
// main - little bits
// objectpool

public interface IScene
{
    void Update(float dt);
    void Draw();
}

public static class Main
{
    static IScene? scene;

    public static void SwitchLevel(uint num, (int X, int Y)? pos = null)
    {
        if (pos != null) throw new NotImplementedException();

        scene = new Game($"maps/map{num}.ldtkl");
        GC.Collect();
    }

    public static void Startup()
    {
        Util.RegisterButtons();
        SwitchLevel(1);
    }

    public static void Update(double dt)
    {
        scene?.Update((float)dt);
    }

    public static void Draw()
    {
        scene?.Draw();
    }

    public static Game World
    {
        get
        {
            if (scene?.GetType() != typeof(Game))
            {
                throw new Exception("Current scene isn't game");
            }

            return (Game)scene;
        }
    }
}