import "entity" for Entity
import "engine" for Asset, Trap
import "collision" for Dir

// springs work like moving platforms, and will be called from the player's think early on
// this would probably be cleaner if i could query to see if any entities are standing on the spring
// and trigger the bounce on them instead of having the player check.
class Spring is Entity {
   platform { true }

   construct new(world, obj, ox, oy) {
      super(world, obj, ox, oy, 8, 8)
      _activateTime = -1
      _thinkTime = 0
      _baseY = oy
      _delay = 3
      _activated = false

      props["spring"] = true

      _sound = Asset.create(Asset.Sound, "spring", "sound/spring.wav")
   }

   // springs dont activate immediately, they activate a few frames later
   framesUntilTrigger { _activateTime == -1 ? _delay : _delay - ((world.ticks - _activateTime) / _delay).floor }

   // they work like platforms, only collide from the top going down
   canCollide(other, side, d) {
      return side == Dir.Up && other.y+other.h <= y && other.y+other.h+d > y
   }

   // start the animation
   touch(other, side) {
      if (other.isPlayer == false) {
         return
      }

      if (_activateTime == -1) {
         _activateTime = world.ticks
      }
   }

   trigger() {
      y = _baseY
      dy = 0
      _activateTime = -1
      _activated = true
      Trap.sndPlay(_sound)
   }

   // if we're activated this frame, return the bounce amount
   // otherwise return 0
   checkSpring() {
      return _activated ? -3.4 : 0
   }

   think(dt) {
      if (_thinkTime == world.ticks) {
         return
      }
      _thinkTime = world.ticks

      if (_activateTime == -1) {
         _activated = false
         return
      }

      // set us up if we're set to activate. if there's a player on us, this think
      // will be run before the player calls checkSpring
      if (framesUntilTrigger <= 0) {
         trigger()
      } else {
         // since we work like a moving platform, move down and shrink
         // the player will stick to us
         dy = 2
         y = _baseY + (_delay - framesUntilTrigger) * 2
      }
   }

   draw(t) {
      var frm = _activateTime == -1 ? _delay : framesUntilTrigger
      drawSprite(263 - frm, x, y)      
   }
}