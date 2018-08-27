import "timer" for Timer
import "engine" for Draw, Asset, Trap, Fill, Button
import "math" for Math
import "soundcontroller" for SoundController
import "actionqueue" for ActionQueue

class Game1Title {
   nextScene { _nextScene }
   nextScene=(params) { _nextScene = params }

   construct new(params) {
      _nextScene = null
      _time = 0

      _clive = Asset.create(Asset.Image, "clive", "gfx/game1/logo_clive.png")
      _title = Asset.create(Asset.Image, "title", "gfx/game1/logo_title.png")
      _tower = Asset.create(Asset.Image, "tower", "gfx/game1/logo_tower.png")

      _stinger = Asset.create(Asset.Sound, "game1_logo", "sound/game1_logo.ogg")

      _drawClive = false
      _drawTitle = false
      _drawTower = false
      _towerStartTime = 0
      _towerAnimLength = 4.0
      _towerWidth = 156
      _towerHeight = 196

      Asset.loadAll()      

      _actions = [
         [1, Fn.new { _drawClive = true }],
         [1, Fn.new {
            SoundController.playOnce(_stinger)
            _drawTitle = true
         }],
         [2, Fn.new {
            _drawTower = true
            _towerStartTime = _time
         }],
         [_towerAnimLength + 3, Fn.new { _nextScene = ["td", "maps/e1m1.tmx"] }]
      ]
      _actionQueue = ActionQueue.new(_actions)
   }

   update(dt) {
      _time = _time + dt

      _actionQueue.update(dt)

      if (_time > 3 && Trap.keyPressed(Button.B, 0, -1)) {
         _nextScene = ["td", "maps/e1m1.tmx"]
      }
   }

   draw(w, h) {
      Draw.clear()
      Draw.resetTransform()
      Draw.scale(h / 360)

      Draw.translate(192, 72)
      
      if (_drawClive) {
         Draw.image(_clive, 5, 0)
      }

      if (_drawTitle) {
         Draw.image(_title, 0, 30)
      }

      if (_drawTower) {
         var px = (Math.clamp(0.0, (_time - _towerStartTime) / _towerAnimLength, 1.0) * _towerHeight).ceil
         var blockPx = Math.clamp(0, px + (8 - px % 8), _towerHeight)
         Draw.image(_tower, 100, 20, _towerWidth, blockPx)
      }
   }

   shutdown() {

   }
}