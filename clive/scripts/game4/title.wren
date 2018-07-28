import "timer" for Timer
import "engine" for Draw, Asset, Trap, Color, Fill, Button, Align
import "math" for Math
import "soundcontroller" for SoundController
import "actionqueue" for ActionQueue

class Game4Title {
   nextScene { _nextScene }
   nextScene=(params) { _nextScene = params }

   construct new(params) {
      _nextScene = null
      _time = 0

      _music = Asset.create(Asset.Sound, "game4_title", "sound/game4_title.ogg")
      _font = Asset.create(Asset.Font, "body", "fonts/Roboto-Regular.ttf")

      Asset.loadAll()      

      _actions = [
         [2, Fn.new {
            SoundController.playMusic(_music)
            _drawIntro = true
         }]
      ]
      _actionQueue = ActionQueue.new(_actions)
   }

   update(dt) {
      _time = _time + dt
      _actionQueue.update(dt)

      if (_time > 1 && Trap.keyPressed(Button.B, 0, -1)) {
         _nextScene = ["td", "maps/e4m1.tmx"]
      }
   }

   draw(w, h) {
      if (_drawIntro != true) {
         return
      }

      Draw.clear()
      Draw.resetTransform()
      Draw.transform(h / 720, 0, 0, h / 720, 0, 0)

      Draw.setColor(Color.Fill, 255, 255, 255, 255)
      Draw.setTextStyle(_font, 48, 1.0, Align.Left+Align.Top)

      Draw.text(0, 0, 1280, "WIP: game 4 intro")

      Draw.submit()
   }

   shutdown() {

   }
}