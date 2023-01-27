using ImGuiNET;
using Slate2D;
using System;

public class Game : IScene
{
    public Game(string mapName)
    {
    }

    public void Update(double dt)
    {
    }

    public void Draw()
    {
        DC.Clear(30, 30, 30, 255);
        DC.Rect(200, 200, 20, 20, false);
        DC.Rect(300, 300, 20, 20, true);
        DC.Submit();
    }
}