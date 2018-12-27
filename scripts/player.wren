import "engine" for Draw, Asset, Trap, Button, Fill
import "math" for Math
import "entity" for Entity

class Player is Entity{
   construct new(world, obj, x, y, w, h) {
      super(world, obj, x, y, w, h)

      _mouth = Asset.create(Asset.Sprite, "mouth", "gfx/mouth.png")
      Asset.spriteSet(_mouth, 16, 16, 0, 0)
      Asset.loadAll()

      _t = 0
      _flapPressed = false
      _flip = 0

      _flapStrength = 0.4
      _maxFallSpeed = 0.75
      _maxFlightSpeed = 1.0
      _moveSpeed = 0.2
      _maxMoveSpeed = 1.0
      _gravity = 0.02
      _moveDecay = 0.003
   }

   think(dt) {
      _t = _t + dt

      var aPressed = Trap.keyPressed(Button.A)
      var lPressed = Trap.keyPressed(Button.Left)
      var rPressed = Trap.keyPressed(Button.Right)

      if (lPressed || rPressed) {
         _flip = lPressed ? 0 : 1
      }

      if (aPressed && !_flapPressed) {
         world.launched = true
         dy = dy - _flapStrength
         _flapPressed = true

         if (lPressed || rPressed) {
            dx = dx + (lPressed ? -_moveSpeed : _moveSpeed)
         }
      } else {
         if (world.launched) {
            dy = dy + _gravity
         }
      }

      if (dx > 0) {
         dx = dx - _moveDecay
      } else if (dx < 0) {
         dx = dx + _moveDecay
      }

      if (!aPressed) {
         _flapPressed = false
      }

      dx = Math.clamp(-_maxMoveSpeed, dx, _maxMoveSpeed)
      dy = Math.clamp(-_maxFlightSpeed, dy, _maxFallSpeed)

      x = Math.max(world.cam.x + 5, x + dx)
      y = y + dy

      if (y >= 185) {
         dy = 0
         die()
      }

      if (dx <= 0.003 && dx >= -0.003) {
         dx = 0
      }

      Trap.inspect(this, "player")
   }

   draw() {
      var spr = _flapPressed ? 1 : 0
      Draw.sprite(_mouth, spr, x, y, 1.0, 1.0, _flip, 1, 1)
      if (!world.launched) {
         Draw.rect(x, y+16, 16, 3, Fill.Solid)
      }
   }

   die() {
      super()
      world.nextScene = "title"
   }
}