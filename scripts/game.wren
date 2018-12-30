import "random" for Random

import "engine" for Draw, Asset, TileMap, Trap, Button, Fill
import "math" for Math
import "camera" for Camera
import "timer" for Timer

import "player" for Player
import "mine" for Mine
import "minetext" for MineText
import "meter" for Meter
import "levels" for Levels
import "collectible" for Collectible

class Game {
   nextScene { _nextScene }
   nextScene=(params) { _nextScene = params }

   entities { _entities }
   cam { _cam }
   rnd { _rnd }
   meter { _meter }

   construct new(level) {
      var icons = Asset.create(Asset.Sprite, "icons", "gfx/icons.png")
      Asset.spriteSet(icons, 16, 16, 0, 0)

      _shag = Asset.create(Asset.Image, "shag", "gfx/shag.png")
      _skirting = Asset.create(Asset.Image, "skirting", "gfx/skirting.png")

      Asset.create(Asset.Image, "iconbg", "gfx/icon-background.png")

      var collectible = Asset.create(Asset.Sprite, "collectible", "gfx/collectible.png")
      Asset.spriteSet(collectible, 12, 12, 0, 0)

      var font = Asset.create(Asset.BitmapFont, "font", "gfx/font.png")
      Asset.bmpfntSet(font, "abcdefghijklmnopqrstuvwxyz!?'$1234567890", 0, 1, 2, 5)

      _meter = Meter.new()
      _cam = Camera.new(16, 16, 320, 180)
      _rnd = Random.new()
      _player = Player.new(this, {}, 220, 50)

      _t = 0
      _entities = [_player]
      _generatedX = 0 // how far in the world we've generated level parts
      _itemsCollected = 0

      _level = Levels.Levels[level || 0]

      var bgSprite = _level["background"]["sprite"]
      _bg = Asset.create(Asset.Sprite, bgSprite, "gfx/" + bgSprite + ".png")
      Asset.spriteSet(_bg, 32, 48, 0, 0)
      _bgMad = false

      // rain generator
      _nextRainTick = 0

      _uiEntities = []

      Asset.loadAll()
   }

   generateRain(subtype) {
      var cx = _cam.toWorld(0,0)[0]

      if (cx <= _generatedX) {
         _generatedX = cx - _cam.w * 1.25
      }

      if (_t < _nextRainTick) {
         return
      }

      _nextRainTick = _t + 20
      
      var d = [0, 0]
      var y = 0

      if (subtype == "rain") {
         d = [_rnd.float(0.05, 0.25), _rnd.float(0.4, 0.5)]
         y = -16
      } else if (subtype == "snow") {
         d = [_rnd.float(-0.25, 0.25), _rnd.float(0.2, 0.4)]
         y = -16
      } else if (subtype == "ashes") {
         d = [_rnd.float(-0.25, 0.25), _rnd.float(-0.2, -0.4)]
         y = _cam.h
      }

      var mine = Mine.new(this, {}, _rnd.int(cx - 64, cx+_cam.w), y, d[0], d[1])
      _entities.add(mine)
   }

   generateMinefield() {
      // only run once
      var cx = _cam.toWorld(0,0)[0]
      if (cx > _generatedX) {
         return
      }

      var lastCoin = [0, 0]

      // the new chunk will be a bit longer than the camera view so we don't pop in
      _generatedX = cx - _cam.w * 1.25

      var y = 0
      var x = cx
      // generate a random chance of an obstacle in every 24px grid
      while (x > _generatedX) {
         while (y < _cam.h) {
            var i = _rnd.int(100)
            // 20% chance of a mine
            if (i <= 20) {
               var mine = Mine.new(this, {}, x + _rnd.int(8), y + _rnd.int(8), 0, 0)
               _entities.add(mine)
            // 15% chance of a collectible as long as it's spaced out enough
            } else if (i <= 35) {
               if ((lastCoin[0] - x).abs > 24 && (lastCoin[1] - y).abs > 48 && y < 150) {
                  var collectible = Collectible.new(this, {}, x + _rnd.int(8), y + _rnd.int(8))
                  _entities.add(collectible)
                  lastCoin = [x, y]
               }
            }
            y = y + 24
         }
         x = x - 24
         y = 0
      }
   }

   update(dt) {
      _t = _t + dt

      // if we've past the point where we need to switch, pick a new random section
      // var cx = _cam.toWorld(0,0)[0]
      // if (cx <= _generatedX) {
      //    // hardcode first phase to minefield
      //    _generateMode = _generateMode == null ? "minefield" : _rnd.sample(_modes)
      // }
      // if (_player.x < -_levelLength) {
      //    if (_level == Levels.Levels.count - 1) {
      //       nextScene = "ending"
      //    } else {
      //       nextScene = ["levelending", _level + 1]
      //    }
      // }

      if (_itemsCollected >= 3) {
         nextScene = _level["nextlevel"]
         return
      }

      // run the level generator tick
      var genMode = _level["generateMode"]
      if (genMode == "minefield") {
         generateMinefield()
      } else if (genMode == "rain" || genMode == "snow" || genMode == "ashes") {
         generateRain(genMode)
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

   onMineHit(ent) {
      _uiEntities.add(MineText.new(ent.spr, 140, 160))
      _bgMad = true
      Timer.runLater(100) {
         _bgMad = false
      }
   }

   onCollectibleHit(ent) {
      _itemsCollected = _itemsCollected + 1
   }

   draw(w, h) {
      Draw.clear()
      Draw.resetTransform()
      Draw.scale(h / _cam.h)

      // background
      Draw.setColor(_level["background"]["color"])
      Draw.rect(0, 0, 320, 160, Fill.Solid)
      Draw.setColor(73, 60, 43, 255)
      Draw.rect(0, 154, _cam.w, 64, Fill.Solid)
      Draw.setColor(255, 255, 255, 255)
      Draw.image(_skirting, 0, 142, 320, 12)

      drawGrass([0])
      drawBg()
      drawGrass(1..4)

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

   drawBg() {
      var x = (-_cam.x / 5) + 10 

      var frame = 0
      if (_bgMad) {
         frame = 2
      } else {
         frame = _t % 32 < 16 ? 1 : 0
      }

      Draw.sprite(_bg, frame, x, 64, 1.0, 2.0)
   }

   drawGrass(range) {
      var grassStart = 140

      for (i in range) {
         // -32 : start offscreen so we don't have gaps on the left side
         // -_cam.x : what drives the motion
         // *(i+1) : further down rows move faster
         // / 5 : slow down the movement
         // + i*4 : offset initial position of rows so they don't ever all match up
         // % 32 : sprite is 32 wide, wrap starting x from 0 to -32
         var x = -32 + (-_cam.x * (i+1) / 5 + i*4) % 32
         while (x < _cam.w) {
            var y = i * 6 + grassStart
            Draw.image(_shag, x, y, 32, 16, 1.0, 1.0, i % 2 == 0 ? 1 : 0)

            x = x + 32
         }
      }
   }

   shutdown() {
      Asset.clearAll()
   }  
}