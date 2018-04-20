import "entity" for Entity

// spikes just hurt players when touched
class Spike is Entity {
   construct new(world, obj, ox, oy) {
      super(world, obj, ox, oy, 8, 8)
   }

   canCollide(other, side, d) { true }

   touch(other, side) {
      if (other.isPlayer == false) {
         return
      }

      other.hurt(this, 1)
   }

   draw(t) {
      drawSprite(242, x, y)
   }
}