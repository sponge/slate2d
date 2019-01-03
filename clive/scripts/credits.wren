import "engine" for Draw, Asset, Trap, Fill, Button, Align
import "fonts" for Fonts
import "soundcontroller" for SoundController

class Credits {
   nextScene { _nextScene }
   nextScene=(params) { _nextScene = params }

   construct new(params) {
      _nextScene = null
      _time = 0

      _bodyFont = Asset.create(Asset.Font, "body", Fonts.body)
      _gradient = Asset.create(Asset.Image, "menu_gradient", "gfx/menu_gradient.png")
      _clickSound = Asset.create(Asset.Sound, "menu_click", "sound/menu_click.ogg")

      Asset.loadAll()
   }

   update(dt) {
      _time = _time + dt

      if (Trap.keyPressed(Button.B, 0, -1)) {
         SoundController.playOnce(_clickSound)
         nextScene = "intromessage"
      }
   }

   drawCredit(col, y, name, link, creds) {
      var margin = 128

      Draw.setColor(200, 227, 255, 255)
      Draw.setTextStyle(_bodyFont, 64, 1.0, Align.Center+Align.Top)
      Draw.text(margin + col * (640 - margin), y, 640 - margin, name)
      y = y + 52

      Draw.setColor(255, 255, 255, 164)
      Draw.setTextStyle(_bodyFont, 40, 1.0, Align.Center+Align.Top)
      Draw.text(margin + col * (640 - margin), y, 640 - margin, link)
      y = y + 36

      Draw.setColor(255, 255, 255, 255)
      Draw.setTextStyle(_bodyFont, 40, 1.0, Align.Center+Align.Top)
      Draw.text(margin + col * (640 - margin), y, 640 - margin, creds)

      Draw.setColor(255, 255, 255, 255)
            
      return y + 96
   }

   draw(w, h) {
      Draw.clear()
      Draw.resetTransform()

      Draw.setColor(0, 57, 113, 255)
      Draw.rect(0, 0, w, h, false)
      Draw.setColor(255, 255, 255, 255)
      Draw.image(_gradient, 0, 0, w, h)

      Draw.scale(h / 720)

      Draw.setColor(200, 227, 255, 255)
      Draw.setTextStyle(_bodyFont, 96, 1.0, Align.Center+Align.Top)
      Draw.text(0, 30, 1280, "CREDITS")
      Draw.setColor(255, 255, 255, 255)

      var textY = 200
      textY = drawCredit(
         0,
         textY,
         "alligator",
         "alligator.itch.io",
         "art, concept, programming, writing, voice acting"
      )
      textY = drawCredit(
         0,
         textY,
         "HardClumping",
         "@hardclumping",
         "audio, music, writing"
      )
      textY = 200
      textY = drawCredit(
         1,
         textY,
         "sponge",
         "@mikerubits",
         "concept, engine, programming, writing"
      )
      textY = drawCredit(
         1,
         textY,
         "LiLPP",
         "@tinypeepee",
         "audio"
      )
   }

   shutdown() {

   }
}