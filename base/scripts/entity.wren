import "math" for Math
import "collision" for CollisionPool
import "engine" for Draw
import "debug" for Debug

// FIXME: duped
var DIM_HORIZ = 1
var DIM_VERT = 2

var DIR_LEFT = 1
var DIR_RIGHT = 2
var DIR_TOP = 4
var DIR_BOTTOM = 8

class Entity {
   x { _x }
   x=(x) { _x = x }
   y { _y }
   y=(y) { _y = y }
   w { _w }
   w=(w) { _w = w }
   h { _h }
   h=(h) { _h = h }
   dx { _dx }
   dx=(dx) { _dx = dx }
   dy { _dy }
   dy=(dy) { _dy = dy }
   active { _active }
   active=(a) { _active = a }

   isPlayer { false }
   world { _world }
   resolve { _baseResolve }
   baseResolve { _baseResolve }
   
   construct new(world, ti, x, y, w, h) {
      _world = world
      _trigger = false
      _active = true
      _x = x
      _y = y
      _w = w
      _h = h
      _dx = 0
      _dy = 0

      // allow multiple entities to use this so they all react to the world similarly
      _baseResolve = Fn.new { |side, tile, tx, ty, ldx, ldy|
         if (tile == 0) {
            return false
         }

         if (tile >= 224) {
            // editor only item
            return false
         }

         if (tile >= 5 && tile <= 7) {
            //Debug.text("collision", "plat", "%(ty), %(side == DIR_BOTTOM) && %(y+h) <= %(ty*8) && %(y+h+ldy) > %(ty*8)")
            return side == DIR_BOTTOM && _y+_h <= ty*8 && _y+_h+ldy > ty*8
         }

         return true
      }
   }
   
   // returns true if this rect intersects with the other ent's rect
   intersects(other) {
      return Math.rectIntersect(x, y, w, h, other.x, other.y, other.w, other.h)
   }

   // modified SAT, always resolves based on the axis passed in, not the nearest
   // always checks one dimension per call
   collide(other, dim, d) {
      var ldx = dim == DIM_HORIZ ? d : 0
      var ldy = dim == DIM_VERT ? d : 0

      var ox = this.x + (this.w / 2) + ldx - other.x - (other.w / 2)
      var px = (this.w / 2) + (other.w / 2) - ox.abs

      if (px <= 0) {
         return d
      }
      
      var oy = this.y + (this.h / 2) + ldy - other.y - (other.h / 2)
      var py = (this.h / 2) + (other.h / 2) - oy.abs

      if (py <= 0) {
         return d
      }

      if (dim == DIM_HORIZ) {
         var rx = ldx + px * Math.sign(ox)
         return rx 
      } else {
         var ry = ldy + py * Math.sign(oy)
         return ry
      }
   }

   // one place to try moving through the world. checks tiles in the way, and all entities
   // probably want some sort of spatial partitioning eventually, but rect intersects are cheap
   check(dim, d) {
      var dir = dim == DIM_HORIZ ? (d > 0 ? DIR_RIGHT : DIR_LEFT) : (d > 0 ? DIR_TOP : DIR_BOTTOM)
      d = _world.tileCollider.query(_x, _y, _w, _h, dim, d, resolve)

      var colInfo = CollisionPool.get()

      if (d == 0) {
         return colInfo.set(d, null, dir)
      }

      var collideEnt = null

      for (ent in _world.entities) {
         if (ent != this && ent.active && (ent.w > 0 || ent.h > 0)) {
            var tmp = this.collide(ent, dim, d)
            if (tmp != d) {
               if (ent.canCollide(this, dir, d)) {
                  if (ent.trigger) {
                     colInfo.addTrigger(d, ent)
                  } else {
                     collideEnt = ent
                     d = tmp.abs < d.abs ? tmp : d
                  }

               }
            }
         }
      }

      colInfo.filterTriggers(d)

      return colInfo.set(d, collideEnt, dir)
   }

   // called from subclassed entities when you want to activate all entities
   // the collision is moving into
   triggerTouch(collision) {
      if (collision.entity != null) {
         collision.entity.touch(this, collision.side)
      }

      for (trigger in collision.triggers) {
         trigger.entity.touch(this, collision.side)
      }
   }

   drawSprite(id, x, y) {
      Draw.sprite(world.spr, id, x, y)
   }

   drawSprite(id, x, y, alph) {
      Draw.sprite(world.spr, id, x, y, alpha)
   }

   // return true or false based on if the receiving entity wants to collide this frame
   canCollide(other, side, d){ true } 
   // if true, it will not impede movement
   trigger { false }
   // if true, player will track and move before player moves
   platform { false }
   // called when another entity collides with you
   touch(other, side){}
   // called every frame
   think(dt){}
   draw(t){}
}