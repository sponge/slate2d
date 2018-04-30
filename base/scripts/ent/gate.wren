import "entity" for Entity

// gates don't do anything but impede movement
class Gate is Entity {
   construct new(world, obj, ox, oy) {
      super(world, obj, ox, oy, 8, 8)
   }

   canCollide(other, side, d) { true }

   activate(activator) {
      die(activator)
   }

   draw(t) {
      drawSprite(249, x, y)
   }
}