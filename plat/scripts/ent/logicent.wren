import "entity" for Entity
import "engine" for Trap

// switches will find an entity from target prop
// and call activated(_) on them
class LogicEnt is Entity {
   construct new(world, obj, ox, oy, w, h) {
      super(world, obj, ox, oy, w, h)

      var targStr = obj["properties"]["target"]
      if (targStr is String == false) {
         Trap.error(2, "LogicEnt doesn't have valid target property at %(ox), %(oy)")
      }
      var targList = targStr.split(",")
      _target = {}
      for (targ in targList) {
         _target[targ] = true
      }
   }

   canCollide(other, side, d) { false }

   activateTargets() {
      for (ent in world.entities) {
         if (_target.containsKey(ent.name)) {
            ent.activate(this)
         }
      }
   }
}