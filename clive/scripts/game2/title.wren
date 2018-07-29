import "engine" for Draw, Trap, Button, Asset, Color
import "soundcontroller" for SoundController

class Game2Title {
   nextScene { _nextScene }

   construct new(param) {
      _logo = Asset.create(Asset.Image, "reasonable_logo", "gfx/game2/logo.png")
      _logoFade1 = Asset.create(Asset.Image, "reasonable_logo_fade1", "gfx/game2/logo_fade1.png")
      _logoFade2 = Asset.create(Asset.Image, "reasonable_logo_fade2", "gfx/game2/logo_fade2.png")
      _music = Asset.create(Asset.Sound, "reasonable_logo_music", "sound/reasonable_logo.ogg")
      _titleBg = Asset.create(Asset.Image, "game2_title", "gfx/game2/title.png")

      _time = 0

      _reaY = -50 
      _sonY = -60
      _aY = -60
      _bleY = -60
      _softwareWidth = 0

      _baseline = 60
      _drawLogo = true
      _drawTitle = false

      Asset.loadAll()

      SoundController.playOnce(_music)
   }

   update(dt) {
      _time = _time + dt

      if (Trap.keyPressed(Button.B, 0, -1)) {
         _nextScene = ["td", "maps/e2m1.tmx"]
      }

      var speed = 12
      _reaY = _reaY < _baseline ? _reaY + speed : _baseline

      if (_time > 0.35) {
         _sonY = _sonY < _baseline ? _sonY + speed : _baseline
      }

      if (_time > 0.70) {
         _aY = _aY < _baseline ? _aY + speed : _baseline
      }

      if (_time > 1.05) {
         _bleY = _bleY < _baseline ? _bleY + speed : _baseline
      }

      if (_time > 1.25) {
         _softwareWidth = _softwareWidth < 345 ? _softwareWidth + 3.5 : 345
      }

      if (_time > 5) {
         _drawTitle = true
      } else if (_time > 4.4) {
         _drawLogo = false
      } else if (_time > 4.2) {
         _logo = _logoFade2
      } else if (_time > 4.0) {
         _logo = _logoFade1
      }
   }

   drawRea(x, y) {
      Draw.image(_logo, x, y, 113, 60)
   }

   drawSon(x, y) {
      Draw.image(_logo, x, y, 104, 60, 1, 1, 0, 113, 0)
   }

   drawA(x, y) {
      Draw.image(_logo, x, y, 33, 60, 1, 1, 0, 220, 0)
   }

   drawBle(x, y) {
      Draw.image(_logo, x, y, 91, 60, 1, 1, 0, 254, 0)
   }

   drawSoftware(x, y) {
      if (_softwareWidth > 0) {
         Draw.image(_logo, x, y, _softwareWidth, 24, 1, 1, 0, 2, 65)
      }
   }

   draw(w, h) {
      if (_drawTitle) {
         Draw.transform(h / 180, 0, 0, h / 180, 0, 0)
         Draw.image(_titleBg, 0, 0)
         Draw.submit()
         return
      }

      Draw.transform(h / 240, 0, 0, h / 240, 0, 0)

      var x = 41 
      var y = 0

      if (_drawLogo) {
         drawSoftware(x, _baseline + 66)

         drawRea(x, _reaY)
         drawSon(x + 113, _sonY)
         drawA(x + 113 + 104, _aY)
         drawBle(x + 113 + 104 + 33, _bleY)
      }

      Draw.submit()
   }

   shutdown() {
   }
}