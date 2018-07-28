import "engine" for Draw, Asset, Trap, Color, Fill, Button, TileMap, CVar
import "timer" for Timer
import "debug" for Debug
import "grid" for Grid
import "piecetray" for PieceTray
import "tower" for Tower
import "actionqueue" for ActionQueue
import "entities/goat" for Goat
import "soundcontroller" for SoundController
import "random" for Random
import "math" for Math

class TD {
   nextScene { _nextScene }
   nextScene=(params) { _nextScene = params }

   th { _th }
   tw { _tw }
   time { _time }
   spr { _spr }
   grid { _grid }
   scale { _scale }
   pieceTray { _pieceTray }
   gameMode { _gameMode }
   font { _font }
   costs { _costs }
   currencies { _currencies }
   currSymbol { _currSymbol }
   enableMagicTower { _enableMagicTower }
   vHeight { _vHeight }

   construct new(mapName) {
      Asset.clearAll()

      _nextScene = null
      _time = 0

      _mapName = mapName
      _checkForWin = false // when true, check for all enemies dead to trigger win condition
      _coinHealth = 3 // when 0, gameover is triggered

      _rnd = Random.new()

      SoundController.stopMusic()

      // load the requested map
      TileMap.load(mapName)
      _layers = TileMap.layerNames()

      _mapProps = TileMap.getMapProperties()

      _tw = _mapProps["tileWidth"]
      _th = _mapProps["tileHeight"]
      _gridX = _mapProps["properties"]["gridx"]
      _gridY = _mapProps["properties"]["gridy"]
      _gridW = _mapProps["properties"]["gridw"]
      _gridH = _mapProps["properties"]["gridh"]

      _musicPath = _mapProps["properties"]["music"]
      _music = Asset.create(Asset.Sound, "game_bgm", _musicPath)

      // setup rules based on gamemode key
      _gameMode = _mapProps["properties"]["gamemode"]

      Asset.create(Asset.Image, "goat", "gfx/game1/goat.png")       // FIXME: make me per game?
      Asset.create(Asset.Sound, "game_click", "sound/click.wav")
      Asset.create(Asset.Sound, "game_plop", "sound/plop.wav")
      _coinHurt = Asset.create(Asset.Sound, "coin_hurt", "sound/coin_hurt.wav")
      Asset.create(Asset.Sound, "game_rotate", "sound/rotate.wav")
      Asset.create(Asset.Sound, "arrow_shoot", "sound/arrow_shoot.wav")
      Asset.create(Asset.Sound, "cannon_shoot", "sound/cannon_shoot.wav")
      Asset.create(Asset.Sound, "magic_shoot", "sound/magic_shoot.wav")

      if (_gameMode == 1) {
         // TODO: this image is loaded twice since the tmx also loads this but we can't use the
         // same name to load the existing texture because it's an image, and not a sprite
         // so we'll just dupe this image since whatever it's so small
         _spr = Asset.create(Asset.Sprite, "e1spr", "maps/tilesets/e1.png")
         _font = Asset.create(Asset.Font, "speccy", "fonts/spectrum.ttf")
         Asset.spriteSet(_spr, 8, 8, 0, 0)
         Asset.create(Asset.Sound, "goat_die", "sound/goat1_die.wav")
         _vHeight = 180
         _winScene = "game1_win"
         _currSymbol = "£"
         _enableMagicTower = false
      } else if (_gameMode == 2) {
         _spr = Asset.create(Asset.Sprite, "e2spr", "maps/tilesets/e2.png")
          _font = Asset.create(Asset.Font, "speccy", "fonts/spectrum.ttf")
         Asset.spriteSet(_spr, 8, 8, 0, 0)
         Asset.create(Asset.Sound, "goat_die", "sound/goat2_die.wav")
         _vHeight = 180
         _winScene = "game1_win"
         _currSymbol = "£"
         _enableMagicTower = true
      } else if (_gameMode == 3) {
         _spr = Asset.create(Asset.Sprite, "e3spr", "maps/tilesets/e3.png")
         _font = Asset.create(Asset.Font, "speccy", "fonts/spectrum.ttf")
         Asset.spriteSet(_spr, 8, 8, 0, 0)
         Asset.create(Asset.Sound, "goat_die", "sound/goat3_die.wav")
         _vHeight = 180   
         _winScene = "game1_win"
         _currSymbol = "$"
         _enableMagicTower = true
      } else if (_gameMode == 4) {
         _spr = Asset.create(Asset.Sprite, "e4spr", "maps/tilesets/e4.png")
         _font = Asset.create(Asset.Font, "speccy", "fonts/spectrum.ttf")
         Asset.spriteSet(_spr, 32, 32, 0, 0)   
         Asset.create(Asset.Sound, "goat_die", "sound/goat4_die.wav") 
         _vHeight = 720
         _winScene = "game1_win"
         _currSymbol = "$"
         _enableMagicTower = true
      }

       _scale = Trap.getResolution()[1] / _vHeight

      _costs = {
         "tower1": [5,0,0],
         "tower2": [5,0,0],
         "tower3": [8,0,0],
         "grass": [2,0,0],
         "piece0": [1,0,0],
         "piece1": [1,0,0],
         "piece2": [1,0,0],
         "piece3": [1,0,0],
      }

      _currencies = List.filled(1, 40)

      _actions = [
         [5, Fn.new { spawnGroup(2) }],
         [5, Fn.new { spawnGroup(3) }],
         [5, Fn.new { spawnGroup(1) }],
         [5, Fn.new { spawnGroup(2) }],
         [7, Fn.new { spawnGroup(4) }],
         [1, Fn.new { _checkForWin = true }]
      ]

      // end

      _actionQueue = ActionQueue.new(_actions)

      _grid = Grid.new(this, _gridX, _gridY, _gridW, _gridH, _tw, _th)
      _grid.setGoal(_mapProps["properties"]["goalx"], _rnd.int(0, _gridH-2))

      _pieceTray = PieceTray.new(this, 272, 0, 48, 180)

      Asset.loadAll()

      SoundController.playMusic(_music)
   }

   spawnGroup(count) {
      var baseY = _rnd.int(0, _gridH)
      for (i in 0...count) {
         Timer.runLater(i*0.9) {
            var goatY = Math.clamp(0, _rnd.int(baseY-5, baseY+5), _gridH - 1)
            _grid.addEntity(Goat.new(this, 1, goatY))
         }
      }
   }

   onEntityDied(ent) {
      if (ent.type == "goat") {
         _currencies[0] = _currencies[0] + 3
      }
   }

   gameOver() {
      _nextScene = ["gameover", {"map":_mapName, "mode": _gameMode}]
   }

   onTouchCoin() {
      _coinHealth = _coinHealth - 1
      if (_coinHealth == 0) {
         gameOver()
      } else {
         SoundController.playOnce(_coinHurt)
      }
   }

   update(dt) {
      _time = _time + dt

      _actionQueue.update(dt)

      if (Trap.keyPressed(Button.Start, 0, -1)) {
         _nextScene = "gameselect"
      }

      _grid.update(dt)
      _pieceTray.update(dt)

      if (_checkForWin == true) {
         var left = _grid.entities.where{|e| e.type == "goat"}.count
         if (left == 0) {
            Timer.runLater(3) {
               _nextScene = _winScene
            }
            _checkForWin = false
         }
      }
   }

   draw(w, h) {
      Draw.clear()
      Draw.resetTransform()
      _scale = h / _vHeight
      Draw.transform(_scale, 0, 0, _scale, 0, 0)

      for (i in 0.._layers.count-1) {
         Draw.mapLayer(i)
      }

      _grid.draw()
      _pieceTray.draw()

      Draw.submit()
   }

   shutdown() {
      SoundController.stopMusic()
   }
}