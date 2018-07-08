import "timer" for Timer
import "engine" for Draw, Asset, Trap, Color, Fill, Button
import "debug" for Debug
import "uibutton" for GameSelectButton

class GameSelect {
   nextScene { _nextScene }
   nextScene=(params) { _nextScene = params }

   construct new(mapName) {
      _nextScene = null
      _time = 0

      _items = [
         GameSelectButton.new("game1", 30, 10, 370, 350, "Battlement Defense", "gfx/box_1.png"),
         GameSelectButton.new("game2", 450, 10, 370, 350, "Battlement Defense 2", "gfx/box_1.png"),
         GameSelectButton.new("game3", 870, 10, 370, 350, "Battlement Revived", "gfx/box_1.png"),

         GameSelectButton.new("game4", 220, 360, 370, 350, "Battlement ReDefended", "gfx/box_1.png"),
         GameSelectButton.new("game5", 640, 360, 370, 350, "Towers.", "gfx/box_1.png"),
      ]

      Asset.loadAll()
   }

   update(dt) {
      _time = _time + dt
      var mouse = Trap.mousePosition()

      for (item in _items) {
         item.update(dt, mouse[0], mouse[1])
         if (item.clicked(mouse[0], mouse[1])) {
            Trap.printLn("clicked %(item.id)")
            _nextScene = [item.id, ""]
         }
      }
   }

   draw(w, h) {
      Draw.clear()
      Draw.resetTransform()
      Draw.transform(h / 720, 0, 0, h / 720, 0, 0)

      for (item in _items) {
          item.draw()
      }

      Draw.submit()
   }

   shutdown() {

   }
}