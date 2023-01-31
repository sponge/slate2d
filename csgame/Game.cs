using ImGuiNET;
using Slate2D;
using System;
using System.Collections;

public class Game : IScene
{
    public Game(string mapName)
    {
        string mapStr = FS.ReadTextFile(mapName);
        var ldtk = new LDTK(mapStr);
        // FIXME: freefile?
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