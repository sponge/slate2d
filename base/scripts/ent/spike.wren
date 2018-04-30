import "entity" for Entity
import "engine" for Trap

// spikes just hurt players when touched
class Spike is Entity {
   construct new(world, obj, ox, oy) {
      super(world, obj, ox, oy, 8, 8)
      _visible = obj["properties"]["startVisible"]
      if (_visible is Bool == false) {
         Trap.error(2, "Spike startVisible not bool at %(ox), %(oy)")
      }
   }

   canCollide(other, side, d) { _visible }
   
   activate(activator) {
      _visible = !_visible
   }

   touch(other, side) {
      if (other.isPlayer == false) {
         return
      }

      other.hurt(this, 1)
   }

   draw(t) {
      if (_visible) {
         drawSprite(242, x, y)
      }
   }
}