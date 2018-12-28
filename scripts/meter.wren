import "engine" for Asset, Draw
import "math" for Math
import "entity" for Entity

class Meter {
   construct new() {
      _meter = Asset.create(Asset.Image, "meter", "gfx/meter.png")
      _arrow = Asset.create(Asset.Image, "arrow", "gfx/arrow.png")
      _x = 64
      _xTarget = 64
      Asset.loadAll()
   }

   think(dt) {
      _x = Math.lerp(_x, _xTarget, dt * 0.1)
   }

   increase() {
      _xTarget = _x + 96
   }

   draw() {
      Draw.image(_meter, 16, 152)
      Draw.image(_arrow, _x, 172)
   }
}