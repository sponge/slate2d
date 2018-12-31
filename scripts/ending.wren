import "engine" for Asset, Draw, Trap, Button
import "soundcontroller" for SoundController

class Ending {
   nextScene { _nextScene }
   nextScene=(params) { _nextScene = params }

   construct new(params) {
      _bigFont = Asset.create(Asset.BitmapFont, "sneakattack", "gfx/sneak-attack-bitmap.png")
      Asset.bmpfntSet(_bigFont, "abcdefghijklmnopqrstuvwxyz'!?'", 0, 1, 2, 5)

      _table = Asset.create(Asset.Sprite, "table", "gfx/table.png")
      Asset.spriteSet(_table, 72, 48, 0, 0)

      _win = Asset.create(Asset.Image, "win", "gfx/youwin.png")

      _bgm = Asset.create(Asset.Sound, "bgm", "sound/menu.ogg")

      _t = 0

      Asset.loadAll()

      SoundController.playMusic(_bgm)
   }

   update(dt) {
      _t = _t + dt
      if (Trap.keyPressed(Button.Start, 0, -1)) {
         _nextScene = ["title"]
      }
   }

   drawCenteredText(font, x, y, text) {
      var w = Asset.measureBmpText(font, text)
      Draw.bmpText(font, x - w/2, y, text)
   }

   draw(w, h) {
      Draw.clear()
      Draw.resetTransform()
      Draw.scale(h / 180)

      var winSize = Asset.imageSize(_win)

      for (i in 0...winSize[0]) {
         var y = ((i - _t * 1.2) / 6).sin * 1.3
         Draw.setColor(247, 226, 107, 255)
         Draw.image(_win, 320/2 - winSize[0] + i * 2, y + 8, 1, 0, 1.0, 2.0, 0, i, 0)
         Draw.setColor(235, 137, 49, 255)
         Draw.image(_win, 320/2 - winSize[0] + i * 2, y + 24, 1, 0, 1.0, 2.0, 0, i, 0)
         Draw.setColor(190, 38, 51, 255)
         Draw.image(_win, 320/2 - winSize[0] + i * 2, y + 40, 1, 0, 1.0, 2.0, 0, i, 0)
      }

      Draw.setColor(255, 255, 255, 255)

      drawCenteredText(_bigFont, 320/2, 64, "you survived dinner!")
      Draw.sprite(_table, 2, 320/2 - 72, 76, 1.0, 2.0)
   }

   shutdown() {
      Asset.clearAll()
   }
}