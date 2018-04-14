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