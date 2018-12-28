import "engine" for Draw, Asset, Trap, Button, Fill
import "math" for Math
import "entity" for Entity
import "random" for Random

class Mine is Entity {
   construct new(world, obj, x, y, w, h) {
      super(world, obj, x, y, w, h)

      _icons = obj["sprite"]

      var iconCount = 8
      var rnd = Random.new()
      _spr = rnd.int(iconCount)
   }

   think(dt) {

   }

   draw() {
      Draw.sprite(_icons, _spr, x, y, 1.0, 1.0, 0, 1, 1)
   }

   die() {
      super()
   }
}