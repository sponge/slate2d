import "ent/cannonball" for Cannonball

class StunShot is Cannonball {
   trigger { true }
   canCollide(other, side, d) { other != parent }

   construct new(player, world, obj, ox, oy) {
      super(world, obj, ox, oy, 6, 6)
      dx = 2
      dy = 0
      parent = player
      parent.shotsActive = parent.shotsActive + 1
      _totalDistance = 0
      _endTime = 0
   }

   touch(other, side) {
      if (_endTime == 0) {
         _endTime = world.ticks
         x = x + dx * (dx > 0 ? 1 : 1)
         parent.shotsActive = parent.shotsActive - 1

         if (other && other.props["shootable"]) {
            other.shot(this, 180)
         }
      }
   }

   think(dt) {
      if (_endTime > 0 && world.ticks > _endTime) {
         if (world.ticks >= _endTime + 15) {
            active = false
         }
         return
      }

      super(dt)
      _totalDistance = _totalDistance + dx.abs
      if (_totalDistance > 100) {
         _endTime = world.ticks
         parent.shotsActive = parent.shotsActive - 1
      }
   }

   draw(t) {
      var anim = _endTime > 0 ? ((world.ticks - _endTime) / 5).floor + 1 : 0
      drawSprite(275 + anim, x, y)
   }
}