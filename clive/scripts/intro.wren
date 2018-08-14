import "timer" for Timer
import "engine" for Draw, Asset, Trap, Color, Fill, Button
import "debug" for Debug
import "soundcontroller" for SoundController

class Intro {
   nextScene { _nextScene }
   nextScene=(params) { _nextScene = params }

   construct new(params) {
      _nextScene = null
      _time = 0

      _logo = Asset.create(Asset.Image, "logo", "gfx/title_logo.png")
      _clickToContinue = Asset.create(Asset.Image, "logo_click", "gfx/title_click.png")
      _music = Asset.create(Asset.Sound, "menu_bgm", "sound/menu_bgm.ogg")
      _clickSound = Asset.create(Asset.Sound, "menu_click", "sound/menu_click.ogg")
      _gradient = Asset.create(Asset.Image, "menu_gradient", "gfx/menu_gradient.png")

      Asset.loadAll()

      SoundController.stopMusic()
      SoundController.playMusic(_music)
   }

   update(dt) {
      _time = _time + dt

      if (Trap.keyPressed(Button.B, 0, -1)) {
         SoundController.playOnce(_clickSound)
         nextScene = "credits"
      }
   }

   draw(w, h) {
      Draw.clear()
      Draw.resetTransform()

      Draw.setColor(Color.Fill, 0, 57, 113, 255)
      Draw.rect(0, 0, w, h, false)
      Draw.image(_gradient, 0, 0, w, h)

      Draw.transform(h / 720, 0, 0, h / 720, 0, 0)

      Draw.image(_logo, 0, 0)
      if (_time.floor % 2 == 0) {
         Draw.image(_clickToContinue, 494, 620)
      }

      Draw.setColor(Color.Stroke, 255, 0, 0, 255)

      Draw.submit()
   }

   shutdown() {

   }
}