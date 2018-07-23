import "engine" for Draw, Asset, Trap, Color, Fill, Button, TileMap
import "entities/entity" for Entity
import "debug" for Debug
import "math" for Math

class Goat is Entity {
   construct new(td, x, y) {
      super(x, y, "goat")
      _td = td
      _grid = _td.grid
      _moveInterval = 0.2
      _destroyInterval = 3
      _nextUpdate = _td.time + _moveInterval
      _hp = 10
      _mode = "move"

      _sprite = Asset.find("goat")
   }

   hurt(damage) {
      _hp = _hp - damage
      if (_hp <= 0 ) {
         die()
      }
   }

   die() {
      _td.onEntityDied(this)
      super()
   }

   update(dt) {
      if (_td.time >= _nextUpdate) {
         if (_mode == "destroy") {
            moveDestroy()
         } else if (_mode == "move") {
            moveStep()
         }
      }
   }

   moveDestroy() {
      // FIXME: destroy wall
      _nextUpdate = _td.time + _moveInterval
      _mode = "move"
   }

   moveStep() {
      var neighbours = [
         [x + 1, y],
         [x - 1, y],
         [x, y + 1],
         [x, y - 1]
      ]

      var closest = neighbours.reduce([0, 0, Num.largest]) {|acc, val|
         var dist = _grid.getDistance(val[0], val[1])
         if (dist && dist < acc[2]) {
            return [val[0], val[1], dist]
         }
         return acc
      }

      // if there's no path, just run straight toward the coin and destroy
      // walls until you make one
      if (closest[0] == 0 && closest[1] == 0) {
         // move on the longer axis toward goal
         var xShorter = ((_grid.goalX-x).abs < (_grid.goalY-y).abs)
         var dx = xShorter ? 0 : Math.sign(_grid.goalX-x)
         var dy = xShorter ? Math.sign(_grid.goalY-y) : 0

         // FIXME: if wall is in the way
         if (false) {
            _nextUpdate = _td.time + _destroyInterval
            _mode = "destroy"
         } else {
            x = x + dx
            y = y + dy
         }
      } else {
         x = closest[0]
         y = closest[1]
      }

      if (x == _grid.goalX && y == _grid.goalY) {
         _td.onTouchCoin()
         die()
      }

      _nextUpdate = _td.time + _moveInterval
      _mode = "move"
   }

  draw() {
    var sprX = (x - 1) * _td.tw
    var sprY = (y - 1) * _td.th
    Draw.image(_sprite, sprX, sprY)
    Draw.setColor(Color.Fill, 0, 255, 0, 255)
    Draw.rect(sprX, sprY - 2, _td.tw * 2 * (_hp / 10), 2, false)
  }
}