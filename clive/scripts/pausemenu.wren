import "engine" for Draw, Asset, Trap, Fill, Button, TileMap, CVar, Align
import "uibutton" for CoinButton, TextButton, UIButton
import "fonts" for Fonts

class PauseMenu {
   construct new(x, y, showEnding) {
      _bodyFont = Asset.create(Asset.Font, "body", Fonts.body)

      _x = x
      _y = y

      _pauseButton = UIButton.new("pause", 1280-24, 0, 24, 24)

      _items = []
      _items.add(TextButton.new("resume", x+30, y+30+0, 240, 40, "Resume"))
      _items.add(TextButton.new("menu", x+30, y+30+50, 240, 40, "Back to Menu"))
      if (showEnding) {
         _items.add(TextButton.new("ending", x+30, y+30+100, 240, 40, "View Ending"))
      }

      _showEnding = showEnding
   }

   construct new(x, y) {
      return PauseMenu.new(x, y, true)
   }
   
   pauseClicked() {
      var mouse = Trap.mousePosition()
      return _pauseButton.clicked(mouse[0], mouse[1])
   }

   pauseUpdate(dt) {
      var mouse = Trap.mousePosition()
      _pauseButton.update(dt, mouse[0], mouse[1])
   }

   pauseDraw() {
      if (_pauseButton.hover) {
         Draw.setColor(101, 157, 214, 255)
      } else {
         Draw.setColor(47, 112, 176, 255)
      }

      Draw.rect(_pauseButton.x, _pauseButton.y, _pauseButton.w, _pauseButton.h, Fill.Solid)

      Draw.setColor(255, 255, 255, 255)
      Draw.rect(_pauseButton.x+6, _pauseButton.y+6, 4, 12, false)
      Draw.rect(_pauseButton.x+15, _pauseButton.y+6, 4, 12, false)
   }

   anyClicked() {
      var mouse = Trap.mousePosition()
      for (item in _items) {
         if (item.clicked(mouse[0], mouse[1])) {
            Trap.printLn("clicked %(item.id)")
            return item.id
         }
      }

      return null
   }

   update(dt) {
      var mouse = Trap.mousePosition()

      for (item in _items) {
         item.update(dt, mouse[0], mouse[1])
      }
   }

   draw() {
      Draw.setColor(0, 57, 113, 255)
      Draw.rect(_x, _y, 300, _showEnding ? 200 : 150, false)
      for (item in _items) {
          item.draw()
      }
   }
}