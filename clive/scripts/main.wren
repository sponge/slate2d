import "meta" for Meta
import "engine" for Trap, Draw, Asset, CVar
import "timer" for Timer
import "debug" for Debug

import "intro" for Intro
import "gameselect" for GameSelect

class Main {
   static scene { __scene }

   static init(mapName) {
      Timer.init()
      Debug.init()
      __accumTime = 0

      __inspector = CVar.get("wren_inspector", 1)

      __scenes = {
         "intro": Intro,
         "gameselect": GameSelect
      }

      loadScene("intro", null)
   }

   static update(dt) {
      if (__scene.nextScene != null) {
         Trap.printLn("got scene transfer: %(__scene)")
         loadScene(__scene.nextScene[0], __scene.nextScene[1])
      }

      __accumTime = __accumTime + dt
      if (__accumTime >= 1/60) {
         __accumTime = __accumTime - 1/60
      } else {
         return
      }

      Debug.persist(true)
      Debug.clearPersist()

      __scene.update(1/60)
      Timer.tick(1/60)
   }

   static draw(w, h) {
      Debug.persist(false)
      Draw.clear()
      __scene.draw(w, h)
      Debug.draw()
      Draw.submit()

      if (__inspector.bool()) {
         Trap.inspect(__scene)
      }
   }

   static console(line) {
      var fiber
      var isExpression
      if (line.startsWith("eval ")) {
         line = line["eval ".count..-1]
         fiber = Meta.compile(line)
      } else {
         isExpression = true
         fiber = Meta.compileExpression(line)
      }

      if (fiber == null) return
      var result = fiber.try()
      if (fiber.error != null) {
         // TODO: Include callstack.
         Trap.printLn("Runtime error: %(fiber.error)")
         return
      }

      if (isExpression) {
         Trap.printLn(result)
      }
   }

   static shutdown() {
      if (__scene == null) {
         return
      }

      __scene.shutdown()
      __scene = null

      Asset.clearAll()
   }

   static loadScene(scene, params) {
      Timer.clear()
      shutdown()

      var next = scene == null || __scenes[scene] == null ? "intro" : scene
      __scene = __scenes[next].new(params)

      System.gc()
   }
}