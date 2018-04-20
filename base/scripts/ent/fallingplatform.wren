import "entity" for Entity
import "collision" for Dir

class FallingPlatform is Entity {
   platform { true }
   construct new(world, obj, ox, oy) {
      super(world, obj, ox, oy, 24, 4)

      _fallTime = 0
      _fallSpeed = 1
   }

   canCollide(other, side, d) {
      return side == Dir.Up && other.y+other.h <= y && other.y+other.h+d > y
   }

   touch(other, side) {
      if (other.isPlayer == false) {
         return
      }

      // if a player touches us, wait a bit and then start falling
      if (_fallTime == 0) {
         _fallTime = other.world.ticks + 10
         dy = _fallSpeed
      }
   }

   think(dt) {
      if (_movedTime == world.ticks) {
         return
      }
      _movedTime = world.ticks

      // die if we've fallen off the level
      if ( y > world.level.maxY + world.level.th * 2) {
         active = false
         return
      }

      // keep moving down
      if (_fallTime > 0 && world.ticks > _fallTime) {
         y = y + dy
      }

   }

   draw(t) {
      drawSprite(244, x, y)
      drawSprite(244, x+8, y)
      drawSprite(244, x+16, y)
   }
}