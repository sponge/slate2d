import "timer" for Timer
import "engine" for Draw, Asset, Trap, Color, Fill, Button, ImageFlags, Align
import "debug" for Debug
import "uibutton" for TextButton
import "soundcontroller" for SoundController

class IntroMessage {
   nextScene { _nextScene }
   nextScene=(params) { _nextScene = params }

   construct new(params) {
      _nextScene = null
      _time = 0

      _bodyFont = Asset.create(Asset.Font, "body", "fonts/Roboto-Regular.ttf")
      _banner = Asset.create(Asset.Sprite, "welcome_banner", "gfx/welcome_banner.png")
      Asset.spriteSet(_banner, 150, 16, 0, 0)
      _click = Asset.create(Asset.Sound, "menu_click", "sound/menu_click.ogg")

      Asset.loadAll()
   }

   update(dt) {
      _time = _time + dt

      if (Trap.keyPressed(Button.B, 0, -1)) {
         SoundController.playOnce(_click)
         nextScene = "gameselect"
      }

   }

   draw(w, h) {
      Draw.clear()
      Draw.resetTransform()

      Draw.setColor(Color.Fill, 0, 57, 113, 255)
      Draw.rect(0, 0, w, h, false)

      Draw.transform(h / 720, 0, 0, h / 720, 0, 0)

      Draw.setColor(Color.Fill, 0, 0, 0, 200)
      Draw.rect(108, 0, h/720*w-200, 720, Fill.Solid)

      Draw.setColor(Color.Fill, 255, 255, 255, 255)

      Draw.sprite(_banner, 0, w/2 - 225, 10, 1, 3)

      Draw.setTextStyle(_bodyFont, 24, 1.0, Align.Left+Align.Top)
      Draw.text(128, 128, h/720*w-256, "Thanks for purchasing Clive Sturridge's Battlement Defense Legacy Collection. What you are about to play is a museum-grade collection of games made by the iconic British programmer and designer, Clive Sturridge. His impact on the industry as a whole cannot be understated, making him worthy of his auteurship. For the first time ever, all his works are playable in one collection.

Controls:
Left mouse: Place Selected Piece
Right mouse: Rotate Wall Pieces
"
      )
      Draw.submit()
   }

   shutdown() {

   }
}