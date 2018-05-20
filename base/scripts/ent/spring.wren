import "entity" for Entity
import "engine" for Asset, Trap
import "collision" for Dir
import "debug" for Debug

// springs work like moving platforms, and will be called from the player's think early on
// this would probably be cleaner if i could query to see if any entities are standing on the spring
// and trigger the bounce on them instead of having the player check.
class Spring is Entity {
   platform { true }

   construct new(world, obj, ox, oy) {
      super(world, obj, ox, oy+1, 8, 7)
      _activateTime = null
      _thinkTime = 0
      _baseY = y
      _delay = 3
      _activated = false

      props["spring"] = true

      _sound = Asset.create(Asset.Sound, "spring", "sound/spring.wav")
   }

   // springs dont activate immediately, they activate a few frames later
   framesUntilTrigger { _activateTime == null ? _delay : _delay - ((world.ticks - _activateTime) / _delay).floor }

   // they work like platforms, only collide from the top going down
   canCollide(other, side, d) {
      return side == Dir.Up && other.y+other.h <= y && other.y+other.h+d > y
   }

   // start the animation
   touch(other, side) {
      if (_activateTime == null) {
         _activateTime = world.ticks
      }
   }

   // if we're activated this frame, return the bounce amount
   // otherwise return 0
   checkSpring() {
      return _activated ? -3.4 : 0
   }

   think(dt) {
      if (_activateTime == null) {
         dy = 0
         _activated = false
         return
      }

      // set us up if we're set to activate. if there's a player on us, this think
      // will be run before the player calls checkSpring
      if (framesUntilTrigger <= 0) {
         y = _baseY
         dy = -2 // shift entities up before springing them
         _activateTime = null
         _activated = true
         Trap.sndPlay(_sound)
      } else {
         // since we work like a moving platform, move down and shrink
         // the player will stick to us
         dy = 2
         y = _baseY + (_delay - framesUntilTrigger) * 2
      }
   }

   draw(t) {
      var frm = _activateTime == null ? _delay : framesUntilTrigger
      drawSprite(263 - frm, x, y)      
   }
}