import "timer" for Timer
import "engine" for Draw, Asset, Trap, Color, Fill, Button
import "math" for Math
import "soundcontroller" for SoundController
import "debug" for Debug

class Game3Title {
   nextScene { _nextScene }
   nextScene=(params) { _nextScene = params }

   construct new(params) {
      _nextScene = null

      _time = 0
      _currentActionTime = 0

      _mode = "glitch"
      _countdownStartTime = 0
      _countdownSpeedupFactor = 2
      _enableSkip = false
      _skipWarningTime = null

      SoundController.init()

      _glitch = Asset.create(Asset.Image, "glitch", "gfx/game3/glitch.png")
      _gettingready = Asset.create(Asset.Speech, "gettingready", "getting ready")
      _count48 = Asset.create(Asset.Speech, "count48", "48")
      _count47 = Asset.create(Asset.Speech, "count47", "47")
      _count46 = Asset.create(Asset.Speech, "count46", "46")
      _goodmorning = Asset.create(Asset.Sound, "goodmorning", "sound/goodmorning.ogg")

      _font = Asset.create(Asset.BitmapFont, "buttonfont", "gfx/game3/gradius.png")
      Asset.bmpfntSet(_font, "0123456789=__?___ABCDEFGHIJKLMNOPQRSTUVWXYZ-.____abcdefghijklmnopqrstuvwxyz", 8, 0, 8, 8)

      Asset.loadAll()

      _warningMessage = "========================================
=     WARNING SKIPPING RAM WARMUP     =
=   MAY PERMANENTLY DAMAGE HARDWARE   =
=       CLICK AGAIN TO CONTINUE       =
========================================"

      _actions = [
         [1, Fn.new { Trap.sndPlay(_gettingready) }],
         [2, Fn.new { Trap.sndPlay(_count48) }],
         [2, Fn.new { Trap.sndPlay(_gettingready) }],
         [2, Fn.new { Trap.sndPlay(_count47) }],
         [2, Fn.new { Trap.sndPlay(_gettingready) }],
         [2, Fn.new { Trap.sndPlay(_count46) }],
         [2, Fn.new { Trap.sndPlay(_gettingready) }],
         [2, Fn.new { SoundController.playMusic(_goodmorning) }],
         [2, Fn.new {
            _mode = "countdown"
            _countdownStartTime = _time
            _enableSkip = true
         }],  
         [3 + 100/_countdownSpeedupFactor, Fn.new {
            SoundController.stopMusic()
            _mode = "title"
         }]
      ]
   }

   update(dt) {
      _time = _time + dt

      if (_actions.count > 0 && _time - _currentActionTime >= _actions[0][0]) {
         _actions[0][1].call()
         _actions.removeAt(0)
         _currentActionTime = _time
      }

      if (_enableSkip && Trap.keyPressed(Button.B, 0, -1)) {
         if (_mode == "title") {
            _nextScene = ["td", "maps/e1m1.tmx"]
         } else if (_skipWarningTime == null) {
            Debug.printLn("enabling warning")
            _skipWarningTime = _time
         } else if (_skipWarningTime != null) {
            SoundController.stopMusic()
            _mode = "title"
            _actions = []
         }
      }

      if (_skipWarningTime != null && _time >= _skipWarningTime + 5) {
         _skipWarningTime = null
      }
   }

   draw(w, h) {
      Draw.clear()
      Draw.resetTransform()
      Draw.transform(h / 352, 0, 0, h / 352, 0, 0)

      if (_mode == "glitch") {
         Draw.image(_glitch, 0, 0)
      } else if (_mode == "countdown") {
         var warming = "Warming up now"
         var warmWidth = Asset.measureBmpText(_font, warming, 2)
         Draw.bmpText(_font, 320 - warmWidth/2, 16, warming, 2)

         var count = Math.clamp(0,(100 - (_time - _countdownStartTime) * _countdownSpeedupFactor).floor, 100).toString
         Draw.bmpText(_font, 240, 136, count, 10)

         var presented = "Presented By Clive"
         var presentedWidth = Asset.measureBmpText(_font, presented, 2)
         Draw.bmpText(_font, 320 - presentedWidth/2, 352-32, presented, 2)

         if (_skipWarningTime) {
            var y = 136
            Draw.setColor(Color.Fill, 0, 0, 0, 255)
            Draw.rect(0, y, 640, 80, Fill.Solid)
            Draw.bmpText(_font, 0, y, _warningMessage, 2)
         }
      } else if (_mode == "title") {
         Draw.bmpText(_font, 0, 0, "title screen", 2)
      }
      
      Draw.submit()
   }

   shutdown() {

   }
}