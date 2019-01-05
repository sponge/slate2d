import "entity" for Entity
import "engine" for Trap

// gates don't do anything but impede movement
class Gate is Entity {
  construct new(world, obj, ox, oy) {
    super(world, obj, ox, oy, 8, 8)
    _visible = obj["properties"]["startVisible"]
    if (_visible is Bool == false) {
      Trap.error(2, "Gate startVisible not bool at %(ox), %(oy)")
    }
  }

  canCollide(other, side, d) { _visible }

  activate(activator) {
    _visible = !_visible
  }

  draw(t) {
    if (_visible) {
      drawSprite(249, x, y)
    }
  }
}