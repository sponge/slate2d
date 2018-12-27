import "engine" for Draw, Asset, TileMap, Trap, Button
import "math" for Math

class Flight {
   nextScene { _nextScene }
   nextScene=(params) { _nextScene = params }

   construct new() {
      _mouth = Asset.create(Asset.Sprite, "mouth", "gfx/mouth.png")
      Asset.spriteSet(_mouth, 16, 16, 0, 0)
      Asset.loadAll()

      _t = 0
      _playerX = 160
      _playerY = 100
      _playerYAccel = 0
      _playerXAccel = 0
      _keyDown = false
      _playerFlip = 0
   }

   update(dt) {
      _t = _t + dt
      var aPressed = Trap.keyPressed(Button.A)
      var lPressed = Trap.keyPressed(Button.Left)
      var rPressed = Trap.keyPressed(Button.Right)

      if (_playerXAccel > 0) {
         _playerXAccel = _playerXAccel - 0.006
      } else if (_playerXAccel < 0) {
         _playerXAccel = _playerXAccel + 0.006
      }

      if (lPressed) {
         _playerFlip = 0
      } else if (rPressed) {
         _playerFlip = 1
      }

      if (aPressed && !_keyDown) {
         _playerYAccel = _playerYAccel - 0.3
         _keyDown = true

         if (lPressed) {
            _playerXAccel = _playerXAccel - 0.2
         } else if (rPressed) {
            _playerXAccel = _playerXAccel + 0.2
         }
      } else {
         _playerYAccel = _playerYAccel + 0.02
      }

      if (!aPressed) {
         _keyDown = false
      }

      _playerYAccel = Math.min(_playerYAccel, 1)
      _playerYAccel = Math.max(_playerYAccel, -1)

      _playerXAccel = Math.min(_playerXAccel, 1)
      _playerXAccel = Math.max(_playerXAccel, -1)

      _playerY = _playerY + _playerYAccel
      _playerX = _playerX + _playerXAccel
      _playerX = Math.max(_playerX, 10)
      _playerX = Math.min(_playerX, 290)

      Trap.printWin("flight", "y", _playerY)
      Trap.printWin("flight", "x", _playerX)
      Trap.printWin("flight", "xaccel", _playerXAccel)
      Trap.printWin("flight", "yaccel", _playerYAccel)
   }

   draw(w, h) {
      Draw.clear()
      Draw.resetTransform()
      Draw.scale(h / 180)

      var spr = 0
      if (_keyDown) {
         spr = 1
      }

      Draw.sprite(_mouth, spr, _playerX, _playerY, 1.0, 1.0, _playerFlip, 1, 1)
   }

   shutdown() {
      Asset.clearAll()
   }
}