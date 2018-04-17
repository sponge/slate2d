import "entity" for Entity
import "timer" for Timer
import "main" for Main
import "engine" for Trap, Draw, Color, Fill, Asset
import "debug" for Debug

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
   construct new(world, dim, dir, delayed, ox, oy) {
      _dim = dim
      _flipped = false
      var w = _dim == DIM_HORIZ ? 16 : 6
      var h = _dim == DIM_HORIZ ? 6 : 16
      
      _delay = 0
      if (delayed) {
         _delay = 240
      }

      if (dir == "up" || dir == "left") {
         ox = ox + (_dim == DIM_HORIZ ? -16 : 1)
         oy = oy + (_dim == DIM_HORIZ ? 1 : -16)
      } else {
         ox = ox + (_dim == DIM_HORIZ ? 8 : 1)
         oy = oy + (_dim == DIM_HORIZ ? 1 : 8)
         _flipped = true
      }

      super(world, null, ox, oy, w, h)
   }  

   isHurting() {
      var cyc = (world.ticks + _delay) % 480
      return cyc > 300
   }

   isFiringUp() {
      var cyc = (world.ticks + _delay) % 480
      return cyc > 240 && cyc < 300
   }

   canCollide(other, side, d) { true }
   trigger { true }

   touch(other, side) {
      if (other.isPlayer == false) {
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

      // Draw.setColor(Color.Fill, 255, 0, 0, 255)
      // Draw.rect(x, y, w, h, Fill.Solid)

      var flicker = (t / 3 % 2).floor == 0

      if (_dim == DIM_VERT) {
         // FIXME: flip
         var f = flicker ? 1 : 0
         var flip = _flipped ? 2+f : 0+f
         Draw.sprite(world.spr, spr, x-1, y, 1, 1, flip, 1, 2)
      } else {
         var f = flicker ? 2 : 0
         var flip = _flipped ? 4+1+f : 4+f
         Draw.sprite(world.spr, spr, x, y-1, 1, 1, flip, 1, 2)
      }
   }
}

class Flamethrower is Entity {
   construct new(world, obj, ox, oy) {
      super(world, obj, ox, oy, 8, 8)

      var dir = obj["properties"]["direction"]
      if (dir == "up") {
         _tile = 7
         _dim = DIM_VERT
      } else if (dir == "right") {
         _tile = 8
         _dim = DIM_HORIZ
      } else if (dir == "down") {
         _tile = 9
         _dim = DIM_VERT
      } else if (dir == "left") {
         _tile = 10
         _dim = DIM_HORIZ
      } else {
         Trap.error(2, "Flamethrower at %(ox),%(oy) has no valid direction")
      }

      var altCycle = obj["properties"]["altCycle"] ? true : false

      var ent = Flame.new(world, _dim, dir, altCycle, ox, oy)
      world.entities.add(ent)
   }
   
   draw(t) {
      drawSprite(_tile, x, y)
   }
}

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
   }

   // springs dont activate immediately, they activate a few frames later
   framesUntilTrigger { _activateTime == -1 ? _delay : _delay - ((world.ticks - _activateTime) / _delay).floor }

   // they work like platforms, only collide from the top going down
   canCollide(other, side, d) {
      return side == DIR_TOP && other.y+other.h <= y && other.y+other.h+d > y
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
   }

   // if we're activated this frame, return the bounce amount
   // otherwise return 0
   checkSpring() {
      return _activated ? -3.6 : 0
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

class Cannon is Entity {
   construct new(world, obj, ox, oy) {
      super(world, obj, ox, oy, 8, 8)

      var dir = obj["properties"]["direction"]
      if (dir == "up") {
         _tile = 238
         _dim = DIM_VERT
         _d = -0.5
      } else if (dir == "right") {
         _tile = 239
         _dim = DIM_HORIZ
         _d = 0.5
      } else if (dir == "down") {
         _tile = 240
         _dim = DIM_VERT
         _d = 0.5
      } else if (dir == "left") {
         _tile = 241
         _dim = DIM_HORIZ
         _d = -0.5
      } else {
         Trap.error(2, "Cannon at %(ox),%(oy) has no valid direction")
      }

      _fireTime = world.ticks + 60
   }

   canCollide(other, side, d) { true }

   think(dt) {
      if (world.ticks < _fireTime) {
         return
      }

      // don't fire if we're too close or too far away
      var dist = (world.player.x - x).abs
      if (dist <= 16 || dist > 200) {
         // don't wait a full cycle to retry
         _fireTime = world.ticks + 60
         return
      }

      // spawn a cannonball, set parent to this so it doesn't immediately explode
      var ball = Cannonball.new(world, 270, x, y)
      ball.parent = this
      ball.dx = _dim == DIM_HORIZ ? _d : 0
      ball.dy = _dim == DIM_VERT ? _d : 0
      world.entities.add(ball)

      // recharge
      _fireTime = world.ticks + 300
   }

   draw(t) {
      drawSprite(_tile, x, y)      
   }
}

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

class FallingPlatform is Entity {
   platform { true }
   construct new(world, obj, ox, oy) {
      super(world, obj, ox, oy, 24, 4)

      _fallTime = 0
      _fallSpeed = 1
   }

   canCollide(other, side, d) {
      return side == DIR_TOP && other.y+other.h <= y && other.y+other.h+d > y
   }

   touch(other, side) {
      if (other.isPlayer == false) {
         return
      }

      // if a player touches us, wait a bit and then start falling
      if (_fallTime == 0) {
         _fallTime = other.world.ticks + 10
         dy = _fallSpeed
      }
   }

   think(dt) {
      if (_movedTime == world.ticks) {
         return
      }
      _movedTime = world.ticks

      // die if we've fallen off the level
      if ( y > world.level.maxY + world.level.th * 2) {
         active = false
         return
      }

      // keep moving down
      if (_fallTime > 0 && world.ticks > _fallTime) {
         y = y + dy
      }

   }

   draw(t) {
      drawSprite(244, x, y)
      drawSprite(244, x+8, y)
      drawSprite(244, x+16, y)
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
      if (_movedTime == world.ticks) {
         return
      }
      _movedTime = world.ticks

      // figure out if we need a new destination
      setNextPoint()

      // calculate our movement vector
      dx = (_dim == DIM_HORIZ ? _d : 0)
      dy = (_dim == DIM_VERT ? _d : 0)

      // this is only used to detect if we run into a player. we always move our speed every frame
      var chkx = check(DIM_HORIZ, dx)
      var chky = check(DIM_VERT, dy)

      // if the platform is going to lift the player up, attach them to this and lift them
      if (chky.entity.isPlayer && chky.entity.groundEnt != this && intersects(chky.entity) == false) {
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
      Draw.sprite(world.spr, 272, x, y, 0, 1, 0, 0, 3, 1)
   }
}

class Coin is Entity {
   construct new(world, obj, ox, oy) {
      super(world, obj, ox, oy, 8, 8)
      world.totalCoins = world.totalCoins + 1
      _collect = Asset.create(Asset.Sound, "coin_collect", "sound/coin.wav")
   }

   canCollide(other, side, d) { other.isPlayer == true }
   trigger { true }

   touch(other, side) {
      active = false
      world.coins = world.coins + 1
      Trap.sndPlay(_collect)
   }

   draw(t) {
      drawSprite(256 + (t / 8 % 4).floor, x, y)
   }
}

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
      other.disableControls = true
      world.entities.add(ExitBanner.new(world))
      world.playMusic("victory")
      Timer.runLater(300, Fn.new {
         world.changeScene("intro", _nextLevel)
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

   construct new(world, obj, ox, oy) {
      super(world, obj, ox, oy, 8, 8)
      dx = -0.5
      dy = 0
      _parent = null
   }

   canCollide(other, side, d) { true }
   trigger { true }

   touch(other, side) {
      active = false
      // don't hurt from the top
      if (other && other.isPlayer && side != DIR_TOP) {
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
      if ( y > world.level.maxY + world.level.th * 2 || y < 0) {
         touch(null, 0)
         return
      }

      if ( x > world.level.maxX || x < 0) {
         touch(null, 0)
         return
      }
   }

   draw(t) {
      drawSprite(270, x, y)      
   }
}

class StunShot is Cannonball {
   trigger { true }
   canCollide(other, side, d) { other != parent }

   construct new(player, world, obj, ox, oy) {
      super(world, obj, ox, oy, 6, 6)
      dx = 2
      dy = 0
      parent = player
      parent.shotsActive = parent.shotsActive + 1
      _totalDistance = 0
      _endTime = 0
   }

   touch(other, side) {
      if (_endTime == 0) {
         _endTime = world.ticks
         parent.shotsActive = parent.shotsActive - 1
      }
   }

   think(dt) {
      if (_endTime > 0 && world.ticks > _endTime) {
         if (world.ticks >= _endTime + 15) {
            active = false
         }
         return
      }

      super(dt)
      _totalDistance = _totalDistance + dx.abs
      if (_totalDistance > 100) {
         _endTime = world.ticks
         parent.shotsActive = parent.shotsActive - 1
      }
   }

   draw(t) {
      var anim = _endTime > 0 ? ((world.ticks - _endTime) / 5).floor + 1 : 0
      drawSprite(275 + anim, x, y)
   }
}