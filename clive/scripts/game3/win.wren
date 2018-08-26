import "engine" for Draw, Trap, Asset, Button, Align

class Game3Win {
   nextScene { _nextScene }

   construct new(params) {
      Asset.clearAll()

      _nextScene = null
      _time = 0
      _extraGoats = params["extraGoats"] + 2

      _font = Asset.create(Asset.Font, "speccy", "fonts/spectrum.ttf")
      _spr = Asset.create(Asset.Sprite, "endingspr", "maps/tilesets/e3.png")
      Asset.spriteSet(_spr, 8, 8, 0, 0)

      Asset.loadAll()
   }

   shutdown() {
   }

   update(dt) {
      _time = _time + dt

      if (_time > 3) {
         _nextScene = ["td", {"map":"maps/e3m1.tmx", "extraGoats": _extraGoats}]
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
      Draw.clear()
      Draw.transform(h/180, 0, 0, h/180, 0, 0)

      Draw.setTextStyle(_font, 20, 1.0, Align.Center|Align.Top)
      Draw.setColor(textColor())
      Draw.text(0, 50, 320, "THE GOATS HAVE BEEN BESTED")
      Draw.text(0, 60, 320, "TRY AGAIN WITH %(_extraGoats) EXTRA GOATS!")

      var frame = (_time * 8% 4).floor
      Draw.sprite(_spr, 8 + (frame * 2), 128, 90, 1.0, 4, 0, 2, 2)

      Draw.submit()
   }
}