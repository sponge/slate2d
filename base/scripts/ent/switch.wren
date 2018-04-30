import "entity" for Entity
import "engine" for Trap

// switches will find an entity from target prop
// and call activated(_) on them
class Switch is Entity {
   construct new(world, obj, ox, oy) {
      super(world, obj, ox, oy, 8, 8)
      _target = obj["properties"]["target"]
      if (_target is String == false || _target.count == 0) {
         Trap.error(2, "Switch doesn't have valid target property at %(ox), %(oy)")
      }
   }

   canCollide(other, side, d) { true }

   touch(other, side) {
      Trap.printLn("hit switch")
      for (ent in world.entities) {
         if (ent.name == _target) {
            ent.activate(this)
         }
      }
      die(null)
   }

   draw(t) {
      drawSprite(248, x, y)
   }
}