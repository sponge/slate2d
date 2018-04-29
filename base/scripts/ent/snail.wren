import "entity" for Entity
import "collision" for Dir, Dim
import "engine" for Trap
import "math" for Math

class Snail is Entity {
   construct new(world, obj, ox, oy) {
      super(world, obj, ox, oy, 8, 8)
      _speed = 0.25
      _terminalVelocity = 2
      _stunTime = 0
      _shell = false
      _shellSpeed = 1

      dx = -_speed

      props["bouncy"] = true
      props["shootable"] = true
   }
   
   shot(other, time) {
      if (_shell) {
         dx = other.dx > 0 ? _shellSpeed : -_shellSpeed
      } else {
         _stunTime = world.ticks + time
      }
   }

   canCollide(other, side, d) { true }

   touch(other, side) {
      if (other.isPlayer == false) {
         return
      }

      if (_shell) {
         if (side == Dir.Up || side == Dir.Down) {
            dx = centerX > other.centerX ? _shellSpeed : -_shellSpeed
         } else {
            dx = side == Dir.Left ? -_shellSpeed : _shellSpeed
         }
      } else {
         if (side == Dir.Up) {
            _shell = true
            _stunTime = 0
            dx = 0
         } else {
            other.hurt(this, 1)
         }
      }
   }

   think(dt) {
      dy = Math.min(dy + world.gravity, _terminalVelocity)
      var checkY = check(Dim.V, dy)
      if (checkY.t < 1.0) {
         dy = 0
      }
      y = y + checkY.delta

      if (world.ticks > _stunTime && dx != 0) {
         var checkX = check(Dim.H, dx)
         if (dx != 0 && checkX.entity && checkX.entity.isPlayer) {
            checkX.entity.hurt(this, 1)
         }
         x = x + checkX.delta

         if (checkX.t != 1.0) {
            dx = -dx
         }
      }

      if (y > world.level.maxY + 10) {
         die(null)
      }
   }

   draw(t) {
      Trap.inspect(this)
      var drawX = x
      if (world.ticks < _stunTime) {
         drawX = x + (world.ticks).sin / 2
      }

      var offset = _shell ? 1 : 0 //world.ticks / 8 % 2
      drawSprite(288 + offset, drawX, y, 1, 1, dx > 0 ? 1 : 0)
   }
}