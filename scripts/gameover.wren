import "engine" for Asset, Draw, Trap, Button
import "random" for Random

class GameOver {
   nextScene { _nextScene }
   nextScene=(params) { _nextScene = params }

   construct new(level) {
      _font = Asset.create(Asset.BitmapFont, "font", "gfx/font.png")
      Asset.bmpfntSet(_font, "abcdefghijklmnopqrstuvwxyz!?'", 0, 1, 2, 5)

      _bigFont = Asset.create(Asset.BitmapFont, "sneakattack", "gfx/sneak-attack-bitmap.png")
      Asset.bmpfntSet(_bigFont, "abcdefghijklmnopqrstuvwxyz'", 0, 1, 2, 5)

      _spr = Asset.create(Asset.Sprite, "gameover", "gfx/%(level["sprite"]).png")
      Asset.spriteSet(_spr, 16, 48, 0, 0)

      _icons = Asset.create(Asset.Sprite, "icons", "gfx/icons.png")
      _iconCount = 15
      Asset.spriteSet(_icons, 16, 16, 0, 0)

      _bubble = Asset.create(Asset.Image, "bubble", "gfx/bubble.png")

      _level = level

      Asset.loadAll()

      _t = 0
      _rnd = Random.new()
      _startIcon = _rnd.int(_iconCount)
      _bubbleCount = 4
   }

   update(dt) {
      _t = _t + dt

      if (Trap.keyPressed(Button.Start)) {
         _nextScene = "title"
      }

      if (_t % 64 == 0) {
         _startIcon = (_startIcon + _rnd.int(2, _iconCount)) % _iconCount
      }
   }

   drawCenteredText(font, x, y, text, flash, scale) {
      var textWidth = Asset.measureBmpText(font, text, scale)

      if (flash && _t % 24 < 12) {
         Draw.setColor(235, 137, 49, 255)
      } else {
         Draw.setColor(247, 226, 107, 255)
      }
      Draw.bmpText(font, x - textWidth/2, y, text, scale)
      Draw.setColor(255, 255, 255, 255)
   }

   draw(w, h) {
      Draw.clear()
      Draw.resetTransform()
      Draw.scale(h / 180)

      Draw.sprite(_spr, 1, 320/2 - 8, 52)
      var i = 0
      var step = Num.pi / (_bubbleCount - 1)
      for (i in 0..._bubbleCount) {
         var x = 32 * i + 100 
         var y = 40 - ((i * step).sin * 24)
         var flip = x < 160 ? 1 : 0
         Draw.image(_bubble, x, y, 0, 0, 1.0, 1.0, flip)
         Draw.sprite(_icons, (_startIcon + i) % _iconCount, flip == 1 ? x + 3 : x + 6, y + 3)
         i = i + 1
      }

      var y = 112
      var name = _level["spriteName"]
      drawCenteredText(_font, 320/2, y, "oh no! %(name) is yelling!", false, 1.0)
      drawCenteredText(_font, 320/2, y + 12, "better luck next year!", false, 1.0)
      drawCenteredText(_bigFont, 320/2, y + 34, "game over", true, 2.0)
   }

   shutdown() {
      Asset.clearAll()
   }  
}