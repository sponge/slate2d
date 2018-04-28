import "entity" for Entity
import "collision" for Dir, Dim
import "engine" for Trap
import "math" for Math

class Walker is Entity {
   construct new(world, obj, ox, oy) {
      super(world, obj, ox, oy, 8, 8)
      _flipped = -1
      _speed = 0.25
      _terminalVelocity = 2
      _stunTime = 0

      props["bouncy"] = true
      props["shootable"] = true
   }
   
   shot(other, time) {
      _stunTime = world.ticks + time
   }

   canCollide(other, side, d) { true }

   touch(other, side) {
      if (other.isPlayer == false) {
         return
      }

      if (side == Dir.Up) {
         active = false
         return
      }

      other.hurt(this, 1)
   }

   think(dt) {
      dy = Math.min(dy + world.gravity, _terminalVelocity)
      var checkY = check(Dim.V, dy)
      if (checkY.t < 1.0) {
         dy = 0
      }
      y = y + checkY.delta

      if (world.ticks > _stunTime) {
         var checkX = check(Dim.H, _speed * _flipped)
         if (checkX.entity && checkX.entity.isPlayer) {
            checkX.entity.hurt(this, 1)
         }
         x = x + checkX.delta

         if (checkX.t != 1.0) {
            _flipped = _flipped * -1
         }
      }

      if (y > world.level.maxY + 10) {
         die(null)
      }
   }

   draw(t) {
      var drawX = x
      if (world.ticks < _stunTime) {
         drawX = x + (world.ticks).sin / 2
      }

      var offset = world.ticks / 8 % 2
      drawSprite(284 + offset, drawX, y, 1, 1, _flipped == 1 ? 1 : 0)
   }
}