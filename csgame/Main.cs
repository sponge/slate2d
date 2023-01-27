using ImGuiNET;
using Slate2D;

// skipped
// assetlist
// entity
// entmap - use decorators instead!
// fsmentity
// game - nontrivial
// ldtk
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
        scene = new Game($"maps/Map{num}.ldtkl");
        GC.Collect();
    }

    public static void Startup()
    {
        SwitchLevel(1);
    }

    public static void Update(double dt)
    {
        scene?.Update(dt);
    }

    public static void Draw()
    {
        if (ImGui.Begin("Stuff!"))
        {
            if (ImGui.BeginTable("stats", 2))
            {
                ImGui.TableSetupColumn("Key");
                ImGui.TableSetupColumn("Value");
                ImGui.TableHeadersRow();

                var res = SLT.GetResolution();
                ImGui.TableNextColumn();
                ImGui.Text("Resolution");
                ImGui.TableNextColumn();
                ImGui.Text($"{res.w}x{res.h}");

                var mouse = Input.MousePosition();
                ImGui.TableNextColumn();
                ImGui.Text("Mouse");
                ImGui.TableNextColumn();
                ImGui.Text($"{mouse.x},{mouse.y}");

                var analog = Input.ControllerAnalog(0);
                ImGui.TableNextColumn();
                ImGui.Text("Left Stick");
                ImGui.TableNextColumn();
                ImGui.Text($"{analog.leftX},{analog.leftY}");

                ImGui.TableNextColumn();
                ImGui.Text("Right Stick");
                ImGui.TableNextColumn();
                ImGui.Text($"{analog.rightX},{analog.rightY}");

                ImGui.TableNextColumn();
                ImGui.Text("Triggers");
                ImGui.TableNextColumn();
                ImGui.Text($"{analog.triggerLeft},{analog.triggerRight}");

                foreach (int i in Enum.GetValues(typeof(Buttons)))
                {
                    ImGui.TableNextColumn();
                    ImGui.Text($"{Enum.GetName(typeof(Buttons), i)}");
                    ImGui.TableNextColumn();
                    ImGui.Text($"{Input.ButtonPressed(i)}");
                }
            }
            ImGui.EndTable();
        }
        ImGui.End();

        scene?.Draw();
    }
}