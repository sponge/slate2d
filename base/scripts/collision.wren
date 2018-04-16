// FIXME: duped
var DIM_HORIZ = 1
var DIM_VERT = 2

var DIR_LEFT = 1
var DIR_RIGHT = 2
var DIR_TOP = 4
var DIR_BOTTOM = 8

// don't bother releasing since nothing ever needs for more than a single frame.
// just cycle through the 16
class CollisionPool {
   static init() {
      __pool = []
      __curr = 0
      __max = 16

      for (i in 1..__max) {
         __pool.add(Collision.new(0, null, 0))
      }
   }
   
   static get() {
      var res = __pool[__curr]
      res.clear()
      __curr = (__curr + 1) % __max
      return res
   }
}

// storage class since we need to know distance to trigger to filter
// them out during movement
class TriggerInfo {
   delta { _delta }
   entity { _entity }
   construct new(delta, entity) {
      _delta = delta
      _entity = entity
   }
}

// storage class for collision. don't alloc these directly, use CollisionPool
class Collision {
   delta { _delta }
   entity { _entity }
   entity=(e) { _entity = e }
   side { _side }
   triggers { _triggers }

   construct new(delta, entity, side) {
      set(delta, entity, side)
   }

   clear() {
      _delta = 0
      _entity = null
      _side = 0
      _triggers = []
   }

   // since we can trigger multiple entities per frame, we need to store
   // all that we can possibly collide with, and then filter out ones that
   // are too far away at the end.
   addTrigger(delta, entity) {
      _triggers.add(TriggerInfo.new(delta, entity))
   }

   filterTriggers(delta) {
      if (_triggers.count == 0) {
         return
      }

      for (i in _triggers.count-1..0) {
         if (delta.abs < _triggers[i].delta.abs) {
            _triggers.removeAt(i)
         }
      }
   }

   // returns true if the specified ent class is one that the collision ran into
   // should probably use properties or some sort of ECS-esque system but this
   // works good enough
   triggerHas(classname) {
      for (t in _triggers) {
         if (t.entity is classname) {
            return true
         }
      }

      return false
   }

   set(delta, entity, side) {
      _delta = delta
      _entity = entity
      _side = side

      return this
   }
}

class TileCollider {
   construct new(getTile, tileWidth, tileHeight) {
      _tw = tileWidth
      _th = tileHeight
      _getTile = getTile
   }

   getTileRange(ts, x, w, d) {
      var gx = (x / ts).floor
      var right = x+w+d
      right = right == right.floor ? right - 1 : right
      var gx2 = d >= 0 ? (right / ts).floor : ((x+d) / ts).floor
      //Debug.text("gtr", "%(x) %(w) %(d) %(gx..gx2)")

      return gx..gx2
   }

   query(x, y, w, h, dim, d, resolveFn) {
      if (dim == DIM_HORIZ) {
         var origPos = x + d
         var xRange = getTileRange(_tw, x, w, d)
         var yRange = getTileRange(_th, y, h, 0)

         for (tx in xRange) {
            for (ty in yRange) {
               //Debug.rectb(tx*8, ty*8, 8, 8, 4)
               var tile = _getTile.call(tx, ty)
               if (tile > 0) {
                  var dir = d < 0 ? DIR_LEFT : DIR_RIGHT
                  if (resolveFn.call(dir, tile, tx, ty, d, 0) == true) {
                     //Debug.rectb(tx*8, ty*8, 8, 8, 8)
                     var check = origPos..(tx + (d >= 0 ? 0 : 1)) *_tw - (d >= 0 ? w : 0)
                     return (d < 0 ? check.max : check.min) - x
                  }
               }
            }
         }

         return d
      } else {
         var origPos = y + d
         var xRange = getTileRange(_tw, x, w, 0)
         var yRange = getTileRange(_th, y, h, d)

         for (ty in yRange) {
            for (tx in xRange) {
               //Debug.rectb(tx*8, ty*8, 8, 8, 4)
               var tile = _getTile.call(tx, ty)
               if (tile > 0) {
                  var dir = d < 0 ? DIR_TOP : DIR_BOTTOM
                  if (resolveFn.call(dir, tile, tx, ty, 0, d) == true) {
                     //Debug.rectb(tx*8, ty*8, 8, 8, 8)
                     var check = origPos..(ty + (d >= 0 ? 0 : 1)) *_th - (d >= 0 ? h : 0)
                     return (d < 0 ? check.max : check.min) - y
                  }
               }
            }
         }

         return d
      }
   }
}
