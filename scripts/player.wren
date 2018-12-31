import "engine" for Draw, Asset, Trap, Button, Fill
import "math" for Math
import "timer" for Timer
import "entity" for Entity

class Player is Entity {
   launched { _launched }

   construct new(world, obj, x, y) {
      super(world, obj, x, y, 10, 10)

      _world = world

      _mouth = Asset.create(Asset.Sprite, "mouth", "gfx/mouth.png")
      Asset.spriteSet(_mouth, 16, 16, 0, 0)

      _t = 0
      _flip = 0
      _health = 100
      _invuln = false
      _launched = false
      _flap = false

      _flapStrength = 0.4
      _maxFallSpeed = 0.75
      _maxFlightSpeed = 1.0
      _moveSpeed = 0.2
      _maxMoveSpeed = 1.0
      _gravity = 0.02
      _moveDecay = 0.003
      _invulnTime = 120
      _regenRate = 80
      _collectibleHeal = 14
   }

   think(dt) {
      _t = _t + dt

      _flap = Trap.keyPressed(Button.A) || Trap.keyPressed(Button.B)
      var lPressed = Trap.keyPressed(Button.Left)
      var rPressed = Trap.keyPressed(Button.Right)

      // draw them in the right direction
      if (lPressed || rPressed) {
         _flip = lPressed ? 0 : 1
      }

      // if the flap key is down for the first time
      if (_flap && !_flapHeld) {
         // detach the player from the starting platform if necessary
         _launched = true
         // give the player a bump
         dy = dy - _flapStrength
         _flapHeld = true

         // only move the player when flapping
         if (lPressed || rPressed) {
            dx = dx + (lPressed ? -_moveSpeed : _moveSpeed)
         }
      } else {
         // no flap this frame, apply gravity if not on the platform
         if (_launched) {
            dy = dy + _gravity
         }

         if (!_flap) {
            _flapHeld = false
         }
      }

      // apply air friction
      dx = dx + _moveDecay * Math.sign(-dx)

      // cap speed
      dx = Math.clamp(-_maxMoveSpeed, dx, _maxMoveSpeed)
      dy = Math.clamp(-_maxFlightSpeed, dy, _maxFallSpeed)

      // do the move
      x = Math.max(_world.cam.x + 5, x + dx)
      var yMin = _world.canWin ? -32 : 5
      y = Math.max(yMin, y + dy)

      // if they've fallen off the screen, game over
      if (y >= _world.cam.h + 5) {
         die()
         return
      }

      // round small movements to 0
      if (dx <= 0.003 && dx >= -0.003) {
         dx = 0
      }

      // perform collision detection. we only care about intersections happening, nothing will block you
      _world.entities.each {|ent|
         if (ent == this || ent.dead || _launched == false) {
            return
         }

         if (Math.rectIntersect(ent.x, ent.y, ent.w, ent.h, x, y, w, h) == false) {
            return
         }

         // we have a collision

         // if it was a mine, hurt you
         if (ent.name == "mine" && !_invuln) {
            ent.die(true)
            _health = _health - 36

            if (_health <= 0) {
               die()
               return
            }

            _invuln = true
            Timer.runLater(_invulnTime) {
               _invuln = false
            }
         // if it was a collectible, heal you
         } else if (ent.name == "collectible") {
            ent.die(true)
            _health = Math.clamp(0, _health + _collectibleHeal, 100)
         }
      }

      // regen 1 hp every _regenRate ticks
      if (_t % _regenRate == 0) {
         _health = Math.min(100, _health + 1)
      }
      _world.meter.set(_health)
   }

   draw() {
      var spr = _flap ? 1 : 0
      Draw.sprite(_mouth, spr, x - 3, y - 3, _invuln ? 0.4 : 1.0, 1.0, _flip, 1, 1)
      if (!_launched) {
         Draw.rect(x-3, y+12, 16, 3, Fill.Solid)
      }

      Trap.inspect(this, "player")
   }

   die() {
      super()
      world.onGameOver()
   }
}