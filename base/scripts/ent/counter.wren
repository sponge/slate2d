import "entity" for Entity
import "engine" for Trap
import "ent/logicent" for LogicEnt

// counters need to be triggered x times in order to activate their c
class Counter is LogicEnt {
   construct new(world, obj, ox, oy) {
      super(world, obj, ox, oy, 8, 8)

      _count = obj["properties"]["count"]
      if (_count is Num == false) {
         Trap.error(2, "Counter missing numeric property count at %(ox), %(oy)")
      }

      _currentCount = 0
   }

   activate(activator) {
      _currentCount = _currentCount + 1
      if (_currentCount >= _count) {
         activateTargets()
      }
   }
}