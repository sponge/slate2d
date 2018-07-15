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
      _countdownStartTime = 0

      SoundController.init()

      _glitch = Asset.create(Asset.Image, "glitch", "gfx/game3/glitch.png")
      _gettingready = Asset.create(Asset.Speech, "gettingready", "getting ready")
      _count48 = Asset.create(Asset.Speech, "count48", "48")
      _count47 = Asset.create(Asset.Speech, "count47", "47")
      _count46 = Asset.create(Asset.Speech, "count46", "46")
      _goodmorning = Asset.create(Asset.Sound, "goodmorning", "sound/goodmorning.ogg")

      _font = Asset.create(Asset.BitmapFont, "buttonfont", "gfx/game3/gradius.png")
      Asset.bmpfntSet(_font, "0123456789___?___ABCDEFGHIJKLMNOPQRSTUVWXYZ-.____abcdefghijklmnopqrstuvwxyz", 8, 0, 8, 8)


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
         [17, Fn.new {
            _mode = "countdown"
            _countdownStartTime = _time
         }],  
         [17 + (100/4), Fn.new {
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
      Draw.transform(h / 352, 0, 0, h / 352, 0, 0)

      if (_actions.count > 0 && _time >= _actions[0][0]) {
         _actions[0][1].call()
         _actions.removeAt(0)
      }

      if (_mode == "glitch") {
         Draw.image(_glitch, 0, 0)
      } else if (_mode == "countdown") {
         var warming = "Warming up now"
         var warmWidth = Asset.measureBmpText(_font, warming, 2)
         Draw.bmpText(_font, 320 - warmWidth/2, 16, warming, 2)

         var count = Math.clamp(0,(100 - (_time - _countdownStartTime) * 2).floor, 100).toString
         Draw.bmpText(_font, 240, 136, count, 10)

         var presented = "Presented By Clive"
         var presentedWidth = Asset.measureBmpText(_font, presented, 2)
         Draw.bmpText(_font, 320 - presentedWidth/2, 352-32, presented, 2)
      }
      
      Draw.submit()
   }

   shutdown() {

   }
}