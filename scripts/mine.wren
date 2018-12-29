import "engine" for Draw, Asset, Trap, Button, Fill
import "math" for Math
import "entity" for Entity

class Mine is Entity {
   spr { _spr }
   construct new (world, obj, x, y, dx, dy) {
      super(world, obj, x, y, 10, 10)

      _icons = obj["sprite"]
      _iconbg = Asset.create(Asset.Image, "iconbg", "gfx/icon-background.png")

      var iconCount = 9
      _spr = world.rnd.int(iconCount)

      _dx = dx
      _dy = dy
      _t = 0

      Asset.loadAll()
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
   }

   draw() {
      Draw.image(_iconbg, x-4, y-4)
      Draw.sprite(_icons, _spr, x-3, y-3, 1.0, 1.0, 0, 1, 1)
   }

   die(hitByPlayer) {
      super()
      if (hitByPlayer) {
         world.onMineHit(this)
      }
   }
}