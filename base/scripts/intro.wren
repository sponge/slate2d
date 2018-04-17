import "timer" for Timer
import "engine" for Draw
import "main" for Scene

class Intro {
   construct new(num) {
      _num = num

      Timer.runLater(120, Fn.new {
         Scene.level(_num)
      })
   }

   update(t) {

   }

   draw(t) {
      // TIC.cls(2)
      // TIC.print("ENTERING LEVEL %(_num+1)", 70, 60)
   }
}