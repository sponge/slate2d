using Slate2D;
using System;
using System.Numerics;

public enum Dir
{
	None,
	Up,
	Right,
	Down,
	Left
}

public enum Buttons
{
	Up,
	Down,
	Left,
	Right,
	Jump,
	Shoot
}

public enum CollisionType
{
	Disabled,
	Enabled,
	Platform,
	Trigger,
}

class Phys
{
	// physics values
	// most values from https://cdn.discordapp.com/attachments/191015116655951872/332350193540268033/smw_physics.png
	public const byte PMeterCapacity = 112;
	public const float Friction = 0.0625f;
	public const float Accel = 0.09375f;
	public const float SkidAccel = 0.3125f;
	public const float SlideAccel = 0.09375f * 3f;
	public const float RunSpeed = 2.25f;
	public const float MaxSpeed = 3f;
	public const float HeldGravity = 0.1875f;
	public const float Gravity = 0.375f;
	public const float EnemyGravity = 0.375f / 3f;
	public const byte EarlyBounceFrames = 8;
	public const byte EarlyJumpFrames = 6;
	public const byte LateJumpFrames = 6;
	public const float TerminalVelocity = 4f;
	public const float EnemyJumpHeld = 6.5f;
	public const float EnemyJump = 5.5f;
	public const float SpringJumpHeld = 6.5f;
	public const float SpringJump = 5.5f;
	public readonly (float Speed, float Height)[] JumpHeights = {
		(3f, 5.75f),
		(2.5f, 5.5625f),
		(2f, 5.4375f),
		(1.5f, 5.25f),
		(1f, 5.125f),
		(0.5f, 4.9375f),
		(0f, 4.8125f)
	};
}

enum Tile
{
	Empty,
	Solid,
	SlopeL,
	SlopeR,
	SlopeUL,
	SlopeUR,
	Platform,
	Bridge,
	Dirtback
}


public static class Util
{
	public static Dir GetOppositeDir(Dir dir)
	{
		return dir switch
        {
            Dir.Up => Dir.Down,
            Dir.Right => Dir.Left,
            Dir.Down => Dir.Up,
            Dir.Left => Dir.Right,
            _ => Dir.None
        };
	}

	public static bool PointInRect<T>(T px, T py, T x, T y, T w, T h) where T : INumber<T>
	{
		return (px >= x) && (px < (x + w)) && (py >= y) && (py < (y + h));
	}

	public static bool RectIntersect<T>((T, T) apos, (T,T) asize, (T,T) bpos, (T,T) bsize) where T : INumber<T>
	{
		return apos.Item1 < bpos.Item1 + bsize.Item1 && apos.Item1 + asize.Item1 > bpos.Item1 &&
			apos.Item2 < bpos.Item2 + bsize.Item2 && apos.Item2 + asize.Item2 > bpos.Item2;
	}

	public static bool EntIntersect(Entity a, Entity b)
	{
		return RectIntersect(a.Pos, a.Size, b.Pos, b.Size);
	}

	public static float Lerp(float a, float b, float t)
	{
		return (1 - t) * a + t * b;
	}

	public static T InvLerp<T>(T a, T b, T v) where T : INumber<T>
	{
		return (v - a) / (b - a);
	}

	public static void RegisterButtons()
	{
		string[] buttons = { "up", "down", "left", "right", "jump", "shoot" };
        Input.AllocateButtons(buttons);
	}
}