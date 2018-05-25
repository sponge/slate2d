import "entity" for Entity
import "collision" for Dir, Dim

class Cannonball is Entity {
   parent { _parent }
   parent=(ent) { _parent = ent }

   construct new(world, obj, ox, oy) {
      super(world, obj, ox, oy, 8, 8)
      dx = -0.5
      dy = 0
      _parent = null

      props["bouncy"] = true
   }

   canCollide(other, side, d) { true }
   trigger { true }

   touch(other, side) {
      active = false
      // don't hurt from the top
      if (other && other.isPlayer && side != Dir.Up) {
         other.hurt(this, 1)
      }
   }

   // move our speed every frame. if we hit something, activate touch on ourselves
   // and explode
   think(dt) {
      var chkx = check(Dim.H, dx)
      if (chkx.entity != null && chkx.entity != _parent) {
         touch(chkx.entity, chkx.side)
         return
      }

      x = x + chkx.delta
      if (chkx.delta != dx) {
         touch(null, chkx.side)
         return
      }
      
      var chky = check(Dim.V, dy)
      if (chky.entity != null && chky.entity != _parent) {
         touch(chky.entity, chky.side)
         return
      }

      y = y + chky.delta
      if (chky.delta != dy) {
         touch(null, chky.side)
         return
      }
      
      // die if we go off the level
      if ( y > world.level.maxY + world.level.th * 2 || y < 0) {
         touch(null, 0)
         return
      }

      if ( x > world.level.maxX || x < 0) {
         touch(null, 0)
         return
      }
   }

   draw(t) {
      drawSprite(270, x, y)      
   }
}