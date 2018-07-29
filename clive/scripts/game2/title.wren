import "engine" for Draw, Trap, Button, Asset, Color
import "soundcontroller" for SoundController

class Game2Title {
   nextScene { _nextScene }

   construct new(param) {
      _logo = Asset.create(Asset.Image, "reasonable_logo", "gfx/game2/logo.png")
      _music = Asset.create(Asset.Sound, "reasonable_logo_music", "sound/reasonable_logo.ogg")

      _time = 0

      _reaY = -50 
      _sonY = -60
      _aY = -60
      _bleY = -60
      _softwareWidth = 0

      _baseline = 100

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

      if (_time > 0.65) {
         _aY = _aY < _baseline ? _aY + speed : _baseline
      }

      if (_time > 1.05) {
         _bleY = _bleY < _baseline ? _bleY + speed : _baseline
      }

      if (_time > 1.25) {
         _softwareWidth = _softwareWidth < 345 ? _softwareWidth + 3.5 : 345
      }
   }

   drawRea(x, y) {
      Draw.image(_logo, x, y, 113, 54)
   }

   drawSon(x, y) {
      Draw.image(_logo, x, y, 105, 54, 1, 1, 0, 113, 0)
   }

   drawA(x, y) {
      Draw.image(_logo, x, y, 33, 54, 1, 1, 0, 220, 0)
   }

   drawBle(x, y) {
      Draw.image(_logo, x, y, 91, 54, 1, 1, 0, 254, 0)
   }

   drawSoftware(x, y) {
      if (_softwareWidth > 0) {
         Draw.image(_logo, x, y, _softwareWidth, 24, 1, 1, 0, 2, 65)
      }
   }

   draw(w, h) {
      Draw.transform(h / 360, 0, 0, h / 360, 0, 0)

      var x = 148 
      var y = 0

      Draw.setColor(Color.Fill, 170, 170, 170, 255)
      Draw.rect(x, _baseline + 60, 345, 2, false)

      drawSoftware(x, _baseline + 70)

      drawRea(x, _reaY)
      drawSon(x + 114, _sonY)
      drawA(x + 114 + 105, _aY)
      drawBle(x + 114 + 105 + 33, _bleY)

      Draw.submit()
   }

   shutdown() {
   }
}