// NOTE: hot reload doesn't work in this file
using Slate2D;

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

    Main.Update(dt);

    Main.Draw();

    SLT.EndFrame();
    SLT.UpdateLastFrameTime();
}

SLT.Shutdown();