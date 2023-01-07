using Slate2D;

public static class Game
{
	public static void Update(double dt)
	{

	}

	public static void Draw()
	{
		DC.Clear(30, 30, 30, 255);
		DC.DrawRect(200, 200, 20, 20, false);
        DC.DrawRect(300, 300, 20, 20, true);

    }
}