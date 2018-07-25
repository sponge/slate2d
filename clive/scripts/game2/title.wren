import "timer" for Timer
import "engine" for Draw, Asset, Trap, Color, Fill, Button
import "math" for Math


class Game2Title {
   nextScene { _nextScene }
   nextScene=(params) { _nextScene = params }

   construct new(params) {
      _nextScene = null
      _time = 0
      _baseX = null

      _font = Asset.create(Asset.BitmapFont, "cracktrofont", "gfx/game2/cracktrofont.png")
      Asset.bmpfntSet(_font, "abcdefghijklmnopqrstuvwxyz1234567890-+?:,./&!'\"_[]", 32, 0, 32, 32)

      Asset.loadAll()

      _bounce = "another fine alligator release: clive sturridge's battlement defence 2 for ms-dos. this one was "
      _bounce = _bounce + "real easy to crack, almost no protection at all. cmon clive. this marks our 25th release of 1988! "
      _bounce = _bounce + "greetz go out to hardclumping"

      _strWidth = Asset.measureBmpText(_font, _bounce)
      _lw = Asset.measureBmpText(_font, "a")
   }

   update(dt) {
      _time = _time + dt

      if (_time > 3 && Trap.keyPressed(Button.B, 0, -1)) {
         _nextScene = ["td", "maps/e2m1.tmx"]
      }
   }

   draw(w, h) {
      Draw.clear()
      Draw.resetTransform()
      Draw.transform(h / 360, 0, 0, h / 360, 0, 0)

      if (_baseX == null || _baseX < -_strWidth) {
         _baseX = w / (h/360)
      }

      var x = _baseX
      var y = 300
      var i = 0
      for (letter in _bounce) {
         if (x + _lw >= 0) {
            var x2 = x
            var y2 = y + (_time*10 + i).sin * 16
            Draw.bmpText(_font, x2, y2, letter)
         }
         x = x + _lw
         i = i + 1
      }

      _baseX = _baseX - 3

      Draw.submit()
   }

   shutdown() {

   }
}