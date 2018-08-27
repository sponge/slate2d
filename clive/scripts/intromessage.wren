import "timer" for Timer
import "engine" for Draw, Asset, Trap, Fill, Button, ImageFlags, Align
import "debug" for Debug
import "uibutton" for TextButton
import "soundcontroller" for SoundController
import "fonts" for Fonts

class IntroMessage {
   nextScene { _nextScene }
   nextScene=(params) { _nextScene = params }

   construct new(params) {
      _nextScene = null
      _time = 0

      _bodyFont = Asset.create(Asset.Font, "description", Fonts.description)
      _banner = Asset.create(Asset.Sprite, "welcome_banner", "gfx/welcome_banner.png")
      _help = Asset.create(Asset.Image, "help", "gfx/help.png")
      _gradient = Asset.create(Asset.Image, "menu_gradient", "gfx/menu_gradient.png")

      Asset.spriteSet(_banner, 150, 16, 0, 0)
      _click = Asset.create(Asset.Sound, "menu_click", "sound/menu_click.ogg")

      Asset.loadAll()
   }

   update(dt) {
      _time = _time + dt

      if (Trap.keyPressed(Button.B, 0, -1)) {
         SoundController.playOnce(_click)
         nextScene = ["gameselect", "tutorial"]
      }

   }

   draw(w, h) {
      Draw.clear()
      Draw.resetTransform()

      Draw.setColor(0, 57, 113, 255)
      Draw.rect(0, 0, w, h, false)
      Draw.image(_gradient, 0, 0, w, h)

      Draw.scale(h / 720)

//       Draw.setColor(0, 0, 0, 200)
//       Draw.rect(108, 0, h/720*w-200, 720, Fill.Solid)

      Draw.setColor(255, 255, 255, 255)

      Draw.sprite(_banner, 0, w/2 - 225, 10, 1, 3)

      Draw.setTextStyle(_bodyFont, 32, 1.0, Align.Left+Align.Top)
      Draw.text(128, 80, h/720*w-256, "Thanks for purchasing Clive Sturridge's Battlement Defense Legacy Collection!\n\nWhat you are about to play is a museum-grade collection of classic tower defense games, created by the iconic British programmer and designer, Sir Clive Sturridge. Sir Clive's incredible forethought and unceasing commitment to producing quality games has made him a living legend and household name across the world.\n\nWe present, for the first time ever, all of his work in a single collection.
")

      Draw.image(_help, 240, 320)
   }

   shutdown() {

   }
}