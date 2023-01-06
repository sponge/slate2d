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

    Game.Update(dt);

    Game.Draw();

    DC.Submit();

    SLT.EndFrame();
    SLT.UpdateLastFrameTime();
}

SLT.Shutdown();
