import "random" for Random

import "engine" for Draw, Asset, TileMap, Trap, Button
import "math" for Math
import "camera" for Camera

import "player" for Player
import "mine" for Mine
import "minetext" for MineText
import "meter" for Meter

class Game {
   nextScene { _nextScene }
   nextScene=(params) { _nextScene = params }

   entities { _entities }
   cam { _cam }
   rnd { _rnd }

   construct new(params) {
      _icons = Asset.create(Asset.Sprite, "icons", "gfx/icons.png")
      Asset.spriteSet(_icons, 16, 16, 0, 0)

      _player = Player.new(this, {}, 220, 50)
      _entities = [_player]
      _cam = Camera.new(16, 16, 320, 180)
      _rnd = Random.new()
      _generatedX = 0 // how far in the world we've generated level parts

      _uiEntities = []
      _meter = Meter.new()

      Asset.loadAll()
   }

   update(dt) {
      // find the top left corner of the camera and figure out if we need to generate more level
      var cx = _cam.toWorld(0,0)[0]
      if (cx <= _generatedX) {
         // the new chunk will be a bit longer than the camera view so we don't pop in
         _generatedX = cx - (_cam.w * 1.25)

         var y = 0
         var x = cx
         // generate a random chance of an obstacle in every 16px grid
         while (y < _cam.h) {
            while (x > _generatedX) {
               if (_rnd.int(8) == 0) {
                  _entities.add(Mine.new(this, {"sprite": _icons}, x + _rnd.int(8), y + _rnd.int(8)))
               }
               x = x - 16
            }
            y = y + 16
            x = cx
         }
      }

      // if the player isn't on the starting platform, autoscroll the camera
      if (_player.launched) {
         _cam.move(_cam.x - 0.25, 0)
      }

      // update each entity and kill them if they're off camera
      for (ent in _entities) {
         ent.think(dt)
         if (ent.x > _cam.x + _cam.w) {
            ent.die(false)
         }
      }

      // trim out dead entities from the list
      _entities = _entities.where {|c| !c.dead }.toList

      // update ui entities
      for (ent in _uiEntities) {
         ent.think(dt)
      }
      _uiEntities = _uiEntities.where {|c| !c.dead }.toList
      _meter.think(dt)
   }

   onMineHit(mine) {
      _uiEntities.add(MineText.new(mine.spr, 140, 160))
      _meter.increase()
   }

   draw(w, h) {
      Draw.clear()
      Draw.resetTransform()
      Draw.scale(h / _cam.h)

      Draw.translate(-_cam.x, -_cam.y)

      for (ent in _entities) {
         ent.draw()
      }

      // draw the ui elements on top of everything without the camera translation
      Draw.translate(_cam.x, _cam.y)
      for (ent in _uiEntities) {
         ent.draw()
      }
      _meter.draw()
   }

   shutdown() {
      Asset.clearAll()
   }  
}