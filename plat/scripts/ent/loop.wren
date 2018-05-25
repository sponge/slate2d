import "entity" for Entity
import "engine" for Trap
import "timer" for Timer
import "ent/logicent" for LogicEnt

// loops will activate all targets every (frequency) ticks
class Loop is LogicEnt {
   construct new(world, obj, ox, oy) {
      super(world, obj, ox, oy, 8, 8)
      _startTime = 0

      _enabled = obj["properties"]["startActive"]
      if (_enabled is Bool == false) {
         Trap.error(2, "Loop startActive not bool at %(ox), %(oy)")
      }

      _frequency = obj["properties"]["frequency"]
      if (_frequency is Num == false) {
         Trap.error(2, "Loop missing numeric property frequency at %(ox), %(oy)")
      }
   }

   activate(activator) {
      _startTime = world.ticks
      _enabled = !_enabled
   }

   think(dt) {
      if (!_enabled) {
         return
      }

      if ((world.ticks - _startTime) % _frequency == 0) {
         activateTargets()
      }
   }
}