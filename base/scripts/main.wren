import "meta" for Meta

// including most everything here for syntax validation
import "game" for Game
import "timer" for Timer
import "debug" for Debug
import "math" for Math
import "collision" for CollisionPool, Collision, TriggerInfo
import "tilecollider" for TileCollider
import "camera" for Camera
import "entity" for Entity
import "player" for Player
import "world" for World

class Main {
   static scene { __scene }

   static init(mapName) {
      CollisionPool.init()
      Timer.init()
      Debug.init()
      
      loadLevel(mapName)
   }

   static update(dt) {
      __scene.update(dt)
      Timer.tick(1)
   }

   static draw(w, h) {
      __scene.draw(w, h)
      Debug.draw()
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
      __scene = Game.new(mapName)
      System.gc()
   }
}