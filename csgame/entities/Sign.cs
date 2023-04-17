namespace Sign;
using Slate2D;

enum States {
  Default,
  Idle,
  Expand,
  Read,
  Shrink,
}

[Spawnable]
class Sign : FSMEntity<States> {
  (int X, int Y) SignPos = (0, 130);
  (int W, int H) SignSize = (0, 0);
  uint SignTime = 20;
  string SignText = "";

  public Sign(LDTKEntity ent) : base(ent) {
    Collidable = CollisionType.Trigger;
    Sprite = Assets.Find("sign");
    RunWhilePaused = true;
    SignText = ent.Properties.GetValueOrDefault("Text", null)?.Str ?? "No Text key found";
    FSMTransitionTo(States.Idle);
  }

  public override void Draw() {
    base.Draw();
    DC.SetColor(255, 255, 255, 255);
    DC.Sprite(Sprite, Frame, Pos.X + DrawOfs.X, Pos.Y + DrawOfs.Y, 1, FlipBits, 1, 1);
  }

  void Idle_Enter() {
    Layer = Layer.Background;
    Main.World.GameState.Paused = false;
  }
  void Idle_Collide(Entity other, Dir dir) {
    if (other is Player == false) return;
    if (!Input.ButtonPressed((int)Buttons.Up)) return;
    FSMTransitionTo(States.Expand);
  }

  void Expand_Enter() {
    FSMTimer(States.Read, SignTime);
    var size = Assets.TextSize(0, SignText, 0);
    SignSize = (size.W + 20, size.H + 20);
    SignPos.X = Main.World.Res.W / 2 - SignSize.W / 2;
    Main.World.GameState.Paused = true;
    Layer = Layer.Foreground;
  }
  void Expand_Draw() {
    float t = Math.Clamp((Ticks - StartStateTime) / (float)SignTime, 0, 1);
    var w = Util.Lerp(0, SignSize.W, t);
    var h = Util.Lerp(0, SignSize.H, t);
    var x = SignPos.X + (SignSize.W - w) / 2;
    var y = SignPos.Y + (SignSize.H - h) / 2;
    DC.SetColor(0, 0, 0, 255);
    DC.Rect(x, y, w, h, false);
  }

  void Read_Update(uint ticks, float dt) {
    if (Input.ButtonPressed((int)Buttons.Jump)) FSMTransitionTo(States.Shrink);
  }
  void Read_Draw() {
    DC.SetColor(0, 0, 0, 255);
    DC.Rect(SignPos.X, SignPos.Y, SignSize.W, SignSize.H, false);
    DC.SetTextStyle(Main.World.BlueFont, 1, 1, 2);
    DC.SetColor(255, 255, 255, 255);
    DC.Text(SignPos.X + 10, SignPos.Y + 10, SignSize.W - 20, SignText, 0);
  }

  void Shrink_Enter() => FSMTimer(States.Idle, SignTime);
  void Shrink_Draw() {
    float t = Math.Clamp((Ticks - StartStateTime) / (float)SignTime, 0, 1);
    var w = Util.Lerp(SignSize.W, 0, t);
    var h = Util.Lerp(SignSize.H, 0, t);
    var x = SignPos.X + (SignSize.W - w) / 2;
    var y = SignPos.Y + (SignSize.H - h) / 2;
    DC.SetColor(0, 0, 0, 255);
    DC.Rect(x, y, w, h, false);
  }
}
