import "engine" for Draw, Trap, Asset, Button, Align

class Game1Win {
  nextScene { _nextScene }

  construct new(params) {
    _nextScene = null
    _time = 0
    _scale = 4

    _font = Asset.create(Asset.Font, "speccy", "fonts/spectrum.ttf")
    _sprite = Asset.create(Asset.Image, "goat", "gfx/game1/goat.png")
    // TODO: should this be passed in? are we gonna re-use this game over?
    _spr = Asset.create(Asset.Sprite, "spr", "maps/tilesets/e1.png")
    Asset.spriteSet(_spr, 8, 8, 0, 0)

    Asset.loadAll()
  }

  shutdown() {
  }

  update(dt) {
    _time = _time + dt

    if (Trap.keyPressed(Button.B, 0, -1) || Trap.keyPressed(Button.Start, 0, -1)) {
      _nextScene = "gameselect"
    }
  }

  textColor() {
    var idx = ((_time * 12) % 16).floor
    var l = idx & 1
    var b = (idx & 2) >> 1
    var g = (idx & 4) >> 2
    var r = (idx & 8) >> 3

    var mul = l > 0 ? 255 : 192
    return [r * mul, g * mul, b * mul, 255]
  }

  draw(w, h) {
    Draw.clear()
    Draw.scale(_scale)
    w = w/_scale
    h = h/_scale

    Draw.setTextStyle(_font, 20, 1.0, Align.Left+Align.Top)
    Draw.setColor(textColor())
    var y = h/2 + 30
    Draw.text(30, y, w, "TOP BANANA! YOU WON.")
    Draw.text(60, y+16, w, "everything by clive")
    y = y + 32
    Draw.text(90, y, w, "THANKS MUM FOR THE PIES")

    Draw.setColor(255, 255, 255, 255)
    y = y + 16

    var frame = (_time * 8% 4).floor
    Draw.sprite(_spr, 8 + (frame * 2), w/2 - 8, h/2 - 32, 255, 1, 0, 2, 2)
  }
}