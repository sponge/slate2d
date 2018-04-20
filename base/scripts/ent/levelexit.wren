import "entity" for Entity
import "engine" for Trap

class LevelExit is Entity {
   construct new(world, obj, ox, oy) {
      super(world, obj, ox, oy, 8, 8)

      _nextLevel = obj["properties"]["next"]

      if (_nextLevel is String == false) {
         Trap.error(2, "LevelExit without next string property at %(ox), %(oy)")
      }
   }

   canCollide(other, side, d) { other.isPlayer == true }
   trigger { true }

   draw(t) {
      drawSprite(268 + (t / 7 % 2).floor, x, y)
   }

   touch(other, side) {
      active = false
      world.winLevel(_nextLevel)
   }
}