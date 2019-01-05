import "engine" for Asset, Draw, Fill
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
    _meterMin = 14
  }

  think(dt) {
    _t = _t + dt
    _adjust = (_t / 33).sin * 4
    _x = Math.lerp(_x, _xTarget, dt * 0.1)
  }

  set(val) {
    _xTarget = (1 - val / 100) * 288 + _meterMin
  }

  draw() {
    var x = Math.clamp(_meterMin, _x + _adjust, _meterMin + 288)
    Draw.image(_meter, 16, 152)
    Draw.image(_arrow, x, 171)

    Draw.setColor(0, 0, 0, 150)
    Draw.rect(x + 3, 162, 2, 9, Fill.Solid)
    Draw.setColor(255, 255, 255, 255)
  }
}