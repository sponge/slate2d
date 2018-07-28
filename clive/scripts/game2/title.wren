import "timer" for Timer
import "engine" for Draw, Asset, Trap, Color, Fill, Button
import "math" for Math
import "soundcontroller" for SoundController

class Game2Title {
   nextScene { _nextScene }
   nextScene=(params) { _nextScene = params }

   construct new(params) {
      _nextScene = null
      _time = 0
      _baseX = null

      _font = Asset.create(Asset.BitmapFont, "cracktrofont", "gfx/game2/cracktrofont.png")
      Asset.bmpfntSet(_font, "abcdefghijklmnopqrstuvwxyz1234567890-+?:,./&!'\"_[]", 32, 0, 32, 32)

      _smallFont = Asset.create(Asset.BitmapFont, "afterburnerfont", "gfx/game2/afterburner.png")
      Asset.bmpfntSet(_smallFont, " !\"#$\%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~", 8, 0, 8, 8)

      _music = Asset.create(Asset.Mod, "unreal", "sound/2ND_PM.S3M")

      _bg = Asset.create(Asset.Image, "crackbg", "gfx/game2/britstralia.png")

      Asset.loadAll()


      _bounce = "clive sturridge's battlement defence 2 for ms-dos. this one was "
      _bounce = _bounce + "real easy to crack, almost no protection at all. cmon clive. this marks our 25th release of 1988! "
      _bounce = _bounce + "greetz go out to fairlight, hardclumping, razor1911, lilpp, the dream team, vinic "
      _bounce = _bounce + "see you in the next release!"


      _strWidth = Asset.measureBmpText(_font, _bounce)
      _lw = Asset.measureBmpText(_font, "a")

      SoundController.playMusic(_music)
   }

   drawCenteredText(font, x, y, str, scale) {
      var width = Asset.measureBmpText(font, str) * scale
      Draw.bmpText(font, x - width/2, y, str, scale)
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

      Draw.image(_bg, 0, 0)

      drawCenteredText(_smallFont, 320, 16, "ALLiGATOR PRESENTS", 2)
      drawCenteredText(_smallFont, 320, 32, "ANOTHER FINE BRITSTRALIA RELEASE", 2)
      drawCenteredText(_smallFont, 320, 64, "BATTLEMENT DEFENCE 2", 2)

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