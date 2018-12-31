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
   canWin { _canWin }
   level { _level }

   construct new(level) {

      if (level == "endless") {
         _endless = true
         _level = Levels.Levels[0]
      } else {
         _level = Levels.Levels[level || 0]
         // _level = Levels.Levels[1]
         _endless = false
      }

      var icons = Asset.create(Asset.Sprite, "icons", "gfx/icons.png")
      Asset.spriteSet(icons, 16, 16, 0, 0)

      _shag = Asset.create(Asset.Image, "shag", "gfx/shag.png")
      _skirting = Asset.create(Asset.Image, "skirting", "gfx/skirting.png")

      _iconBg = Asset.create(Asset.Image, "iconbg", "gfx/icon-background.png")

      _collectible = Asset.create(Asset.Sprite, "collectible", "gfx/collectible.png")
      Asset.spriteSet(_collectible, 12, 12, 0, 0)

      _font = Asset.create(Asset.BitmapFont, "font", "gfx/font.png")
      Asset.bmpfntSet(_font, "abcdefghijklmnopqrstuvwxyz!?'$1234567890", 0, 1, 2, 5)

      if (!_endless && _level["help"] == true) {
         _arrows = Asset.create(Asset.Image, "arrows", "gfx/arrows.png")
         _zx = Asset.create(Asset.Image, "zx", "gfx/zx.png")

         _dpad = Asset.create(Asset.Image, "dpad", "gfx/arrows-gamepad.png")
         _padButtons = Asset.create(Asset.Image, "dpadbuttons", "gfx/zx-gamepad.png")
      }

      _tick = Asset.create(Asset.Image, "tick", "gfx/tick.png")
      _cross = Asset.create(Asset.Image, "cross", "gfx/cross.png")

      _meter = Meter.new()
      _cam = Camera.new(16, 16, 320, 180)
      _rnd = Random.new()
      _player = Player.new(this, {}, 220, 70)

      _t = 0
      _entities = [_player]
      _generatedX = 0 // how far in the world we've generated level parts
      _itemsToWin = 5
      _totalItems = 0
      _canWin = false
      _paused = false
      _pauseQuitSelected = false
      _flashCounter = false

      if (_endless == true) {
         _bg = null
      } else {
         var bgSprite = _level["background"]["sprite"]
         _bg = Asset.create(Asset.Sprite, bgSprite, "gfx/" + bgSprite + ".png")
         Asset.spriteSet(_bg, _level["background"]["spriteWidth"] || 32, 48, 0, 0)
         _bgMad = false
         _arrow = Asset.create(Asset.Image, "arrow", "gfx/arrow.png")
      }

      // rain generator
      _nextRainTick = 0

      _uiEntities = []

      Asset.loadAll()
   }

   generateRain(subtype) {
      var cx = _cam.toWorld(0,0)[0]

      if (cx <= _generatedX) {
         var sectionWidth = _cam.w * 1.25
         _generatedX = cx - sectionWidth
         var coinStart = cx
         while (coinStart >= _generatedX) {
            var collectible = Collectible.new(this, {}, coinStart - _rnd.int(50), _rnd.int(8, 140))
            _entities.add(collectible)
            coinStart = coinStart - sectionWidth / 3
         }
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

   pauseUpdate() {
      if (Trap.keyPressed(Button.Start, 0, -1)) {
         if (_pauseQuitSelected) {
            _nextScene = "title"
         }
         _paused = false
      }

      if (Trap.keyPressed(Button.Left, 0, -1)) {
         _pauseQuitSelected = true
      }
      if (Trap.keyPressed(Button.Right, 0, -1)) {
         _pauseQuitSelected = false
      }
   }

   update(dt) {
      if (_paused) {
         pauseUpdate()
         return
      }
      
      _t = _t + dt

      if (!_paused && Trap.keyPressed(Button.Start, 0, -1)) {
         _paused = true
         _pauseQuitSelected = false
      }

      // select a new random level in endless if we've passed one
      if (_endless) {
         var cx = _cam.toWorld(0,0)[0]
         if (cx <= _generatedX) {
            _level = _rnd.sample(Levels.Levels)
         }
      // collected all the items, next level
      } else if (_itemsToWin <= 0) {
         _canWin = true
      }

      if (_canWin && _player.y < -16) {
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
      _itemsToWin = Math.max(0, _itemsToWin - 1)
      _totalItems = _totalItems + 1

      _flashCounter = true
      Timer.runLater(120) {
         _flashCounter = false
      }
   }

   onGameOver() {
      _nextScene = ["gameover", [_level, _totalItems]]
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

      if (_canWin) {
         drawExit()
      }

      drawStatus()

      if (!_endless && !_player.launched && _level["help"] == true) {
         drawHelp()
      }

      if (_paused) {
         drawPause()
      }
   }

   drawPause() {
      var w = 64
      var h = 28
      Draw.setColor(0, 0, 0, 200)
      Draw.rect(320/2 - w/2, 160/2 - h/2, w, h, false)
      Draw.setColor(255, 255, 255, 255)

      var tw = Asset.measureBmpText(_font, "quit?")
      Draw.bmpText(_font, 320/2 - tw/2, 160/2 - h/2 + 2, "quit?")

      if (_pauseQuitSelected) {
         Draw.setColor(255, 255, 255, 255)
         Draw.image(_tick, 320/2 - w/2 + 8, 160/2 - h/2 + 10)
         Draw.setColor(127, 127, 127, 255)
         Draw.image(_cross, 320/2 + w/2 - 24, 160/2 - h/2 + 10)
      } else {
         Draw.setColor(127, 127, 127, 255)
         Draw.image(_tick, 320/2 - w/2 + 8, 160/2 - h/2 + 10)
         Draw.setColor(255, 255, 255, 255)
         Draw.image(_cross, 320/2 + w/2 - 24, 160/2 - h/2 + 10)
      }
         Draw.setColor(255, 255, 255, 255)
   }

   drawExit() {
      // draw arrows pointing to exit if you can win
      var y = (_t / 4).sin
      Draw.image(_arrow, 320/4, y + 2)
      Draw.bmpText(_font, 320/4 - 4, y + 12, "exit")
      Draw.image(_arrow, 320/4 * 2, y + 2)
      Draw.bmpText(_font, 320/4 * 2 - 4, y + 12, "exit")
      Draw.image(_arrow, 320/4 * 3, y + 2)
      Draw.bmpText(_font, 320/4 * 3 - 4, y + 12, "exit")
   }

   drawStatus() {
      _meter.draw()

      // this is bad but GAME JAM CODE!
      // draw collectibles status
      var collectX = 222
      Draw.sprite(_collectible, 0, collectX, 148)

      var msg
      var msgX
      var msgY
      if (_endless) {
         msg = "%(_totalItems) dodged topics!"
      } else if (_itemsToWin != 0) {
         msg = "%(_itemsToWin) more to escape!"
      } else {
         msg = "get out of there!"
      }

      msgX = collectX + 13
      msgY = 153

      if ((_flashCounter || _itemsToWin == 0) && _t % 24 < 12) {
         Draw.setColor(235, 137, 49, 255)
      } else {
         Draw.setColor(27, 38, 50, 255)
      }
      
      Draw.bmpText(_font, msgX+1, msgY, msg)
      Draw.bmpText(_font, msgX-1, msgY, msg)
      Draw.bmpText(_font, msgX, msgY-1, msg)
      Draw.bmpText(_font, msgX, msgY+1, msg)

      Draw.setColor(247, 226, 107, 255)
      Draw.bmpText(_font, msgX, msgY, msg)
      Draw.setColor(255, 255, 255, 255)
   }

   drawHelp() {
      var x = 80
      var y = 25
      Draw.setColor(0, 0, 0, 200)
      Draw.rect(x, y, 90, 100, Fill.Solid)
      Draw.setColor(255, 255, 255, 255)

      x = x + 5
      y = y + 5

      Draw.image(_zx, x, y)
      Draw.image(_padButtons, x, y + 20)
      Draw.bmpText(_font, x + 8, y + 40, "flap")

      Draw.image(_arrows, x + 48, y)
      Draw.image(_dpad, x + 48, y + 20)
      Draw.bmpText(_font, x + 55, y + 40, "move")

      Draw.image(_iconBg, x + 8, y + 55)
      Draw.bmpText(_font, x + 7, y + 80, "avoid")

      Draw.sprite(_collectible, 0, x + 60, y + 58)
      Draw.bmpText(_font, x + 55, y + 80, "grab")


   }

   drawBg() {
      if (_endless) {
         return
      }
      
      var x = (-_cam.x / 5) + 10 

      var frame = 0
      if (_bgMad) {
         frame = 2
      } else {
         frame = _t % 48 < 24 ? 1 : 0
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