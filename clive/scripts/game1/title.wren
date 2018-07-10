import "timer" for Timer
import "engine" for Draw, Asset, Trap, Color, Fill, Button
import "math" for Math


class Game1Title {
   nextScene { _nextScene }
   nextScene=(params) { _nextScene = params }

   construct new(mapName) {
      _nextScene = null
      _time = 0

      _clive = Asset.create(Asset.Image, "clive", "gfx/game1/logo_clive.png")
      _title = Asset.create(Asset.Image, "title", "gfx/game1/logo_title.png")
      _tower = Asset.create(Asset.Image, "tower", "gfx/game1/logo_tower.png")

      _x = 192
      _y = 72

      _towerStartTime = 2.0
      _towerAnimLength = 4.0
      _towerWidth = 156
      _towerHeight = 196

      Asset.loadAll()
   }

   update(dt) {
      _time = _time + dt

      if (_time > 3 && Trap.keyPressed(Button.B, 0, -1)) {
         _nextScene = ["game", 1]
      }
   }

   draw(w, h) {
      Draw.clear()
      Draw.resetTransform()
      Draw.transform(h / 360, 0, 0, h / 360, 0, 0)

      Draw.image(_clive, _x + 5, _y)

      if (_time >= 1) {
         Draw.image(_title, _x, _y + 30)
      }

      if (_time >= _towerStartTime) {
         var px = (Math.clamp(0.0, (_time - _towerStartTime) / _towerAnimLength, 1.0) * _towerHeight).ceil
         var blockPx = Math.clamp(0, px + (8 - px % 8), _towerHeight)
         Draw.image(_tower, _x + 100, _y + 20, _towerWidth, blockPx)
      }

      Draw.submit()
   }

   shutdown() {

   }
}