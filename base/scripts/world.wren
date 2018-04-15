class World {
   /*
   time { _time }
   tileCollider { _tileCollider }
   cam { _cam }
   entities { _entities }
   level { _level }
   levelNum { _levelNum }
   coins { _coins }
   coins=(c) { _coins = c }
   totalCoins { _totalCoins }
   totalCoins=(c) { _totalCoins = c }
   drawHud { _drawHud }
   drawHud=(b) { _drawHud = b }
   player { _player }

   construct new(i) {
      _getTile = Fn.new { |x, y|
         if (x < _level.x || x >= _level.x + _level.w) {
            return 1
         }

         return TIC.mget(x,y)
      }
      _tileCollider = TileCollider.new(_getTile, 8, 8)

      _entities = []
      _coins = 0
      _totalCoins = 0
      _time = 0
      _drawHud = true
      _levels = [
         Level.new(0, 0, 43, 17),
         Level.new(45, 0, 30, 17)
      ]

      _level = _levels[i]
      _levelNum = i
      _cam = Camera.new(8, 8, 240, 136)
      _cam.constrain(_level.x*8, _level.y*8, _level.w*8, _level.h*8)
      

      var entmappings = {
         255: Player,
         254: LevelExit,
         253: Coin,
         248: MovingPlatform,
         247: MovingPlatform,
         246: MovingPlatform,
         245: MovingPlatform,
         244: FallingPlatform,
         243: Spring,
         242: Spike,
         241: Cannon,
         240: Cannon,
         239: Cannon,
         238: Cannon,
         14: Flame,
         13: Flame,
         12: Flame,
         11: Flame,
         10: Flame,
         9: Flame,
         8: Flame,
         7: Flame,
      }

      for (y in _level.y.._level.y+_level.h) {
         for (x in _level.x.._level.x+_level.w) {
            var i = TIC.mget(x, y)
            var e = entmappings[i]
            if (e != null) {
               var ent = e.new(this, i, x*8, y*8)
               if (ent is Player) {
                  _entities.insert(0, ent)
                   _player = ent
               } else {
                  _entities.add(ent)
               }
            }

         }
      }

      _remap = Fn.new { |i, x, y|
         if (i >= 224) {
            return 0
         }
         return i
      }
   }

   update(dt) {
      _time = _time + dt
      //Debug.text("time", time)
      //Debug.text("ents", _entities.count)

      for (ent in _entities) {
         if (ent.active) {
            ent.think(dt)
         }
      }

      for (i in _entities.count-1..0) {
         if (_entities[i].active == false) {
            _entities.removeAt(i)
         }
      }
   }

   draw(t) {
      TIC.cls(2)
      TIC.map(_cam.tx, _cam.ty, _cam.tw, _cam.th, 0 - _cam.x % 8, 0 - _cam.y % 8, 2, 1, _remap)

      for (ent in _entities) {
         if (ent.active) {
            cam.entToCamera(ent)
            ent.draw(t)
         }
      }

      if (_drawHud && _player != null) {
         TIC.rect(0, 0, 240, 12, 1)
         if (_totalCoins > 0) {
            TIC.spr(256, 100, 1, 0)
            TIC.print("%(_coins)/%(_totalCoins)", 110, 3, _coins == _totalCoins ? 14 : 15, true)
         }
         TIC.print("S", 4, 3, 15, true)

         for (i in 0..2) {
            TIC.spr(i < _player.health ? 265 : 281, 198+(i*14), 2, 0, 1, 0, 0, 2, 1)
         }

         var pct = (_player.pMeter / _player.pMeterCapacity * 40 / 8).floor
         for (i in 0..4) {
            TIC.spr(i < pct ? 283 : 267, 11 + i * 6, 2, 0)
         }
      }
   }
   */
}