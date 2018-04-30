import "entity" for Entity
import "engine" for Trap

// switches will find an entity from target prop
// and call activated(_) on them
class Switch is Entity {
   construct new(world, obj, ox, oy) {
      super(world, obj, ox, oy, 8, 8)

      var targStr = obj["properties"]["target"]
      if (targStr is String == false || targStr.count == 0) {
         Trap.error(2, "Switch doesn't have valid target property at %(ox), %(oy)")
      }
      var targList = targStr.split(",")
      _target = {}
      for (targ in targList) {
         _target[targ] = true
      }

      _visible = true
   }

   activate(activator) {
      _visible = !_visible
   }

   canCollide(other, side, d) { _visible }

   touch(other, side) {
      for (ent in world.entities) {
         if (_target.containsKey(ent.name)) {
            ent.activate(this)
         }
      }

      _visible = false
   }

   draw(t) {
      if (_visible) {
         drawSprite(248, x, y)
      }
   }
}