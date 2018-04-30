import "entity" for Entity
import "engine" for Trap
import "timer" for Timer

// loops will activate all targets every (frequency) ticks
class Loop is Entity {
   construct new(world, obj, ox, oy) {
      super(world, obj, ox, oy, 8, 8)
      _world = world
      _startTime = 0

      _active = obj["properties"]["startActive"]
      if (_active is Bool == false) {
         Trap.error(2, "Loop startActive not bool at %(ox), %(oy)")
      }

      _target = obj["properties"]["target"]
      if (_target is String == false || _target.count == 0) {
         Trap.error(2, "Loop doesn't have valid target property at %(ox), %(oy)")
      }

      _frequency = obj["properties"]["frequency"]
      if (_frequency is Num == false) {
         Trap.error(2, "Loop missing numeric property frequency at %(ox), %(oy)")
      }
   }

   activate(activator) {
      _startTime = world.ticks
      _active = !_active

   }

   think(dt) {
      if (!_active) {
         return
      }

      if ((world.ticks - _startTime) % _frequency == 0) {
         for (ent in world.entities) {
            if (ent.name == _target) {
               ent.activate(this)
            }
         }
      }
   }

   canCollide(other, side, d) { false }
}