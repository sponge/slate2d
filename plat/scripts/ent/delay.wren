import "entity" for Entity
import "engine" for Trap
import "ent/logicent" for LogicEnt

// delays will wait x seconds before calling activated(_) on their target
class Delay is LogicEnt {
   construct new(world, obj, ox, oy) {
      super(world, obj, ox, oy, 8, 8)

      _delay = obj["properties"]["delay"]
      if (_delay is Num == false) {
         Trap.error(2, "Switch missing numeric property delay at %(ox), %(oy)")
      }

      _activateTime = null
   }

   activate(activator) {
      _activateTime = world.ticks + _delay
   }

   think(dt) {
      if (_activateTime != null && world.ticks >= _activateTime) {
         activateTargets()
         _activateTime = null
      }
   }
}