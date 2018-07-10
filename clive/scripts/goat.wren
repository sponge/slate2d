import "engine" for Draw, Asset, Trap, Color, Fill, Button, TileMap

class Goat {
  construct new(td, x, y) {
    _td = td
    _x = x
    _y = y
    _updateInterval = 0.2
    _nextUpdate = td.time + _updateInterval

    _sprite = Asset.create(Asset.Image, "goat", "gfx/game1/goat.png")
  }

  update(dt) {
    if (_td.time >= _nextUpdate) {
      var neighbours = [
        [_x + 1, _y],
        [_x - 1, _y],
        [_x, _y + 1],
        [_x, _y - 1]
      ]

      var closest = neighbours.reduce([0, 0, Num.largest]) {|acc, val|
        var dist = _td.getDistance(val[0], val[1])
        if (dist && dist < acc[2]) {
          return [val[0], val[1], dist]
        }
        return acc
      }

      _x = closest[0]
      _y = closest[1]

      _nextUpdate = _td.time + _updateInterval
    }
  }

  draw() {
    Draw.image(_sprite, _x * 8 - 8, _y * 8 - 8)
  }
}