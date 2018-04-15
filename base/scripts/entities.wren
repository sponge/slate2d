import "entity" for Entity
import "player" for Player
import "timer" for Timer
import "main" for Main

// FIXME: duped
var DIM_HORIZ = 1
var DIM_VERT = 2

var DIR_LEFT = 1
var DIR_RIGHT = 2
var DIR_TOP = 4
var DIR_BOTTOM = 8

// flames actually spawn out of a tile in the tilemap range. the flamethrower is solid
// but we spawn the flame and toggle it on and off instead to cut back on entities used
class Flame is Entity {
   construct new(world, ti, ox, oy) {
      _dim = ti % 2 == 0 ? DIM_HORIZ : DIM_VERT
      var w = _dim == DIM_HORIZ ? 16 : 6
      var h = _dim == DIM_HORIZ ? 6 : 16
      
      ti = ti - 7
      _delay = 0
      if (ti > 3) {
         ti = ti - 4
         _delay = 240
      }
      _tile = ti

      if (ti == 0 || ti == 3) {
         ox = ox + (_dim == DIM_HORIZ ? -16 : 1)
         oy = oy + (_dim == DIM_HORIZ ? 1 : -16)
      } else {
         ox = ox + (_dim == DIM_HORIZ ? 8 : 1)
         oy = oy + (_dim == DIM_HORIZ ? 1 : 8)
      }

      super(world, ti, ox, oy, w, h)
   }  

   isHurting() {
      var cyc = (world.time + _delay) % 480
      return cyc > 300
   }

   isFiringUp() {
      var cyc = (world.time + _delay) % 480
      return cyc > 240 && cyc < 300
   }

   canCollide(other, side, d) { true }
   trigger { true }

   touch(other, side) {
      if (other is Player == false) {
         return
      }

      if (isHurting() == false) {
         return
      }

      other.hurt(this, 1)
   }

   draw(t) {
      var spr = 263
      if (isHurting() == false) {
         if (isFiringUp()) {
            spr = 264
         } else {
            return
         }     
      }

      var flicker = (t / 3 % 2).floor == 0

      if (_dim == DIM_VERT) {
         var f = flicker ? 1 : 0
         var flip = _tile == 2 ? 2 + f : f
         // FIXME: draw
         // TIC.spr(spr, cx - 1, cy, 1, 1, flip, 0, 1, 2)
      } else {
         var f = flicker ? 2 : 0
         var flip = _tile == 3 ? 1 + f : f
         // FIXME: draw
         // TIC.spr(spr, cx, cy - 1, 1, 1, flip, 1, 1, 2)         
      }
   }
}

// springs work like moving platforms, and will be called from the player's think early on
// this would probably be cleaner if i could query to see if any entities are standing on the spring
// and trigger the bounce on them instead of having the player check.
class Spring is Entity {
   platform { true }

   construct new(world, ti, ox, oy) {
      super(world, ti, ox, oy, 8, 8)
      _activateTime = -1
      _thinkTime = 0
      _baseY = oy
      _delay = 3
      _activated = false
   }

   // springs dont activate immediately, they activate a few frames later
   framesUntilTrigger { _activateTime == -1 ? _delay : _delay - ((world.time - _activateTime) / _delay).floor }

   // they work like platforms, only collide from the top going down
   canCollide(other, side, d) {
      return side == DIR_TOP && other.y+other.h <= y && other.y+other.h+d > y
   }

   // start the animation
   touch(other, side) {
      if (other is Player == false) {
         return
      }

      if (_activateTime == -1) {
         _activateTime = world.time
      }
   }

   trigger() {
      y = _baseY
      dy = 0
      _activateTime = -1
      _activated = true
   }

   // if we're activated this frame, return the bounce amount
   // otherwise return 0
   checkSpring() {
      return _activated ? -3.6 : 0
   }

   think(dt) {
      if (_thinkTime == world.time) {
         return
      }
      _thinkTime = world.time

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
      // FIXME: draw
      // TIC.spr(263 - frm, cx, cy, 0)      
   }
}

class Cannon is Entity {
   construct new(world, ti, ox, oy) {
      super(world, ti, ox, oy, 8, 8)
      _tile = ti

      ti = ti - 238
      _dim = ti % 2 == 0 ? DIM_VERT : DIM_HORIZ
      _d = ti == 0 || ti == 3 ? -0.5 : 0.5

      _fireTime = world.time + 60
   }

   canCollide(other, side, d) { true }

   think(dt) {
      if (world.time < _fireTime) {
         return
      }

      // don't fire if we're too close or too far away
      var dist = (world.player.x - x).abs
      if (dist <= 16 || dist > 200) {
         // don't wait a full cycle to retry
         _fireTime = world.time + 60
         return
      }

      // spawn a cannonball, set parent to this so it doesn't immediately explode
      var ball = Cannonball.new(world, 270, x, y)
      ball.parent = this
      ball.dx = _dim == DIM_HORIZ ? _d : 0
      ball.dy = _dim == DIM_VERT ? _d : 0
      world.entities.add(ball)

      // recharge
      _fireTime = world.time + 300
   }

   draw(t) {
      // FIXME: draw
      // TIC.spr(_tile, cx, cy, 13)      
   }
}

// spikes just hurt players when touched
class Spike is Entity {
   construct new(world, ti, ox, oy) {
      super(world, ti, ox, oy, 8, 8)
   }

   canCollide(other, side, d) { true }

   touch(other, side) {
      if (other is Player == false) {
         return
      }

      other.hurt(this, 1)
   }

   draw(t) {
      // FIXME: draw
      // TIC.spr(242, cx, cy, 0)      
   }
}

class FallingPlatform is Entity {
   platform { true }
   construct new(world, ti, ox, oy) {
      super(world, ti, ox, oy, 24, 4)

      _fallTime = 0
      _fallSpeed = 1
   }

   canCollide(other, side, d) {
      return side == DIR_TOP && other.y+other.h <= y && other.y+other.h+d > y
   }

   touch(other, side) {
      if (other is Player == false) {
         return
      }

      // if a player touches us, wait a bit and then start falling
      if (_fallTime == 0) {
         _fallTime = other.world.time + 10
         dy = _fallSpeed
      }
   }

   think(dt) {
      if (_movedTime == world.time) {
         return
      }
      _movedTime = world.time

      // die if we've fallen off the level
      if ( y > (world.level.y + world.level.h + 2) * 8) {
         active = false
         return
      }

      // keep moving down
      if (_fallTime > 0 && world.time > _fallTime) {
         y = y + dy
      }

   }

   draw(t) {
      // FIXME: draw
      // TIC.spr(244, cx, cy, 0)
      // TIC.spr(244, cx+8, cy, 0)
      // TIC.spr(244, cx+16, cy, 0)
   }
}

class MovingPlatform is Entity {
   resolve { _resolve }
   platform { true }
    
   construct new(world, ti, ox, oy) {
      super(world, ti, ox, oy, 24, 0)
      _dist = 0 // how far before we reach our target
      _d = 0 // speed
      _dim = 0 // what axis we're moving on

      // passed into tilecollider to find the next turn 
      _targetResolve = Fn.new { |side, tile, tx, ty, ldx, ldy|
         // don't collide with our current turn tile
         if (tx == _ignoreX && ty == _ignoreY) {
            return false
         }

         if (tile >= 245 && tile <= 252) {
            return true
         }

         return false
      }

      // never collide with any tile during movement
      _resolve = Fn.new { |side, tile, tx, ty, ldx, ldy| false }

      // set initial direction based on spawn and then the first destination
      setDirection(ti)
      setNextPoint()
   }

   // moving platforms work like one way. only collide if you're falling through from the top
   canCollide(other, side, d) {
      //Debug.text("ret", "%(side) == %(DIR_TOP) && %(other.y)+%(other.h) <= %(y) && %(other.y)+%(other.h)+%(other.dy) > %(y)")
      return side == DIR_TOP && other.y+other.h <= y && other.y+other.h+d > y
   }

   setDirection(ti) {
      ti = ti - 245
      // spawns also act as turns.
      if (ti > 3) {
         ti = ti - 4
      }

      _dim = ti % 2 == 0 ? DIM_VERT : DIM_HORIZ
      _d = ti == 0 || ti == 3 ? -0.5 : 0.5
   }

   setNextPoint() {
      // FIXME: tic call
      /*
      // if we've still got time to go, or we couldn't find a valid target
      if (_d == 0 || _dim == 0 || _dist > 0) {
         return
      }

      // we need a new destination. don't consider the one we're on top of now
      _ignoreX = x / 8
      _ignoreY = y / 8

      // but we do need to know which way we're about to go!
      var t = TIC.mget(_ignoreX, _ignoreY)
      setDirection(t)

      // go a long way out to find how far our next target is
      _dist = world.tileCollider.query(x, y, 1, 1, _dim, _d*2048, _targetResolve)

      // if we don't have a destination, just freeze it in place
      if (_dist.abs == _d*2048) {
         _dim = 0
         _d = 0
         return
      }

      // offset for size of platform
      _dist = _dist.abs + (_d > 0 ? 1 : 8)
      */
   }

   think(dt) {
      // don't move twice in a frame in case a player called us
      if (_movedTime == world.time) {
         return
      }
      _movedTime = world.time

      // figure out if we need a new destination
      setNextPoint()

      // calculate our movement vector
      dx = (_dim == DIM_HORIZ ? _d : 0)
      dy = (_dim == DIM_VERT ? _d : 0)

      // this is only used to detect if we run into a player. we always move our speed every frame
      var chkx = check(DIM_HORIZ, dx)
      var chky = check(DIM_VERT, dy)

      // if the platform is going to lift the player up, attach them to this and lift them
      if (chky.entity is Player && chky.entity.groundEnt != this && intersects(chky.entity) == false) {
         chky.entity.groundEnt = this
         chky.entity.y = chky.entity.y + chky.entity.check(DIM_VERT, dy).delta
         // Debug.text("attach")
      }

      x = x + dx
      y = y + dy

      // subtract our distance remaining
      _dist = _dist - _d.abs

      // Debug.text("p", "%(x),%(y) %(_dist)s")
   }

   draw(t) {
      // FIXME: draw
      // TIC.spr(272, cx, cy, 0, 1, 0, 0, 3, 1)
   }
}

class Coin is Entity {
   construct new(world, ti, ox, oy) {
      super(world, ti, ox, oy, 8, 8)
      world.totalCoins = world.totalCoins + 1
   }

   canCollide(other, side, d) { other is Player == true }
   trigger { true }

   touch(other, side) {
      active = false
      world.coins = world.coins + 1
   }

   draw(t) {
      // FIXME: draw
      // TIC.spr(256 + (t / 8 % 4).floor, cx, cy, 0)
   }
}

class LevelExit is Entity {
   construct new(world, ti, ox, oy) {
      super(world, ti, ox, oy, 8, 8)
   }

   canCollide(other, side, d) { other is Player == true }
   trigger { true }

   draw(t) {
      // FIXME: draw
      // TIC.spr(268 + (t / 7 % 2).floor, cx, cy, 0)
   }

   touch(other, side) {
      active = false
      other.disableControls = true
      world.entities.add(ExitBanner.new(world))
      world.drawHud = false
      Timer.runLater(360, Fn.new {
         Main.intro(world.levelNum + 1)
      })
   }
}

class ExitBanner is Entity {
   construct new(world) {
      super(world, 0, 0, 0, 0, 0)
   }

   draw(t) {
      // FIXME: draw
      // TIC.rect(0, 40, 240, 56, 5)
      // TIC.print("Level Cleared", 45, 45, 15, false, 2)
      // TIC.print("Now, lets move on to the next one!", 27, 60)
      // if (world.totalCoins > 0) {
      //    var pct = (world.coins / world.totalCoins *100).floor
      //    TIC.print("Coins ........ %(pct)\%", 60, 75, 15, true)
      // }
   }
}

class Cannonball is Entity {
   parent { _parent }
   parent=(ent) { _parent = ent }

   construct new(world, ti, ox, oy) {
      super(world, ti, ox, oy, 8, 8)
      dx = -0.5
      dy = 0
      _parent = null
   }

   canCollide(other, side, d) { true }
   trigger { true }

   touch(other, side) {
      active = false
      // don't hurt from the top
      if (other is Player && side != DIR_TOP) {
         other.hurt(this, 1)
      }
   }

   // move our speed every frame. if we hit something, activate touch on ourselves
   // and explode
   think(dt) {
      var chkx = check(DIM_HORIZ, dx)
      if (chkx.entity != null && chkx.entity != _parent) {
         touch(chkx.entity, chkx.side)
         return
      }

      x = x + chkx.delta
      if (chkx.delta != dx) {
         touch(null, chkx.side)
         return
      }
      
      var chky = check(DIM_VERT, dy)
      if (chky.entity != null && chky.entity != _parent) {
         touch(chky.entity, chky.side)
         return
      }

      y = y + chky.delta
      if (chky.delta != dy) {
         touch(null, chky.side)
         return
      }
      
      // die if we go off the level
      if ( y > (world.level.y + world.level.h + 2) * 8 || y < world.level.y) {
         touch(null, 0)
         return
      }

      if ( x > (world.level.x + world.level.w) * 8 || x < world.level.x) {
         touch(null, 0)
         return
      }
   }

   draw(t) {
      // FIXME: draw
      // TIC.spr(270, cx, cy, 13)      
   }
}

class StunShot is Cannonball {
   trigger { true }
   canCollide(other, side, d) { other != parent }

   construct new(player, world, ti, ox, oy) {
      super(world, ti, ox, oy, 6, 6)
      dx = 2
      dy = 0
      parent = player
      parent.shotsActive = parent.shotsActive + 1
      _totalDistance = 0
      _endTime = 0
   }

   touch(other, side) {
      if (_endTime == 0) {
         _endTime = world.time
         parent.shotsActive = parent.shotsActive - 1
      }
   }

   think(dt) {
      if (_endTime > 0 && world.time > _endTime) {
         if (world.time >= _endTime + 15) {
            active = false
         }
         return
      }

      super(dt)
      _totalDistance = _totalDistance + dx.abs
      if (_totalDistance > 100) {
         _endTime = world.time
         parent.shotsActive = parent.shotsActive - 1
      }
   }

   draw(t) {
      var anim = _endTime > 0 ? ((world.time - _endTime) / 5).floor + 1 : 0
      // FIXME: draw
      // TIC.spr(275 + anim, cx, cy, 0)      
   }
}