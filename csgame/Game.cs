using ImGuiNET;
using Slate2D;
using System;
using System.Collections;

public class Game : IScene
{
    LDTK ldtk;

    public Game(string mapName)
    {
        string mapStr = FS.ReadTextFile(mapName);
        ldtk = new LDTK(mapStr);
        // FIXME: freefile?
    }

    public void Update(double dt)
    {
    }

    public void Draw()
    {
        DC.Clear(30, 30, 30, 255);

        DC.SetColor(255, 255, 255, 255);
        ldtk.Draw("BGTiles");
        ldtk.Draw("BGWorld");
        ldtk.Draw("BGDecoration");
        ldtk.Draw("Collision");
        DC.SetColor(255, 255, 255, 255);
        DC.Submit();
    }
}