import "engine" for Trap, Button, Draw, Asset, Fill, Color, TileMap
import "debug" for Debug
import "collision" for TileCollider
import "camera" for Camera
import "entities" for LevelExit, Coin, MovingPlatform, FallingPlatform, Spring, Spike, Cannon, Flamethrower
import "player" for Player

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

   construct new(mapName) {
      TileMap.load(mapName)
      var mapProps = TileMap.getMapProperties()

      _w = mapProps["width"]
      _h = mapProps["height"]
      _tw = mapProps["tileWidth"]
      _th = mapProps["tileHeight"]
      _maxX = _w * _tw
      _maxY = _h * _th
      _layers = TileMap.layerNames()

      _worldLayer = TileMap.layerByName("world")
      if (_worldLayer == -1) {
         Trap.error(2, "can't find layer named world")
         return
      }

      var rgba = mapProps["backgroundColor"]
      _backgroundColor = [(rgba>>16)&0xFF, (rgba>>8)&0xFF, (rgba)&0xFF, (rgba>>24)&0xFF]

      _music = mapProps["properties"]["music"]
      _title = mapProps["properties"]["title"]
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
   ticks { _ticks }
   tileCollider { _tileCollider }
   cam { _cam }
   entities { _entities }
   level { _level }
   coins { _coins }
   coins=(c) { _coins = c }
   totalCoins { _totalCoins }
   totalCoins=(c) { _totalCoins = c }
   drawHud { _drawHud }
   drawHud=(b) { _drawHud = b }
   player { _player }
   spr { _spr }
   
   construct new(mapName) {
      _getTile = Fn.new { |x, y| _level.getTile(x, y) }
      _level = Level.new(mapName)
      _tileCollider = TileCollider.new(_getTile, _level.tw, _level.th)
      _entities = []
      _coins = 0
      _totalCoins = 0
      _ticks = 0
      _drawHud = true
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
         "Flamethrower": Flamethrower
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

      _fixedFont = Asset.create(Asset.BitmapFont, "fixedfont", "gfx/panicbomber.png")
      Asset.bmpfntSet(_fixedFont, " !\"#$\%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~", 8, 0, 8, 8)

      _fixedFontBlue = Asset.create(Asset.BitmapFont, "fixedfontblue", "gfx/panicbomber_blue.png")
      Asset.bmpfntSet(_fixedFontBlue, " !\"#$\%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~", 8, 0, 8, 8)

      var sprites = Asset.create(Asset.Image, "sprites", "maps/tilesets/plat.gif")

      _music = Asset.create(Asset.Mod, "music", level.music)
      _deathMusic = Asset.create(Asset.Mod, "deathmusic", "music/victory_ditty_24.mod")
      _victoryMusic = Asset.create(Asset.Mod, "victorymusic", "music/jingles_22.mod")

      Asset.loadAll()

      _spr = Asset.createSprite(sprites, 8, 8, 0, 0)

      _musicHnd = Trap.sndPlay(_music, 1.0, 0.0, true)
   }

   playMusic(type) {
      Trap.sndStop(_musicHnd)

      if (type == "death") {
         _musicHnd = Trap.sndPlay(_deathMusic)
      } else if (type == "victory") {
         _musicHnd = Trap.sndPlay(_victoryMusic)
      }
   }

   update(dt) {
      _ticks = _ticks + dt
      // Debug.text("world", "time", _ticks)
      // Debug.text("world", "ents", _entities.count)

      for (ent in _entities) {
         if (ent.active) {
            ent.think(1/60)
         }
      }

      for (i in _entities.count-1..0) {
         if (_entities[i].active == false) {
            _entities.removeAt(i)
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
      if (_drawHud && _player != null) {

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
   }

   shutdown() {

   }
}