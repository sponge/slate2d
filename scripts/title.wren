import "engine" for Draw, Asset, TileMap, Trap, Button, Fill
import "random" for Random
import "math" for Math

class Title {
   nextScene { _nextScene }
   nextScene=(params) { _nextScene = params }

   construct new(params) {
      _font = Asset.create(Asset.BitmapFont, "font", "gfx/font.png")
      Asset.bmpfntSet(_font, "abcdefghijklmnopqrstuvwxyz!?'$1234567890", 0, 1, 2, 5)

      _iconbg = Asset.create(Asset.Image, "iconbg", "gfx/icon-background.png")

      _icons = Asset.create(Asset.Sprite, "icons", "gfx/icons.png")
      Asset.spriteSet(_icons, 16, 16, 0, 0)
      _iconCount = 15

      _logo = Asset.create(Asset.Image, "hotair", "gfx/hotair.png")

      Asset.loadAll()

      _rnd = Random.new()

      _selectedItem = 0
      _currentIcon = 0
      _t = 0

      _items = [
         "start",
         "endless mode",
         "help",
         "quit"
      ]

      _actions = [
         Fn.new {
            nextScene = ["cutscene", "game"]
         },

         Fn.new {
            nextScene = ["game", "endless"]
         },

         Fn.new {
            nextScene = "help"
         },

         Fn.new {
            Trap.console("quit")
         }
      ]

      _snow = []
   }

   update(dt) {
      _t = _t + dt

      if (_t % 4 == 0) {
         // x, y, dx
         _snow.add([_rnd.int(320), 0, 0])
      }

      for (s in _snow) {
         s[1] = s[1] + dt
         s[2] = ((_t + s[0]) / 16).sin * 2
         s[0] = Math.lerp(s[0], s[0] + s[2], 0.1)
      }

      _snow = _snow.where{|s| s[1] < 180 }.toList

      var confirmed = Trap.keyPressed(Button.Start, 0, -1) || Trap.keyPressed(Button.A, 0, -1)
      var up = Trap.keyPressed(Button.Up, 0, -1)
      var down = Trap.keyPressed(Button.Down, 0, -1)

      if (up) {
         _selectedItem = _selectedItem - 1
         if (_selectedItem < 0) {
            _selectedItem = _items.count - 1
         }
      } else if (down) {
         _selectedItem = (_selectedItem + 1) % _items.count
      } else if (confirmed) {
         _actions[_selectedItem].call()
      }
   }

   draw(w, h) {
      Draw.clear()
      Draw.resetTransform()
      Draw.scale(h / 180)

      // grid squares
      Draw.setColor(17, 94, 51, 255)
      Draw.rect(0, 0, w, h, Fill.Solid)

      Draw.setColor(115, 41, 48, 255)

      var x = -32 + (_t / 2) % 64
      var y = -32 + (_t / 8) % 64
      while (x < w) {
         var innerY = y
         while (innerY < h) {
            Draw.rect(x, innerY, 32, 32, Fill.Solid)
            Draw.rect(x-32, innerY-32, 32, 32, Fill.Solid)
            innerY = innerY + 64
         }
         x = x + 64
      }

      for (s in _snow) {
         Draw.setColor(178, 220, 239, 255)
         Draw.rect(s[0], s[1], 2, 2, false)
         Draw.setColor(255, 255, 255, 255)
         Draw.rect(s[0] + 1, s[1], 1, 1, false)
      }

      //icon strip
      Draw.setColor(0, 0, 0, 150)
      Draw.rect(0, 0, w, 29, Fill.Solid)
      Draw.rect(0, 180-29, w, 29, Fill.Solid)

      Draw.setColor(255, 255, 255, 255)

      var cycle = _t / 4 % 32
      if (cycle == 0) {
         _currentIcon = (_currentIcon + 1) % _iconCount
      }

      x = -32
      var topIcon = _currentIcon
      var bottomIcon = _currentIcon
      while (x < w) {
         Draw.image(_iconbg, x - cycle, 5)
         Draw.sprite(_icons, topIcon, x - cycle +3, 5+1)

         Draw.image(_iconbg, x + cycle, 156)
         Draw.sprite(_icons, bottomIcon, x + cycle +3, 156+1)

         x = x + 32
         topIcon = (topIcon + 1) % _iconCount
         bottomIcon = bottomIcon == 0 ? _iconCount - 1 : bottomIcon - 1
      }

      Draw.image(_logo, 320/2 - 80, 46, 0, 0, 1.0, 2.0)

      for (i in 0..._items.count) {
         Draw.setColor(i == _selectedItem ? [255, 255, 0, 255] : [255, 255, 255, 255])
         var w = Asset.measureBmpText(_font, _items[i], 1)
         Draw.bmpText(_font, 320/2 - w/2, i*14 + 95, _items[i], 1)
      }
      
      Draw.setColor(255, 255, 255, 255)
   }

   shutdown() {
      Asset.clearAll()
   }  
}