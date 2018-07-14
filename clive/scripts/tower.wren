import "engine" for Draw, Asset, Trap, Color, Fill, Button, TileMap
import "entities/projectile" for Cannonball
import "math" for Math

class Tower {
   static Slow { 0 }
   static Fast { 1 }
   static Magic { 2 }

   x { _x }
   y { _y }

   construct new(td, x, y, type) {
      _x = x
      _y = y
      _type = type
      _td = td
      _range = 5
      _closest = null
      _nextShot = td.time
   }

   update(dt, creeps) {
      var closest = {}

      for (creep in creeps) {
         var dist = ((_x - creep.x).pow(2) + (_y - creep.y).pow(2)).sqrt
         if (closest["dist"] == null || dist < closest["dist"]) {
            closest = { "dist": dist, "creep": creep }
         }
      }

      if (closest["creep"] != null && closest["dist"] < _range) {
         _closest = closest["creep"]
      } else {
         _closest = null
      }

      if (_closest && _td.time > _nextShot) {
         _nextShot = _td.time + 3
         fireAt(_closest)
      }
   }

   fireAt(creep) {
      var proj = Cannonball.new(_td, _x, _y, creep)
      _td.grid.entities.add(proj)
   }

   draw() {
      Draw.sprite(_td.spr, _type * 2, _x * 8, _y * 8, 255, 1, 0, 2, 2)
   }
}