import "meta" for Meta
import "engine" for Trap, Draw, Asset, CVar

import "timer" for Timer
import "debug" for Debug
import "collision" for CollisionPool
import "world" for World
import "intro" for Intro
import "test" for Title
import "soundcontroller" for SoundController

class Main {
  static scene { __scene }

  static init(mapName) {
    CollisionPool.init()
    Timer.init()
    Debug.init()
    SoundController.init()
    __accumTime = 0

    __inspector = CVar.get("wren_inspector", 0)

    if (mapName == null) {
      loadScene("title", null)
      // loadScene("world", "maps/plat2.tmx")
    } else {
      loadScene("intro", mapName)
    }
  }

  static update(dt) {
    if (__scene && __scene.nextScene != null) {
      Trap.printLn("got scene transfer: %(__scene.nextScene)")
      if (__scene.nextScene is String) {
        loadScene(__scene.nextScene, null)
      } else {
        var params = __scene.nextScene.count > 1 ? __scene.nextScene[1] : null
        loadScene(__scene.nextScene[0], params)
      }
    }

    __accumTime = __accumTime + dt
    if (__accumTime >= 1/60) {
      __accumTime = __accumTime - 1/60
    } else {
      return
    }

    Debug.persist(true)
    Debug.clearPersist()

    if (__scene != null) {
      __scene.update(1)
    }
    Timer.tick(1)

    // see the hack comment in engine.wren for why this is
    Trap.clearKeyPressed()
  }

  static draw(w, h) {
    Debug.persist(false)
    Draw.clear()
    if (__scene != null) {
      __scene.draw(w, h)
    }
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

    if (scene == "intro") {
      __scene = Intro.new(params)
    } else if (scene == "world") {
      __scene = World.new(params)
    } else if (scene == "title") {
      __scene = Title.new("maps/8x8.tmx")
    }

    System.gc()
  }
}