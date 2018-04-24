import "engine" for Trap, Button, Draw, Asset, Fill, Color, TileMap
import "debug" for Debug
import "collision" for TileCollider
import "camera" for Camera
import "player" for Player
import "timer" for Timer
import "soundcontroller" for SoundController

import "ent/flamethrower" for Flamethrower
import "ent/levelexit" for LevelExit
import "ent/spring" for Spring
import "ent/cannonball" for Cannonball
import "ent/coin" for Coin
import "ent/movingplatform" for MovingPlatform
import "ent/fallingplatform" for FallingPlatform
import "ent/cannon" for Cannon
import "ent/spike" for Spike
import "ent/walker" for Walker

class Level {
   w { _w }
   h { _h }
   tw { _tw }
   th { _th }
   maxX { _maxX }
   maxY { _maxY }
   layers { _layers }
   music { _music }
   title { _title }
   worldLayer { _worldLayer }
   backgroundColor { _backgroundColor }
   props { _mapProps }

   construct new(mapName) {
      TileMap.load(mapName)
      _mapProps = TileMap.getMapProperties()

      _w = _mapProps["width"]
      _h = _mapProps["height"]
      _tw = _mapProps["tileWidth"]
      _th = _mapProps["tileHeight"]
      _maxX = _w * _tw
      _maxY = _h * _th
      _layers = TileMap.layerNames()

      _worldLayer = TileMap.layerByName("world")
      if (_worldLayer == -1) {
         Trap.error(2, "can't find layer named world")
         return
      }

      var rgba = _mapProps["backgroundColor"]
      _backgroundColor = [(rgba>>16)&0xFF, (rgba>>8)&0xFF, (rgba)&0xFF, (rgba>>24)&0xFF]

      _music = _mapProps["properties"]["music"]
      _title = _mapProps["properties"]["title"]
   }

   getTile(x, y) {
      if (x < 0 || x > _w) {
         return 1
      }

      if (y < 0 || y > _h) {
         return 0
      }

      return TileMap.getTile(_worldLayer, x, y)
   }

   objects() {
      var merged = []

      for (layer in _layers) {
         var id = TileMap.layerByName(layer)
         var objects = TileMap.objectsInLayer(id)
         for (obj in objects) {
            merged.add(obj)
         }
      }

      return merged
   }
}

class World {
   nextScene { _nextScene }
   ticks { _ticks }
   tileCollider { _tileCollider }
   cam { _cam }
   entities { _entities }
   level { _level }
   coins { _coins }
   coins=(c) { _coins = c }
   gravity { _gravity }
   totalCoins { _totalCoins }
   totalCoins=(c) { _totalCoins = c }
   player { _player }
   spr { _spr }
   levelWon { _levelWon }
   
   construct new(mapName) {
      _getTile = Fn.new { |x, y| _level.getTile(x, y) }
      _nextScene = null
      _mapName = mapName
      _level = Level.new(mapName)
      _levelWon = false
      _tileCollider = TileCollider.new(_getTile, _level.tw, _level.th)
      _entities = []
      _coins = 0
      _totalCoins = 0
      _ticks = 0
      _gravity = 0.1875
      _cam = Camera.new(8, 8, 320, 180)
      _cam.constrain(0, 0, _level.maxX, _level.maxY)
      
      var objects = _level.objects()

      var entmappings = {
         "Player": Player,
         "LevelExit": LevelExit,
         "Coin": Coin,
         "MovingPlatform": MovingPlatform,
         "FallingPlatform": FallingPlatform,
         "Spring": Spring,
         "Spike": Spike,
         "Cannon": Cannon,
         "Flamethrower": Flamethrower,
         "Walker": Walker
      }

      for (obj in objects) {
         var eType = entmappings[obj["type"]]
         if (eType != null) {
            var ent = eType.new(this, obj, obj["x"], obj["y"] - level.th)
            if (ent is Player) {
               _entities.insert(0, ent)
               _player = ent
            } else {
               _entities.add(ent)
            }
         }
      }

      _font = Asset.create(Asset.BitmapFont, "font", "gfx/good_neighbors.png")
      Asset.bmpfntSet(_font, "!\"#$\%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~", 0, -1, 7, 16)

      _fixedFont = Asset.create(Asset.BitmapFont, "fixedfont", "gfx/panicbomber.png")
      Asset.bmpfntSet(_fixedFont, " !\"#$\%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~", 8, 0, 8, 8)

      _fixedFontBlue = Asset.create(Asset.BitmapFont, "fixedfontblue", "gfx/panicbomber_blue.png")
      Asset.bmpfntSet(_fixedFontBlue, " !\"#$\%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~", 8, 0, 8, 8)

      var sprites = Asset.create(Asset.Image, "sprites", "maps/tilesets/plat.gif")

      _music = Asset.create(Asset.Mod, "music", level.music)
      _deathMusic = Asset.create(Asset.Mod, "deathmusic", "music/victory_ditty_24.mod")
      _victoryMusic = Asset.create(Asset.Mod, "victorymusic", "music/jingles_22.mod")
      _winSpeech = Asset.create(Asset.Speech, "speech", "great job! you are a good dog!")
      _loseSpeech = Asset.create(Asset.Speech, "losespeech", "bad dog bad dog bad dog bad dog bad dog bad dog")

      Asset.loadAll()

      _spr = Asset.createSprite(sprites, 8, 8, 0, 0)

      SoundController.playMusic(_music)
   }

   winLevel(nextLevel) {
      _player.disableControls = true
      _levelWon = true
      SoundController.playMusic(_victoryMusic)
      SoundController.playOnce(_winSpeech, 4.0, 0, false)
      Timer.runLater(300, Fn.new {
         if (player.health > 0) {
            changeScene("intro", nextLevel)
         }
      })
   }

   reloadLevel() {
      _nextScene = ["intro", _mapName]
   }

   changeScene(name, params) {
      _nextScene = [name, params]
   }

   playerDied(player) {
      player.active = false
      SoundController.stopAsset(_winSpeech)
      SoundController.playMusic(_deathMusic)
      if (_levelWon) {
         SoundController.playOnce(_loseSpeech, 4.0, 0, false)
      }
      Timer.runLater(240, Fn.new {
         reloadLevel()
      })
   }

   update(dt) {
      _ticks = _ticks + dt
      // Debug.text("world", "time", _ticks)
      // Debug.text("world", "ents", _entities.count)

      for (ent in _entities) {
         if (ent.active) {
            ent.think(1/60)

            if (ent.y > level.maxY + 10) {
               ent.die("world")
            }
         }
      }

      if (_entities.count > 0) {
         for (i in _entities.count-1..0) {
            if (_entities[i].active == false) {
               _entities.removeAt(i)
            }
         }
      }
   }

   draw(w, h) {
      Draw.resetTransform()

      Draw.transform(h / _cam.h, 0, 0, h / _cam.h, 0, 0)
      
      Draw.setScissor(0, 16, _cam.w, _cam.h - 16)

      Draw.setColor(Color.Fill, level.backgroundColor)
      Draw.rect(0, 0, w, h, Fill.Solid)

      Draw.translate(0 - _cam.x, 0 - _cam.y)

      for (i in 0..level.layers.count-1) {
         Draw.mapLayer(i)
      }

      for (ent in _entities) {
         if (ent.active) {
            ent.draw(_ticks)
         }
      }

      Draw.resetScissor()

      Draw.translate(_cam.x, _cam.y)
      if (_player != null) {
         Draw.bmpText(_fixedFont, 4, 4, "S")
         var pct = (_player.pMeter / _player.pMeterCapacity * 40 / 8).floor
         for (i in 0..4) {
            var num = _player.pMeter == _player.pMeterCapacity ? 299 : i < pct ? 283 : 267
            Draw.sprite(_spr, num, 14 + i * 6, 4)
         }

         if (_totalCoins > 0) {
            Draw.sprite(_spr, 256, 140, 4)
            Draw.bmpText(_totalCoins == _coins ? _fixedFontBlue : _fixedFont, 150, 4, "%(_coins)/%(_totalCoins)")
         }

         for (i in 0..2) {
            Draw.sprite(_spr, i < _player.health ? 265 : 281, 275+(i*14), 4, 1, 1, 0, 2, 1)
         }
      }

      if (_levelWon) {

         if (player.health == 0) {
            Draw.setColor(Color.Fill, 149, 0, 0, 255)
         } else {
            Draw.setColor(Color.Fill, 52, 101, 36, 255)

         }
         Draw.rect(0, 70, 320, 56, Fill.Solid)

         var str1 = ""
         var str2 = ""
         var bounce = ""
         if (player.health == 0) {
            str1 = "Oh no..."
            str2 = "...you seem to have died instead..."
            bounce = "BAD DOG!"
         } else {
            str1 = "Level Cleared"
            str2 = "Now, lets move on to the next one!"
            bounce = "Good Dog!"
         }
         
         Draw.bmpText(_fixedFont, (_cam.w - (str1.count*8)) / 2, 88, str1)
         Draw.bmpText(_fixedFontBlue, (_cam.w - (str2.count*8)) / 2, 104, str2)

         var x = (_cam.w - Asset.measureBmpText(_font, bounce)) / 2
         var y = 60
         var i = 0
         for (letter in bounce) {
            var x2 = player.health == 0 ? x + (_ticks/3 + i).sin : x
            var y2 = player.health == 0 ? y + (_ticks/2 + i).sin : y + (_ticks/10 + i).sin * 4
            Draw.bmpText(_font, x2, y2, letter)
            x = x + Asset.measureBmpText(_font, letter)
            i = i + 1
         }
      }
   }

   shutdown() {

   }
}