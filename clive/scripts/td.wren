import "engine" for Draw, Asset, Trap, Color, Fill, Button, TileMap, CVar, Align
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
import "fonts" for Fonts
import "uibutton" for CoinButton, TextButton, UIButton
import "game4/slots" for SlotMachine

class PauseMenu {
   construct new(x, y) {
      _bodyFont = Asset.create(Asset.Font, "body", Fonts.body)

      _x = x
      _y = y

      _pauseButton = UIButton.new("pause", 1280-24, 0, 24, 24)

      _items = [
         TextButton.new("resume", x+30, y+30+0, 240, 40, "Resume"),
         TextButton.new("menu", x+30, y+30+50, 240, 40, "Back to Menu"),
         TextButton.new("ending", x+30, y+30+100, 240, 40, "View Ending"),
      ]
   }
   
   pauseClicked() {
      var mouse = Trap.mousePosition()
      return _pauseButton.clicked(mouse[0], mouse[1])
   }

   pauseUpdate(dt) {
      var mouse = Trap.mousePosition()
      _pauseButton.update(dt, mouse[0], mouse[1])
   }

   pauseDraw() {
      if (_pauseButton.hover) {
         Draw.setColor(Color.Fill, 101, 157, 214, 255)
      } else {
         Draw.setColor(Color.Fill, 47, 112, 176, 255)
      }

      Draw.rect(_pauseButton.x, _pauseButton.y, _pauseButton.w, _pauseButton.h, Fill.Solid)

      Draw.setColor(Color.Fill, 255, 255, 255, 255)
      Draw.rect(_pauseButton.x+6, _pauseButton.y+6, 4, 12, false)
      Draw.rect(_pauseButton.x+15, _pauseButton.y+6, 4, 12, false)
   }

   anyClicked() {
      var mouse = Trap.mousePosition()
      for (item in _items) {
         if (item.clicked(mouse[0], mouse[1])) {
            Trap.printLn("clicked %(item.id)")
            return item.id
         }
      }

      return null
   }

   update(dt) {
      var mouse = Trap.mousePosition()

      for (item in _items) {
         item.update(dt, mouse[0], mouse[1])
      }
   }

   draw() {
      Draw.setColor(Color.Fill, 0, 57, 113, 255)
      Draw.rect(_x, _y, 300, 200, false)
      for (item in _items) {
          item.draw()
      }
   }
}

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
   paused { _paused }

   construct new(params) {
      Asset.clearAll()

      _nextScene = null
      _time = 0
      _paused = false

      _extraGoats = 0
      if (params is String) {
         _mapName = params
      } else {
         _mapName = params["map"]
         _extraGoats = params["extraGoats"]
      }

      _checkForWin = false // when true, check for all enemies dead to trigger win condition
      _coinHealth = 5 // when 0, gameover is triggered
      _coinsPerKill = 3 // amount of coins for each kill

      _rnd = Random.new()
      _seenTip = false

      SoundController.stopMusic()

      // load the requested map
      TileMap.load(_mapName)
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

      Asset.create(Asset.Sound, "game_click", "sound/click.wav")
      Asset.create(Asset.Sound, "game_plop", "sound/plop.wav")
      _coinHurt = Asset.create(Asset.Sound, "coin_hurt", "sound/coin_hurt.wav")
      Asset.create(Asset.Sound, "game_rotate", "sound/rotate.wav")
      Asset.create(Asset.Sound, "arrow_shoot", "sound/arrow_shoot.wav")
      Asset.create(Asset.Sound, "cannon_shoot", "sound/cannon_shoot.wav")
      Asset.create(Asset.Sound, "magic_shoot", "sound/magic_shoot.wav")
      _coinPickup = Asset.create(Asset.Sound, "coin_pickup", "sound/coin_pickup.wav")

      if (_gameMode == 1) {
         // TODO: this image is loaded twice since the tmx also loads this but we can't use the
         // same name to load the existing texture because it's an image, and not a sprite
         // so we'll just dupe this image since whatever it's so small
         _spr = Asset.create(Asset.Sprite, "e1spr", "maps/tilesets/e1.png")
         _font = Asset.create(Asset.Font, "speccy", "fonts/spectrum.ttf")
         Asset.spriteSet(_spr, 8, 8, 0, 0)
         Asset.create(Asset.Sound, "goat_die", "sound/goat1_die.wav")
         Asset.create(Asset.Image, "goat", "gfx/game1/goat.png")
         Asset.create(Asset.Image, "goat_eating", "gfx/game1/goat_eating.png")
         _vHeight = 180
         _winScene = "game1_win"
         _currSymbol = "£"
         _enableMagicTower = false
         _goatsDropMoney = false
      } else if (_gameMode == 2) {
         _spr = Asset.create(Asset.Sprite, "e2spr", "maps/tilesets/e2.png")
          _font = Asset.create(Asset.Font, "speccy", "fonts/spectrum.ttf")
         Asset.spriteSet(_spr, 8, 8, 0, 0)
         Asset.create(Asset.Sound, "goat_die", "sound/goat2_die.wav")
         Asset.create(Asset.Image, "goat", "gfx/game1/goat.png")
         Asset.create(Asset.Image, "goat_eating", "gfx/game1/goat_eating.png")
         _vHeight = 180
         _winScene = "game2_win"
         _currSymbol = "£"
         _enableMagicTower = true
         _goatsDropMoney = false
         showTip(890, 130, "NEW: Slow goats with Freeze Tower")
      } else if (_gameMode == 3) {
         _spr = Asset.create(Asset.Sprite, "e3spr", "maps/tilesets/e3.png")
         _font = Asset.create(Asset.Font, "speccy", "fonts/spectrum.ttf")
         Asset.spriteSet(_spr, 8, 8, 0, 0)
         Asset.create(Asset.Sound, "goat_die", "sound/goat3_die.wav")
         Asset.create(Asset.Image, "goat", "gfx/game1/goat.png")
         Asset.create(Asset.Image, "goat_eating", "gfx/game1/goat_eating.png")
         _vHeight = 180   
         _winScene = "game3_win"
         _currSymbol = "$"
         _enableMagicTower = true
         _goatsDropMoney = true
      } else if (_gameMode == 4) {
         _spr = Asset.create(Asset.Sprite, "e4spr", "maps/tilesets/e4.png")
         _font = Asset.create(Asset.Font, "speccy", "fonts/spectrum.ttf")
         Asset.spriteSet(_spr, 32, 32, 0, 0)   
         Asset.create(Asset.Sound, "goat_die", "sound/goat4_die.wav")
         Asset.create(Asset.Image, "goat", "gfx/game4/goat.png")
         Asset.create(Asset.Image, "goat_eating", "gfx/game4/goat_eating.png")
         _vHeight = 720
         _winScene = "game4_win"
         _currSymbol = "$"
         _enableMagicTower = true
         _goatsDropMoney = false
         _coinsPerKill = 0 // slot machine gives you money in game 4
         showTip(760, 140, "NEW: Spin slots to earn money!")
      }

      _scale = Trap.getResolution()[1] / _vHeight

      _costs = {
         "tower1": [8,0,0],
         "tower2": [8,0,0],
         "tower3": [8,0,0],
         "grass": [2,0,0],
         "piece0": [1,0,0],
         "piece1": [1,0,0],
         "piece2": [1,0,0],
         "piece3": [1,0,0],
      }

      _currencies = List.filled(1, 40)

      _actions = [
         [7, Fn.new { spawnGroup(2) }],
         [7, Fn.new { spawnGroup(2) }],
         [7, Fn.new { spawnGroup(1) }],
         [7, Fn.new { spawnGroup(2) }],
         [7, Fn.new { spawnGroup(3) }],
         [7, Fn.new { spawnGroup(4) }],
         [1, Fn.new { _checkForWin = true }]
      ]

      // end

      _actionQueue = ActionQueue.new(_actions)

      _grid = Grid.new(this, _gridX, _gridY, _gridW, _gridH, _tw, _th)
      _grid.setGoal(_mapProps["properties"]["goalx"], _rnd.int(0, _gridH-2))

      _pieceTray = PieceTray.new(this, 272, 0, 48, 180)

      if (_gameMode == 4) {
         _slots = SlotMachine.new(this, _tw*23, _th*1)
      }

      _coins = []

      // units here are virtual 720p (see transform before drawing)
      _pauseMenu = PauseMenu.new(490, 260)

      _bodyFont = Asset.create(Asset.Font, "body", Fonts.body)
      _descriptionFont = Asset.create(Asset.Font, "description", Fonts.description)

      Asset.loadAll()

      SoundController.playMusic(_music)
   }

   showTip(x, y, text) {
      if (_seenTip == true) {
         return
      }

      _seenTip = true
      _tipX = x
      _tipY = y
      _tipText = text
      _drawTip = true
      Timer.runLater(5) {
         _drawTip = false
      }
   }

   spawnGroup(count) {
      count = count + _extraGoats
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
         if (_goatsDropMoney) {
            var x = (ent.x+_gridX)*_tw + _rnd.int(-8,8)
            var y = (ent.y+_gridY)*_th + _rnd.int(-8,8)
            if (_extraGoats == 0) {
               showTip(x*_scale-90, y*_scale-70, "NEW: Click coins to earn money!")
            }
            _coins.add(CoinButton.new(this, x, y))
         } else {
            _currencies[0] = _currencies[0] + _coinsPerKill
         }
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
      var mouse = Trap.mousePosition()

      _pauseMenu.pauseUpdate(dt)
      if (_pauseMenu.pauseClicked() || Trap.keyPressed(Button.Start, 0, -1)) {
         _paused = _paused ? false : true
      }

      if (_paused) {
         _pauseMenu.update(dt)

         var pauseAction = _pauseMenu.anyClicked()
         if (pauseAction == "menu") {
            _nextScene = "gameselect"
         } else if (pauseAction == "resume") {
            _paused = false
         } else if (pauseAction == "ending") {
            _nextScene = [_winScene, {"extraGoats": _extraGoats}]
         }

         return
      }

      _time = _time + dt

      _actionQueue.update(dt)

      _grid.update(dt)
      _pieceTray.update(dt)

      if (_checkForWin == true) {
         var left = _grid.entities.where{|e| e.type == "goat"}.count
         if (left == 0) {
            Timer.runLater(3) {
               _nextScene = [_winScene, {"extraGoats": _extraGoats}]
            }
            _checkForWin = false
         }
      }

      if (_coins.count > 0) {
         for (i in _coins.count-1..0) {
            _coins[i].update(dt, mouse[0] / _scale, mouse[1] / _scale)
            if (_coins[i].clicked(mouse[0] / _scale, mouse[1] / _scale)) {
               _coins.removeAt(i)
               _currencies[0] = _currencies[0] + _coinsPerKill
               SoundController.playOnce(_coinPickup)
            }
         }
      }

      if (_slots != null) {
         _slots.update(dt)
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

      for (coin in _coins) {
         coin.draw()
      }

      if (_slots !=  null) {
         _slots.draw()
      }

      if (_drawTip) {
         Draw.resetTransform()
         Draw.transform(h/720, 0, 0, h/720, 0, 0)
         
         Draw.setColor(Color.Fill, 0, 57, 113, 200)
         Draw.rect(_tipX, _tipY, 200, 65, false)
         Draw.setColor(Color.Fill, 255, 255, 255, 255)
         Draw.setTextStyle(_descriptionFont, 36, 0.85, Align.Center+Align.Top)
         Draw.text(_tipX, _tipY, 200, _tipText)
      }

      Draw.resetTransform()
      Draw.transform(h/720, 0, 0, h/720, 0, 0)

      if (_paused) {
         _pauseMenu.draw()
      }
      _pauseMenu.pauseDraw()

      Draw.submit()
   }

   shutdown() {
      SoundController.stopMusic()
   }
}