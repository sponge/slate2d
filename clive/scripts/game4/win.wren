import "engine" for Draw, Trap, Asset, Color, Button, Align
import "britnames" for BritNames
import "debug" for Debug

class Game4Win {
   nextScene { _nextScene }

   construct new(params) {
      Asset.clearAll()

      _nextScene = null
      _time = 0

      _font = Asset.create(Asset.Font, "body", "fonts/Roboto-Regular.ttf")

      _britNames = BritNames.new()
      _names = []
      for (i in 1...300) {
         Debug.printLn(_britNames.next())
      }

      Asset.loadAll()
   }

   shutdown() {
   }

   update(dt) {
      _time = _time + dt

   }

   draw(w, h) {
      Draw.clear()
      Draw.clear()
      Draw.transform(h/180, 0, 0, h/180, 0, 0)

      Draw.submit()
   }
}