import "meta" for Meta
import "engine" for Trap, Draw

import "timer" for Timer
import "debug" for Debug
import "collision" for CollisionPool
import "world" for World

class Main {
   static scene { __scene }

   static init(mapName) {
      CollisionPool.init()
      Timer.init()
      Debug.init()
      __accumTime = 0
      
      loadLevel(mapName)
   }

   static update(dt) {
      __accumTime = __accumTime + dt
      if (__accumTime >= 1/60) {
         __accumTime = __accumTime - 1/60
      } else {
         return
      }

      Debug.persist(true)
      Debug.clearPersist()

      __scene.update(1)
      Timer.tick(1)
   }

   static draw(w, h) {
      Debug.persist(false)
      Draw.clear()
      __scene.draw(w, h)
      Debug.draw()
      Draw.submit()
   }

   static console(line) {
      Meta.eval(line)
   }

   static shutdown() {
      __scene.shutdown()
      __scene = null
   }

   static loadIntro(mapName) {
      // Timer.clear()
      // __scene = Intro.new(mapName)
      // System.gc()
   }

   static loadLevel(mapName) {
      Timer.clear()
      __scene = World.new(mapName)
      System.gc()
   }
}