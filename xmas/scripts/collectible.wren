import "engine" for Draw, Asset, Trap, Button, Fill
import "math" for Math
import "entity" for Entity
import "random" for Random

class Collectible is Entity {
  construct new (world, obj, x, y) {
    super(world, obj, x, y, 10, 10)
    name = "collectible"

    _baseX = x
    _baseY = y

    _spr = Asset.find("collectible")

    _t = 0
  }

  think(dt) {
    _t = _t + dt
    x = _baseX + (_t / 20).sin * 6
    y = _baseY + (_t / 20).cos * 6
  }

  draw() {
    Draw.sprite(_spr, 0, x-2, y-2)
  }

  die(hitByPlayer) {
    super()
    if (hitByPlayer) {
      world.onCollectibleHit(this)
    }
  }
}