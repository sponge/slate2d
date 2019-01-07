import "engine" for Asset, Draw, Trap, Button, Align

class Help {
  nextScene { _nextScene}
  nextScene=(params) { _nextScene = params }

  construct new(param) {
    _font = Asset.create(Asset.BitmapFont, "font", "gfx/font.png")
    Asset.bmpfntSet(_font, "abcdefghijklmnopqrstuvwxyz!?'$1234567890,", 0, 1, 2, 8)
    _fontSize = 1.0

    // _font = Asset.create(Asset.Font, "font", "fonts/Roboto-Regular.ttf")
    // _fontSize = 12

    _arrows = Asset.create(Asset.Image, "arrows", "gfx/arrows.png")
    _zx = Asset.create(Asset.Image, "zx", "gfx/zx.png")

    _dpad = Asset.create(Asset.Image, "dpad", "gfx/arrows-gamepad.png")
    _padButtons = Asset.create(Asset.Image, "dpadbuttons", "gfx/zx-gamepad.png")

    Asset.loadAll()

    _credits = [
      ["engine, programming", "sponge"],
      ["programming, art", "alligator"],
      ["music", "hardclumping"],

    ]
    _story = "oh no it's christmas day at aunt deborah and
uncle tony's house!

avoid the minefield of controversial
conversation topics and try not to get anyone
into a full on rant!

collect blue conversation bubbles to make
enough smalltalk, until you can escape by
flying out of the room.

can you make it past christmas dinner?"
    _t = 0
    _canReturn = false
  }

  update(dt) {
    _t = _t + dt

    if (!Trap.keyPressed(Button.Start)) {
      _canReturn = true
    }

    if (Trap.keyPressed(Button.Start) && _canReturn) {
      _nextScene = "title"
    }
  }

  drawBox(title, x, y, w, h) {
    var measure = Asset.textWidth(_font, title, _fontSize)
    Draw.setColor(49, 162, 242, 255)
    Draw.setTextStyle(_font, _fontSize, 1.0, Align.Left+Align.Top)
    Draw.text(x + 4, y, 0, title)
    Draw.setColor(255, 255, 255, 255)

    Draw.rect(x, y + 3, 2, 1, false)
    Draw.rect(x, y + 3, 1, h - 3, false)
    Draw.rect(x, y + h, w + 1, 1, false)
    Draw.rect(x + w, y + 3, 1, h - 3, false)
    Draw.rect(x + measure + 5, y + 3, w - measure - 5, 1, false)
  }

  drawControls(x, y) {
    drawBox("controls", x, y, 92, 60)
    x = x + 6
    y = y + 10
    Draw.image(_zx, x, y)
    Draw.image(_padButtons, x, y + 20)
    Draw.setTextStyle(_font, _fontSize, 1.0, Align.Left+Align.Top)
    Draw.text(x + 8, y + 40, 0, "flap")

    Draw.image(_arrows, x + 48, y)
    Draw.image(_dpad, x + 48, y + 20)
    Draw.setTextStyle(_font, _fontSize, 1.0, Align.Left+Align.Top)
    Draw.text(x + 55, y + 40, 0, "move")

  }

  drawCredits(x, y) {
    drawBox("credits", x, y, 92, 86)

    var currentY = y + 8
    for (credit in _credits) {
      Draw.setColor(247, 226, 107, 255)
      Draw.setTextStyle(_font, _fontSize, 1.0, Align.Left+Align.Top)
      Draw.text(x + 3, currentY, 0, credit[1])
      Draw.setColor(255, 255, 255, 255)
      Draw.setTextStyle(_font, _fontSize, 1.0, Align.Left+Align.Top)
      Draw.text(x + 3, currentY + 8, 0, credit[0])
      currentY = currentY + 23
    }
  }

  drawStory(x, y) {
    drawBox("story", x, y, 200, 154)

    Draw.setTextStyle(_font, _fontSize, 1.0, Align.Left+Align.Top)
    Draw.text(x + 4, y + 8, 0, _story)
  }

  draw(w, h) {
    Draw.clear()
    Draw.resetTransform()
    Draw.scale(h / 180)

    drawStory(10, 6)
    drawControls(220, 6)
    drawCredits(220, 74)

    var tw = Asset.textWidth(_font, "press enter to return", _fontSize)
    if (_t % 64 < 32) {
      Draw.setTextStyle(_font, _fontSize, 1.0, Align.Left+Align.Top)
      Draw.text(320/2 - tw/2, 166, 0, "press enter to return")
    }
  }

  shutdown() {
    Asset.clearAll()
  }
}