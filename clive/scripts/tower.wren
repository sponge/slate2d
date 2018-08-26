import "engine" for Draw, Asset, Trap, Fill, Button, TileMap
import "entities/projectile" for Cannonball, Arrow, MagicBolt
import "math" for Math
import "soundcontroller" for SoundController
import "debug" for Debug

class Tower {
   static Fast { 0 }
   static Slow { 1 }
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
      _fireRate = type == Tower.Fast ? 0.5 : type == Tower.Slow ? 2 : 0.5

      _slowShoot = Asset.find("cannon_shoot")
      _fastShoot = Asset.find("arrow_shoot")
      _magicShoot = Asset.find("magic_shoot")
   }


   update(dt, creeps) {
      if (_td.time < _nextShot) {
         return
      }

      var closest = {}

      for (creep in creeps) {
         if (_type != Tower.Magic || creep.frozen == false) {
            var dist = ((_x - creep.x).pow(2) + (_y - creep.y).pow(2)).sqrt
            if (closest["dist"] == null || dist < closest["dist"]) {
               closest = { "dist": dist, "creep": creep }
            }
         }
      }

      if (closest["creep"] != null && closest["dist"] < _range) {
         _closest = closest["creep"]
      } else {
         _closest = null
      }

      if (_closest) {
         _nextShot = _td.time + _fireRate
         fireAt(_closest)
      }
   }

   fireAt(creep) {
      if (_type == Tower.Slow) {
         var proj = Cannonball.new(_td, _x, _y, creep)
         _td.grid.entities.add(proj)
         SoundController.playOnce(_slowShoot)
      } else if (_type == Tower.Fast) {
         var proj = Arrow.new(_td, _x, _y, creep)
         _td.grid.entities.add(proj)
         SoundController.playOnce(_fastShoot)
      } else if (_type == Tower.Magic) {
         var proj = MagicBolt.new(_td, _x, _y, creep)
         _td.grid.entities.add(proj)
         SoundController.playOnce(_magicShoot)
      }
   }

   draw() {
      var tid = _type == Tower.Magic ? 37 : _type * 2
      if (_type == Tower.Magic) {
         Draw.sprite(_td.spr, tid, _x * _td.tw, _y * _td.th - (2*_td.th), 1.0, 1, 0, 2, 4)
      } else {
         Draw.sprite(_td.spr, tid, _x * _td.tw, _y * _td.th, 1.0, 1, 0, 2, 2)
      }
   }
}