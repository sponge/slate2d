// NOTE: hot reload doesn't work in this file
using ImGuiNET;
using Slate2D;
using System.Numerics;

SLT.Init(args);

Main.Startup();

while (true)
{
    var dt = SLT.StartFrame();
    if (dt < 0)
    {
        SLT.EndFrame();
        break;
    }
    Timer.Start("total");

    Timer.Start("update");
    Main.Update(dt);
    var updTime = Timer.End("update").ToString("0.000");

    Timer.Start("draw");
    Main.Draw();
    var drawTime = Timer.End("draw").ToString("0.000");

    Timer.Start("submit");
    DC.Submit();
    var submitTime = Timer.End("submit").ToString("0.000");

    var totalTime = Timer.End("total").ToString("0.000");

    var res = SLT.GetResolution();
    ImGui.SetNextWindowPos(new Vector2(res.W - 200, 50));
    ImGui.SetNextWindowSize(new Vector2(200, 0));
    ImGui.Begin("##fps2", ImGuiWindowFlags.NoDecoration | ImGuiWindowFlags.NoMove | ImGuiWindowFlags.NoSavedSettings | ImGuiWindowFlags.NoInputs);
    ImGui.Text($"upd: {updTime} (5s: {Timer.Max("update").ToString("0.000")})");
    ImGui.Text($"drw: {drawTime} (5s: {Timer.Max("draw").ToString("0.000")})");
    ImGui.Text($"sub: {submitTime} (5s: {Timer.Max("submit").ToString("0.000")})");
    ImGui.Text($"sum: {totalTime} (5s: {Timer.Max("total").ToString("0.000")})");
    ImGui.SetWindowPos(new Vector2(0, 0), ImGuiCond.Always);
    ImGui.End();

    SLT.EndFrame();
    SLT.UpdateLastFrameTime();
}

SLT.Shutdown();