using Slate2D;

SLT.Init(args);

while (true)
{
    var dt = SLT.StartFrame();
    if (dt < 0)
    {
        SLT.EndFrame();
        break;
    }

    DC.Clear(60, 30, 30, 255);

    DC.Submit();

    SLT.EndFrame();
    SLT.UpdateLastFrameTime();
}

SLT.Shutdown();
