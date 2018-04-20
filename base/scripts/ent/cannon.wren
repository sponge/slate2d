import "entity" for Entity
import "collision" for Dim
import "engine" for Trap, Asset

import "ent/cannonball" for Cannonball

class Cannon is Entity {
   construct new(world, obj, ox, oy) {
      super(world, obj, ox, oy, 8, 8)

      var dir = obj["properties"]["direction"]
      if (dir == "up") {
         _tile = 238
         _dim = Dim.V
         _d = -0.5
      } else if (dir == "right") {
         _tile = 239
         _dim = Dim.H
         _d = 0.5
      } else if (dir == "down") {
         _tile = 240
         _dim = Dim.V
         _d = 0.5
      } else if (dir == "left") {
         _tile = 241
         _dim = Dim.H
         _d = -0.5
      } else {
         Trap.error(2, "Cannon at %(ox),%(oy) has no valid direction")
      }

      _fireTime = world.ticks + 60

      _sound = Asset.create(Asset.Sound, "cannon_shoot", "sound/cannon_shoot.wav")

   }

   canCollide(other, side, d) { true }

   think(dt) {
      if (world.ticks < _fireTime) {
         return
      }

      // don't fire if we're too close or too far away
      var distX = (world.player.x - x).abs
      var distY = (world.player.y - y).abs
      if ((distX <= 24 || distX > 200) && (distY <= 24 || distY > 200)) {
         // don't wait a full cycle to retry
         _fireTime = world.ticks + 60
         return
      }

      // spawn a cannonball, set parent to this so it doesn't immediately explode
      var ball = Cannonball.new(world, 270, x, y)
      ball.parent = this
      ball.dx = _dim == Dim.H ? _d : 0
      ball.dy = _dim == Dim.V ? _d : 0
      world.entities.add(ball)
      Trap.sndPlay(_sound)

      // recharge
      _fireTime = world.ticks + 300
   }

   draw(t) {
      drawSprite(_tile, x, y)      
   }
}