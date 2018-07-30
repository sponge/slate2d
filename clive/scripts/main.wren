import "meta" for Meta
import "engine" for Trap, Draw, Asset, CVar
import "timer" for Timer
import "debug" for Debug
import "soundcontroller" for SoundController

import "intro" for Intro
import "intromessage" for IntroMessage
import "gameselect" for GameSelect
import "gameinfo" for Game1Info, Game2Info, Game3Info, Game4Info, Game5Info, GameInfo
import "gameover" for GameOver

import "game1/title" for Game1Title
import "game1/win" for Game1Win
import "game2/cracktro" for Game2Cracktro
import "game2/title" for Game2Title
import "game2/win" for Game2Win
import "game3/title" for Game3Title
import "game3/win" for Game3Win
import "game4/title" for Game4Title
import "game4/win" for Game4Win
import "game5/title" for Game5Title
import "game5/towers" for Towers
import "game5/ending" for TowersEnding

import "td" for TD

class Main {
   static scene { __scene }

   static init(params) {
      SoundController.init()
      Timer.init()
      Debug.init()
      __accumTime = 0

      __inspector = CVar.get("wren_inspector", 0)

      __scenes = {
         "intro": Intro,
         "intromessage": IntroMessage,
         "gameselect": GameSelect,
         "game1": Game1Info,
         "game2": Game2Info,
         "game3": Game3Info,
         "game4": Game4Info,
         "game5": Game5Info,
         "game1_title": Game1Title,
         "game1_win": Game1Win,
         "game2_cracktro": Game2Cracktro,
         "game2_title": Game2Title,
         "game2_win": Game2Win,
         "game3_title": Game3Title,
         "game3_win": Game3Win,
         "game4_title": Game4Title,
         "game4_win": Game4Win,
         "game5_title": Game5Title,
         "td": TD,
         "gameover": GameOver,
         "towers": Towers,
         "towers_ending": TowersEnding,
      }

      if (params != null) {
         Asset.clearAll()
         loadScene("td", params)
      } else {
         Asset.clearAll()
         loadScene("intro", null)
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
         __scene.update(1/60)
      }
      Timer.tick(1/60)

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

      //Asset.clearAll()
   }

   static loadScene(scene, params) {
      Timer.clear()
      shutdown()

      var next = scene == null || __scenes[scene] == null ? "intro" : scene
      __scene = __scenes[next].new(params)

      System.gc()
   }
}