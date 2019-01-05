import "entity" for Entity
import "engine" for Trap
import "ent/logicent" for LogicEnt

// PlayerTriggers are rectangle shapes that will activate when a player steps in them
class PlayerTrigger is LogicEnt {
  trigger { true }
  canCollide(other, side, d){ true }

  construct new(world, obj, ox, oy) {
    Trap.printLn(obj)
    super(world, obj, ox, oy, obj["width"], obj["height"])

    _cooldown = obj["properties"]["cooldown"]
    if (_cooldown is Num == false) {
      Trap.error(2, "AreaTrigger missing numeric property cooldown at %(ox), %(oy)")
    }

    _triggerOnce = obj["properties"]["triggerOnce"]
    if (_triggerOnce is Bool == false) {
      Trap.error(2, "AreaTrigger missing bool property triggerOnce at %(ox), %(oy)")
    }

    _activateTime = 0
  }

  touch(other, side) {
    if (other.isPlayer == false || world.ticks < _activateTime) {
      return
    }

    activateTargets()

    if (_triggerOnce) {
      active = false
      return
    }

    _activateTime = world.ticks + _cooldown
  }
}