import "engine" for Draw, Asset, Trap, Button, Fill
import "math" for Math
import "entity" for Entity
import "random" for Random

class Mine is Entity {
   spr { _spr }
   gravity=(amt) { _gravity = amt }
   construct new (world, obj, x, y, dx, dy) {
      super(world, obj, x, y, 10, 10)
      name = "mine"

      _icons = Asset.find("icons")

      var iconCount = 15
      _spr = world.rnd.int(iconCount)
      _flip = 0

      _dx = dx
      _dy = dy
      _t = 0
   }
   
   think(dt) {
      x = x + _dx
      y = y + _dy

      // have a lifetime for the mines if they're moving
      if (_dx != 0 || _dy != 0) {
         _t = _t + dt
         if (_t > 60*30) {
            die(false)
         }
      }

      if (_gravity != null) {
         _dy = _dy + _gravity
      }
   }

   draw() {
      if (_flip) {
         Draw.sprite(_icons, 16, x-16, y-4, 1.0, 1.0, 1, 2, 2)
      } else {
         Draw.sprite(_icons, 16, x+24, y-4, 1.0, 1.0, 0, 2, 2)
      }
      Draw.sprite(_icons, _spr, x-3, y-3, 1.0, 1.0, 0, 1, 1)
   }

   die(hitByPlayer) {
      super()
      if (hitByPlayer) {
         world.onMineHit(this)
      }
   }
}