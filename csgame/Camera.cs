using Slate2D;

public struct CameraConstraint
{
    public int x, y, w, h;

    public CameraConstraint(int x, int y, int w, int h)
    {
        this.x = x;
        this.y = y;
        this.w = w;
        this.h = h;
    }
};

public class Camera
{
    // camera coords
    public int x { get; internal set; } = 0;
    public int y { get; internal set; } = 0;

    // size of viewport
    public int w { get; } = 0;
    public int h { get; } = 0;

    // camera constraint
    CameraConstraint? con = null;

    Camera(int w, int h)
    {
        this.w = w;
        this.h = h;
    }

    // don't let the camera render beyond this box
    void Constrain(int x, int y, int w, int h)
    {
        con = new CameraConstraint(x, y, w, h);
        Move(this.x, this.y);
    }

    // move the camera to this position
    void Move(int x, int y)
    {
        this.x = x;
        this.y = y;

        // clamp if out of bounds
        if (con is not null)
        {
            this.x = Math.Clamp(x, con.Value.x, con.Value.x + con.Value.w - w);
            this.y = Math.Clamp(y, con.Value.y, con.Value.y + con.Value.h - h);
        }
    }

    // moves the camera based on a focus point and a window
    // camera won't move if you're within windowWidth px from the center
    void Window(int mx, int my, int windowWidth, int windowHeight)
    {
        var centerX = x + w / 2;
        var centerY = y + h / 2;

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

        Move(x + deltaX, y + deltaY);
    }

    // centers the camera at a point
    void Center(int mx, int my)
    {
        this.Move(mx - w / 2, my - h / 2);
    }

    // set transform to draw from this camera's POV
    // don't move the camera while inside this!
    void DrawStart()
    {
        DC.Translate(-x, -y);
    }

    // move transform back to stop drawing from this camera's POV
    void DrawEnd()
    {
        DC.Translate(x, y);
    }
}