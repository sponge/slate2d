using Slate2D;

public struct CameraConstraint
{
    public int X, Y, W, H;

    public CameraConstraint(int x, int y, int w, int h)
    {
        X = x;
        Y = y;
        W = w;
        H = h;
    }
};

public class Camera
{
    // camera coords
    public int X { get; internal set; } = 0;
    public int Y { get; internal set; } = 0;

    // size of viewport
    public int W { get; } = 0;
    public int H { get; } = 0;

    // camera constraint
    CameraConstraint? con = null;

    public Camera(int w, int h)
    {
        W = w;
        H = h;
    }

    // don't let the camera render beyond this box
    public void Constrain(int x, int y, int w, int h)
    {
        con = new CameraConstraint(x, y, w, h);
        Move(X, Y);
    }

    // move the camera to this position
    public void Move(int x, int y)
    {
        X = x;
        Y = y;

        // clamp if out of bounds
        if (con is not null)
        {
            var x1 = con.Value.X;
            var x2 = con.Value.X + con.Value.W - W;
            X = x1 > x2 ? Math.Clamp(x, x2, x1) : Math.Clamp(x, x1, x2);
            var y1 = con.Value.Y;
            var y2 = con.Value.Y + con.Value.H - H;
            Y = y1 > y2 ? Math.Clamp(y, y2, y1) : Math.Clamp(y, y1, y2);
        }
    }

    // moves the camera based on a focus point and a window
    // camera won't move if you're within windowWidth px from the center
    public void Window(int mx, int my, int windowWidth, int windowHeight)
    {
        var centerX = X + W / 2;
        var centerY = Y + H / 2;

        var deltaX = 0;
        if (Math.Abs(mx - centerX) > windowWidth)
        {
            deltaX = mx - centerX + (mx > centerX ? -1 : 1) * windowWidth;
        }

        var deltaY = 0;
        if (Math.Abs(my - centerY) > windowHeight)
        {
            deltaY = my - centerY + (my > centerY ? -1 : 1) * windowHeight;
        }

        Move(X + deltaX, Y + deltaY);
    }

    // centers the camera at a point
    public void Center(int mx, int my)
    {
        Move(mx - W / 2, my - H / 2);
    }

    // set transform to draw from this camera's POV
    // don't move the camera while inside this!
    public void DrawStart()
    {
        DC.Translate(-X, -Y);
    }

    // move transform back to stop drawing from this camera's POV
    public void DrawEnd()
    {
        DC.Translate(X, Y);
    }
}