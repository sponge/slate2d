import "timer" for Timer
import "engine" for Draw, Asset, Trap, Color, Fill
import "debug" for Debug

class Intro {
   nextScene { _nextScene }
   nextScene=(params) { _nextScene = params }

   construct new(mapName) {
      _nextScene = null
      _time = 0
      Asset.loadAll()
   }

   update(dt) {
      _time = _time + dt
   }

   draw(w, h) {
      Draw.clear()
      Draw.resetTransform()
      Draw.transform(h / 180, 0, 0, h / 180, 0, 0)

      Draw.setColor(Color.Stroke, 255, 0, 0, 255)
      Draw.rect(160 + _time.sin * 80, 80, 16, 16, Fill.Outline)

      Debug.text("Title Screen", "Welcome to Clive Sturridge's Battlement Defense")

      Draw.submit()
   }

   shutdown() {

   }
}