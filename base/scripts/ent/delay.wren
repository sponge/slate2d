import "entity" for Entity
import "engine" for Trap
import "timer" for Timer

// delays will wait x seconds before calling activated(_) on their target
class Delay is Entity {
   construct new(world, obj, ox, oy) {
      super(world, obj, ox, oy, 8, 8)
      _target = obj["properties"]["target"]
      if (_target is String == false || _target.count == 0) {
         Trap.error(2, "Switch doesn't have valid target property at %(ox), %(oy)")
      }

      _delay = obj["properties"]["delay"]
      if (_delay is Num == false) {
         Trap.error(2, "Switch missing numeric property delay at %(ox), %(oy)")
      }

      _visible = true
   }

   activate(activator) {
      Timer.runLater(_delay, Fn.new {
         for (ent in world.entities) {
            if (ent != this && ent.name == _target) {
               ent.activate(this)
            }
         }
      })
   }

   canCollide(other, side, d) { false }
}