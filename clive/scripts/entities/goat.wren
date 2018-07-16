import "engine" for Draw, Asset, Trap, Color, Fill, Button, TileMap
import "entities/entity" for Entity

class Goat is Entity {
  construct new(td, grid, x, y) {
    super(x, y, "goat")
    _td = td
    _grid = grid
    _updateInterval = 0.2
    _nextUpdate = _td.time + _updateInterval
    _hp = 10

    _sprite = Asset.create(Asset.Image, "goat", "gfx/game1/goat.png")
  }

  hurt(damage) {
    _hp = _hp - damage
    if (_hp <= 0 ) {
      die()
    }
  }

  update(dt) {
    if (_td.time >= _nextUpdate) {
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

      x = closest[0]
      y = closest[1]

      if (x == _grid.goalX && y == _grid.goalY) {
        _td.gameOver()
      }

      _nextUpdate = _td.time + _updateInterval
    }
  }

  draw() {
    var sprX = (x - 1) * _td.tw
    var sprY = (y - 1) * _td.th
    Draw.image(_sprite, sprX, sprY)
    Draw.setColor(Color.Fill, 0, 255, 0, 255)
    Draw.rect(sprX, sprY - 2, _td.tw * 2 * (_hp / 10), 2, false)
  }
}