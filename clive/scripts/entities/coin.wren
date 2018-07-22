import "engine" for Draw, Trap
import "entities/entity" for Entity

class Coin is Entity {
   construct new(td, x, y) {
      super(x, y, "coin")
      _td = td
      _time = 0
   }

   update(dt) {
      _time = _time + dt * 6
   }

   draw() {
      var frame = (_time % 4).floor
      Draw.sprite(_td.spr, 8 + (frame * 2), x * _td.tw, y * _td.th, 1.0, 1, 0, 2, 2)
   }
}