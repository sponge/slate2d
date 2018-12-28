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

      _flapStrength = 0.4
      _maxFallSpeed = 0.75
      _maxFlightSpeed = 1.0
      _moveSpeed = 0.2
      _maxMoveSpeed = 1.0
      _gravity = 0.02
      _moveDecay = 0.003
      _invulnTime = 180
      _regenRate = 50
   }

   think(dt) {
      _t = _t + dt

      var aPressed = Trap.keyPressed(Button.A)
      var lPressed = Trap.keyPressed(Button.Left)
      var rPressed = Trap.keyPressed(Button.Right)

      // draw them in the right direction
      if (lPressed || rPressed) {
         _flip = lPressed ? 0 : 1
      }

      // if the flap key is down for the first time
      if (aPressed && !_flapPressed) {
         // detach the player from the starting platform if necessary
         _launched = true
         // give the player a bump
         dy = dy - _flapStrength
         _flapPressed = true

         // only move the player when flapping
         if (lPressed || rPressed) {
            dx = dx + (lPressed ? -_moveSpeed : _moveSpeed)
         }
      } else {
         // no flap this frame, apply gravity if not on the platform
         if (_launched) {
            dy = dy + _gravity
         }

         if (!aPressed) {
            _flapPressed = false
         }
      }

      // apply air friction
      dx = dx + _moveDecay * Math.sign(-dx)

      // cap speed
      dx = Math.clamp(-_maxMoveSpeed, dx, _maxMoveSpeed)
      dy = Math.clamp(-_maxFlightSpeed, dy, _maxFallSpeed)

      // do the move
      x = Math.max(_world.cam.x + 5, x + dx)
      y = Math.max(5, y + dy)

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
         if (ent == this || ent.dead || _invuln == true) {
            return
         }

         if (Math.rectIntersect(ent.x, ent.y, ent.w, ent.h, x, y, w, h) == false) {
            return
         }

         // if we've collided, destroy the ent we collided with and reduce health by 1
         _health = _health - 36
         ent.die(true)

         if (_health <= 0) {
            die()
            return
         }

         _invuln = true
         Timer.runLater(_invulnTime) {
            _invuln = false
         } 
      }

      // regen 1 hp every _regenRate ticks
      if (_t % _regenRate == 0) {
         _health = Math.min(100, _health + 1)
      }
      _world.meter.set(_health)
   }

   draw() {
      var spr = Trap.keyPressed(Button.A) ? 1 : 0
      Draw.sprite(_mouth, spr, x - 3, y - 3, _invuln ? 0.4 : 1.0, 1.0, _flip, 1, 1)
      if (!_launched) {
         Draw.rect(x-3, y+12, 16, 3, Fill.Solid)
      }

      Trap.inspect(this, "player")
   }

   die() {
      super()
      world.nextScene = "gameover"
   }
}