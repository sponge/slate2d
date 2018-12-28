import "engine" for Asset, Draw
import "math" for Math
import "entity" for Entity
import "engine" for Trap

class Meter {
   construct new() {
      _meter = Asset.create(Asset.Image, "meter", "gfx/meter.png")
      _arrow = Asset.create(Asset.Image, "arrow", "gfx/arrow.png")
      _x = 16
      _xTarget = 16
      _t = 0
      _adjust = 0
      Asset.loadAll()
   }

   think(dt) {
      _t = _t + dt
      _adjust = (_t / 33).sin * 4 
      _x = Math.lerp(_x, _xTarget, dt * 0.1)
   }

   set(val) {
      _xTarget = (1 - val / 100) * 288 + 16
   }

   draw() {
      Draw.image(_meter, 16, 152)
      Draw.image(_arrow, _x +_adjust, 172)
   }
}