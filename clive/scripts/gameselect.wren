import "timer" for Timer
import "engine" for Draw, Asset, Trap, Color, Fill, Button
import "debug" for Debug
import "uibutton" for GameSelectButton

class GameSelect {
   nextScene { _nextScene }
   nextScene=(params) { _nextScene = params }

   construct new(params) {
      _nextScene = null
      _time = 0

      _banner = Asset.create(Asset.Sprite, "banner", "gfx/gameselect_banner.png")
      Asset.spriteSet(_banner, 150, 16, 0, 0)

      var y = 200
      var width = 200
      var spacing = 45
      var x = spacing
      _items = []
      _items.add(GameSelectButton.new("game1", x, y, width, 350, "Battlement Defence", "gfx/box_1.png"))
      x = x + width + spacing
      _items.add(GameSelectButton.new("game2", x, y, width, 350, "Battlement Defence 2", "gfx/box_2.png"))
      x = x + width + spacing
      _items.add(GameSelectButton.new("game3", x, y, width, 350, "Battlement Defense Returns", "gfx/box_1.png"))
      x = x + width + spacing
      _items.add(GameSelectButton.new("game4", x, y, width, 350, "Battlement ReDefended", "gfx/box_1.png"))
      x = x + width + spacing
      _items.add(GameSelectButton.new("game5", x, y, width, 350, "Battlement Towers.", "gfx/box_1.png"))

      _bodyFont = Asset.create(Asset.Font, "body", "fonts/Roboto-Regular.ttf")
      Trap.printLn(_bodyFont)
      _currentItem = ""

      Asset.loadAll()
   }

   update(dt) {
      _time = _time + dt
      var mouse = Trap.mousePosition()

      _currentItem = ""
      for (item in _items) {
         item.update(dt, mouse[0], mouse[1])
         if (item.clicked(mouse[0], mouse[1])) {
            Trap.printLn("clicked %(item.id)")
            _nextScene = [item.id, ""]
         }

         if (item.hover) {
            _currentItem = item.label
         }
      }
   }

   draw(w, h) {
      Draw.clear()

      Draw.setColor(Color.Fill, 0, 57, 113, 255)
      Draw.rect(0, 0, w, h, false)

      Draw.resetTransform()
      Draw.transform(h / 720, 0, 0, h / 720, 0, 0)

      for (item in _items) {
          item.draw()
      }

      Draw.sprite(_banner, 0, w/2 - 225, 10, 1, 3)

      Draw.setTextStyle(0, 48)
      // shadow
      Draw.setColor(Color.Fill, 0, 0, 0, 255)
      Draw.text(640, 153, 1000, _currentItem)
      // text
      Draw.setColor(Color.Fill, 255, 255, 255, 255)
      Draw.text(640, 150, 1000, _currentItem)

      Draw.submit()
   }

   shutdown() {

   }
}