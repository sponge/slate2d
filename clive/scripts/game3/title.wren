import "timer" for Timer
import "engine" for Draw, Asset, Trap, Color, Fill, Button, Align
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
      _title = Asset.create(Asset.Image, "game3_title", "gfx/game3/title.png")
      _gettingready = Asset.create(Asset.Speech, "gettingready", "getting ready")
      _count48 = Asset.create(Asset.Speech, "count48", "48")
      _count47 = Asset.create(Asset.Speech, "count47", "47")
      _count46 = Asset.create(Asset.Speech, "count46", "46")
      _goodmorning = Asset.create(Asset.Sound, "goodmorning", "sound/goodmorning.ogg")

      _teleText = Asset.create(Asset.Font, "teletext", "fonts/TeleTekst.ttf")
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
      Draw.transform(h / 180, 0, 0, h / 180, 0, 0)

      if (_mode == "glitch") {
         Draw.image(_glitch, 0, 0)
      } else if (_mode == "countdown") {
         var warming = "Warming up now"
         var warmWidth = Asset.measureBmpText(_font, warming)
         Draw.bmpText(_font, 160 - warmWidth/2, 4, warming)

         var count = Math.clamp(0,(100 - (_time - _countdownStartTime) * _countdownSpeedupFactor).floor, 100).toString
         var countWidth = Asset.measureBmpText(_font, "99", 4)
         Draw.bmpText(_font, 160 - countWidth/2, 72, count, 4)

         var presented = "Presented By Clive"
         var presentedWidth = Asset.measureBmpText(_font, presented)
         Draw.bmpText(_font, 160 - presentedWidth/2, 168, presented)

         if (_skipWarningTime) {
            var y = 64
            Draw.setColor(Color.Fill, 0, 0, 0, 255)
            Draw.rect(0, y, 360, 64, Fill.Solid)
            Draw.bmpText(_font, 0, y, _warningMessage)
         }
      } else if (_mode == "title") {
         Draw.image(_title, 160-108, 16)
         Draw.setColor(Color.Fill, 255, 255, 255, 255)
         Draw.setTextStyle(_teleText, 10, 1.0, Align.Center|Align.Top)
         Draw.text(0, 4, 320, "CLIVE STURRIDGE'S")
         Draw.text(0, 118, 320, "FREE PLAY")
         Draw.text(0, 164, 320, "©1990 REASONABLE SOFTWARE LTD")

         if ((_time * 2).floor % 2 == 0) {
            Draw.setColor(Color.Fill, 0, 149, 233, 255)
            Draw.text(0, 132, 320, "PUSH START")
         }
      }
      
      Draw.submit()
   }

   shutdown() {

   }
}