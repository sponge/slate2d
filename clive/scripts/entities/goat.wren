import "engine" for Draw, Asset, Trap, Color, Fill, Button, TileMap
import "entities/entity" for Entity
import "debug" for Debug
import "math" for Math
import "soundcontroller" for SoundController

class Goat is Entity {
   frozen { _frozen }

   construct new(td, x, y) {
      super(x, y, "goat")
      _dx = 0
      _dy = 0
      _td = td
      _grid = _td.grid
      _moveInterval = 0.6
      _destroyInterval = 2
      _nextUpdate = _td.time + _moveInterval
      _startingHP = 15
      _hp = _startingHP
      _mode = "move"
      _frozen = false

      _sprite = Asset.find("goat")
      _eatingSprite = Asset.find("goat_eating")
      _dieSound = Asset.find("goat_die")

      _imgSize = Asset.imageSize(_sprite)
   }

   hurt(damage) {
      if (_hp <= 0) {
         return
      }
      
      _hp = _hp - damage
      if (_hp <= 0 ) {
         SoundController.playOnce(_dieSound)
         die()
      }
   }

   die() {
      _td.onEntityDied(this)
      super()
   }

   freeze(time) {
      _nextUpdate = _nextUpdate + time
      _frozen = true
   }

   update(dt) {
      if (_td.time >= _nextUpdate) {
         _frozen = false
         if (_mode == "destroy") {
            moveDestroy()
         } else if (_mode == "move") {
            moveStep()
         } else if (_mode == "grass") {
            moveGrass()
         }
      }

      // seems to happen if you cram towers near the border
      if (x < 1) {
         x = 1
      }
   }

   moveGrass() {
      _grid.destroyGrass(x+_dx, y+_dy)
      x = x + _dx
      y = y + _dy
      _nextUpdate = _td.time + _moveInterval
      _mode = "move"
   }

   moveDestroy() {
      _grid.destroyWall(x+_dx,y+_dy)
      x = x + _dx
      y = y + _dy
      _nextUpdate = _td.time + _moveInterval
      _mode = "move"
   }

   moveToGrass() {
      var neighbours = [
         [ 1,  0],
         [-1,  0],
         [ 0,  1],
         [ 0, -1]
      ]

      for (offset in 1..3) {
         for (n in neighbours) {
            if (n[0] != 0 || n[1] != 0) {
               var tileX = x + n[0] * offset
               var tileY = y + n[1] * offset
               if (_grid.isBlocked(tileX, tileY, true)) {
                  // found something in the way, kill this direction
                  n[0] = n[1] = 0
               } else if (_grid.isGrass(tileX, tileY)) {
                  // found grass, move towards it and return
                  _dx = n[0]
                  _dy = n[1]
                  return true
               }
            }
         }
      }

      return false
   }

   moveStep() {
      var neighbours = [
         [x + 1, y],
         [x - 1, y],
         [x, y + 1],
         [x, y - 1]
      ]

      var ogX = x
      var ogY = y

      var closest = neighbours.reduce([0, 0, Num.largest]) {|acc, val|
         var dist = _grid.getDistance(val[0], val[1])
         if (dist && dist < acc[2]) {
            return [val[0], val[1], dist]
         }
         return acc
      }

      var movedToGrass = moveToGrass()

      // if there's no path, just run straight toward the coin and destroy
      // walls until you make one
      if (!movedToGrass && closest[0] == 0 && closest[1] == 0) {
         // move on the longer axis toward goal
         var xShorter = ((_grid.goalX-x).abs < (_grid.goalY-y).abs)
         _dx = xShorter ? 0 : Math.sign(_grid.goalX-x)
         _dy = xShorter ? Math.sign(_grid.goalY-y) : 0

         // FIXME: if wall is in the way
         if (_grid.isWall(x+_dx, y+_dy)) {
            _nextUpdate = _td.time + _destroyInterval
            _mode = "destroy"
         } else if (!_grid.isBlocked(x + _dx, y + _dy)) {
            x = x + _dx
            y = y + _dy
         }
      } else {
         // the goat has a path to the coin. check for grass or move normally
         if (_grid.isGrass(x+_dx, y+_dy)) {
            _nextUpdate = _td.time + _destroyInterval
            _mode = "grass"
         } else if (movedToGrass && !_grid.isBlocked(x + _dx, y + _dy)) {
            x = x + _dx
            y = y + _dy
         } else {
            x = closest[0]
            y = closest[1]
         }
      }

      if (x == _grid.goalX && y == _grid.goalY) {
         _td.onTouchCoin()
         die()
      }

      if (_mode == "move") {
         _nextUpdate = _td.time + _moveInterval
      }
   }

  draw() {
    var sprX = (x - 1) * _td.tw
    var sprY = (y - 1) * _td.th
    var sprite = (_mode == "grass" || _mode == "destroy") ? _eatingSprite : _sprite
    Draw.image(sprite, sprX, sprY, _imgSize[0], _imgSize[1], 1.0)
    Draw.setColor(Color.Fill, 0, 255, 0, 255)
    Draw.rect(sprX, sprY - 2, _td.tw * 2 * (_hp / _startingHP), _td.th / 4, false)
  }
}