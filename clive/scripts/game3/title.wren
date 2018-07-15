import "timer" for Timer
import "engine" for Draw, Asset, Trap, Color, Fill, Button
import "math" for Math
import "soundcontroller" for SoundController


class Game3Title {
   nextScene { _nextScene }
   nextScene=(params) { _nextScene = params }

   construct new(params) {
      _nextScene = null
      _time = 0
      _mode = "glitch"

      SoundController.init()

      _glitch = Asset.create(Asset.Image, "glitch", "gfx/game3/glitch.png")
      _gettingready = Asset.create(Asset.Speech, "gettingready", "getting ready")
      _count48 = Asset.create(Asset.Speech, "count48", "48")
      _count47 = Asset.create(Asset.Speech, "count47", "47")
      _count46 = Asset.create(Asset.Speech, "count46", "46")
      _goodmorning = Asset.create(Asset.Sound, "goodmorning", "sound/goodmorning.ogg")

      Asset.loadAll()

      _actions = [
         [1, Fn.new { Trap.sndPlay(_gettingready) }],
         [3, Fn.new { Trap.sndPlay(_count48) }],
         [5, Fn.new { Trap.sndPlay(_gettingready) }],
         [7, Fn.new { Trap.sndPlay(_count47) }],
         [9, Fn.new { Trap.sndPlay(_gettingready) }],
         [11, Fn.new { Trap.sndPlay(_count46) }],
         [13, Fn.new { Trap.sndPlay(_gettingready) }],
         [15, Fn.new { SoundController.playMusic(_goodmorning) }],
         [17, Fn.new { _mode = "" }],  
         [25, Fn.new {
            SoundController.stopMusic()
            _nextScene = ["td", "maps/e1m1.tmx"]
         }]       
      ]
   }

   update(dt) {
      _time = _time + dt
   }

   draw(w, h) {
      Draw.clear()
      Draw.resetTransform()
      Draw.transform(h / 180, 0, 0, h / 180, 0, 0)

      if (_actions.count > 0 && _time >= _actions[0][0]) {
         _actions[0][1].call()
         _actions.removeAt(0)
      }

      if (_mode == "glitch") {
         Draw.image(_glitch, 0, 0)
      }
      
      Draw.submit()
   }

   shutdown() {

   }
}