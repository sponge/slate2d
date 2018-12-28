import "engine" for Draw, Asset, Trap, Button, Fill
import "math" for Math
import "entity" for Entity

class Mine is Entity {
   spr { _spr }
   construct new (world, obj, x, y) {
      super(world, obj, x, y, 10, 10)

      _icons = obj["sprite"]

      var iconCount = 8
      _spr = world.rnd.int(iconCount)
   }
   
   think(dt) {

   }

   draw() {
      Draw.sprite(_icons, _spr, x-3, y-3, 1.0, 1.0, 0, 1, 1)
   }

   die(hitByPlayer) {
      super()
      if (hitByPlayer) {
         world.onMineHit(this)
      }
   }
}