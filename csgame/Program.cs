using Slate2D;
using ImGuiNET;

SLT.Init(args);

while (true)
{
    var dt = SLT.StartFrame();
    if (dt < 0)
    {
        SLT.EndFrame();
        break;
    }

    ImGui.Begin("hello");
    ImGui.Button("button");
    ImGui.End();

    Game.Update(dt);

    Game.Draw();

    DC.Submit();

    SLT.EndFrame();
    SLT.UpdateLastFrameTime();
}

SLT.Shutdown();