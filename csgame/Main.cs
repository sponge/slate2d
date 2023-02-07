using ImGuiNET;
using Slate2D;

// skipped
// assetlist
// entmap - use decorators instead!
// fsmentity
// game - nontrivial
// main - little bits
// objectpool
// printwin

public interface IScene
{
    void Update(double dt);
    void Draw();
}

public static class Main
{
    static IScene? scene;

    public static void SwitchLevel(int num)
    {
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
        scene?.Update(dt);
    }

    public static void Draw()
    {
        scene?.Draw();
    }

    public static Game World()
    {
        if (scene?.GetType() != typeof(Game))
        {
            throw new Exception("Current scene isn't game");
        }

        return scene as Game;
    }
}