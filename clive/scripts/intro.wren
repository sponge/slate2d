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
      _click = Asset.create(Asset.Image, "logo_click", "gfx/title_click.png")
      _music = Asset.create(Asset.Sound, "menu_bgm", "sound/menu_bgm.ogg")

      Asset.loadAll()

      SoundController.stopMusic()
      SoundController.playMusic(_music)
   }

   update(dt) {
      _time = _time + dt

      if (Trap.keyPressed(Button.B, 0, -1)) {
         nextScene = "intromessage"
      }
   }

   draw(w, h) {
      Draw.clear()
      Draw.resetTransform()

      Draw.setColor(Color.Fill, 0, 57, 113, 255)
      Draw.rect(0, 0, w, h, false)

      Draw.transform(h / 720, 0, 0, h / 720, 0, 0)

      Draw.image(_logo, 216, 55)
      if (_time.floor % 2 == 0) {
         Draw.image(_click, 494, 600)
      }

      Draw.setColor(Color.Stroke, 255, 0, 0, 255)

      Draw.submit()
   }

   shutdown() {

   }
}