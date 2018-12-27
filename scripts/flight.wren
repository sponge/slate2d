import "engine" for Draw, Asset, TileMap, Trap, Button
import "math" for Math

class Flight {
   nextScene { _nextScene }
   nextScene=(params) { _nextScene = params }

   construct new() {
      _dog = Asset.create(Asset.Sprite, "dog", "gfx/dog.png")
      Asset.spriteSet(_dog, 22, 16, 0, 0)
      Asset.loadAll()

      _t = 0
      _playerX = 160
      _playerY = 100
      _playerYAccel = 0
      _playerXAccel = 0
      _keyDown = false
   }

   update(dt) {
      _t = _t + dt

      if (_playerXAccel > 0) {
         _playerXAccel = _playerXAccel - 0.006
      } else if (_playerXAccel < 0) {
         _playerXAccel = _playerXAccel + 0.006
      }

      var aPressed = Trap.keyPressed(Button.A, 0, 0)
      if (aPressed && !_keyDown) {
         _playerYAccel = _playerYAccel - 0.3
         _keyDown = true

         if (Trap.keyPressed(Button.Left)) {
            _playerXAccel = _playerXAccel - 0.2
         } else if (Trap.keyPressed(Button.Right)) {
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

      Trap.printWin("flight", "y", _playerY)
      Trap.printWin("flight", "xaccel", _playerXAccel)
      Trap.printWin("flight", "yaccel", _playerYAccel)
   }

   draw(w, h) {
      Draw.clear()
      Draw.resetTransform()
      Draw.scale(h / 180)

      var spr = 0
      if (_keyDown) {
         spr = 4
      }
      Draw.sprite(_dog, spr, _playerX, _playerY, 1.0, 1.0, 1, 1, 1)
   }

   shutdown() {
      Asset.clearAll()
   }
}