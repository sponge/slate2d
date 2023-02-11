using ImGuiNET;
using System.Numerics;

public class PrintWin
{
    static private void Print(string title, object key, object value)
    {
        ImGui.SetNextWindowSize(new Vector2(250, 500), ImGuiCond.FirstUseEver);
        ImGui.Begin(title, ImGuiWindowFlags.HorizontalScrollbar | ImGuiWindowFlags.NoFocusOnAppearing);

        float width = ImGui.GetWindowContentRegionMax().X;
        float keyWidth = ImGui.CalcTextSize(key.ToString()).X;
        float valWidth = ImGui.CalcTextSize(value.ToString()).X;

        ImGui.Text(key.ToString());
        if (keyWidth + valWidth + 20 < width)
        {
            ImGui.SameLine();
        }

        int x = (int)(width - valWidth);
        x = x < 5 ? 5 : x;
        ImGui.SetCursorPosX(x);
        ImGui.Text(value.ToString());
        ImGui.Separator();
        ImGui.End();
    }

    static List<(string, object, object)> Messages = new();
    static public bool Retained = true;

    static public void Dbg(string title, object key, object val)
    {
        if (Retained) Messages.Add((title, key, val));
        else Print(title, key, val);
    }

    static public void DrawPrintWin()
    {
        foreach (var msg in Messages)
        {
            Print(msg.Item1, msg.Item2, msg.Item3);
        }
    }

    static public void ClearPrintWin()
    {
        Messages.Clear();
    }
}