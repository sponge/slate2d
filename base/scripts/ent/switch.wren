import "entity" for Entity
import "engine" for Trap
import "ent/logicent" for LogicEnt

// switches will find an entity from target prop
// and call activated(_) on them
class Switch is LogicEnt {
   construct new(world, obj, ox, oy) {
      super(world, obj, ox, oy, 8, 8)
      _visible = true
   }

   activate(activator) {
      _visible = !_visible
   }

   canCollide(other, side, d) { _visible }

   touch(other, side) {
      activateTargets()
      _visible = false
   }

   draw(t) {
      if (_visible) {
         drawSprite(248, x, y)
      }
   }
}