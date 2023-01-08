using ImGuiNET;
using Slate2D;

public static class Game
{
	public static void Update(double dt)
	{

	}

	public static void Draw()
	{
		DC.Clear(30, 30, 30, 255);
		DC.Rect(200, 200, 20, 20, false);
        DC.Rect(300, 300, 20, 20, true);

		if (ImGui.Begin("resolution"))
		{
            Dimensions res = SLT.GetResolution();
			ImGui.Text($"{res.w}x{res.h}");
			ImGui.End();
		}
    }
}