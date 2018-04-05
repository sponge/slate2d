import "engine" for Trap, Button, Draw, Scene, Asset, Fill, Color, TileMap

class Game is Scene {
  construct new(mapName) {
    TileMap.load("maps/map.tmx")

    _sprites = Asset.create(Asset.Image, "sprites", "gfx/ghosts.png")
    _font = Asset.create(Asset.BitmapFont, "font", "gfx/good_neighbors.png")
    Asset.bmpfntSet(_font, "!\"#$\%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~", -1, 7, 16)

    Asset.loadAll()

    _bgLayer = TileMap.layerByName("bg")
    _collideLayer = TileMap.layerByName("collide")
    _spr = Asset.createSprite(_sprites, 8, 8, 0, 0)
    _mapObj = TileMap.objectsInLayer(TileMap.layerByName("objects"))

    Trap.printLn(_mapObj)

    _player = {
      "x": 100,
      "y": 100,
    }
    _playerMoveSpeed = 1

    _currentDialog = null
  }

  update(dt) {
    var repeatSpeed = 33
    var newPlayerX = _player["x"]
    var newPlayerY = _player["y"]

    if (Trap.keyPressed(Button.Down, 0, repeatSpeed)) {
      newPlayerY = newPlayerY + _playerMoveSpeed
    }

    if (Trap.keyPressed(Button.Up, 0, repeatSpeed)) {
      newPlayerY = newPlayerY - _playerMoveSpeed
    }

    if (Trap.keyPressed(Button.Right, 0, repeatSpeed)) {
      newPlayerX = newPlayerX + _playerMoveSpeed
    }

    if (Trap.keyPressed(Button.Left, 0, repeatSpeed)) {
      newPlayerX = newPlayerX - _playerMoveSpeed
    }

    var playerTileX = newPlayerX / 8
    var playerTileY = newPlayerY / 8

    var tile = TileMap.getTile(_collideLayer, playerTileX, playerTileY)
    if (tile > 0) {
      // Trying to move into a tile, reset the move
      newPlayerX = _player["x"]
      newPlayerY = _player["y"]
      playerTileX = newPlayerX / 8
      playerTileY = newPlayerY / 8
    }

    _player["x"] = newPlayerX
    _player["y"] = newPlayerY

    // See if we're in any dialog
    var objs = _mapObj.where {|obj|
      var inX = _player["x"] > obj["x"] && _player["x"] < obj["x"] + obj["width"]
      var inY = _player["y"] > obj["y"] && _player["y"] < obj["y"] + obj["height"]
      return inX && inY
    }

    if (objs.count > 0) {
      _currentDialog = objs.toList[0]
    } else {
      _currentDialog = null
    }
  }

  draw(w, h) {
    // Draw.setColor(Color.Fill, 255, 0, 0, 255)
    Draw.clear()
    // Draw.rect(-1, 0, w, h, false)
    Draw.setTransform(
      h / 160, 0, 0,
      h / 160, 0, 0,
      true)

      Draw.mapLayer(_bgLayer)
      Draw.mapLayer(_collideLayer)

      Draw.sprite(_spr, 0, _player["x"], _player["y"])

      if (_currentDialog != null) {
        Draw.bmpText(_font, _player["x"], _player["y"] - 20, _currentDialog["properties"]["text"])
      }

      Draw.submit()
    }

    shutdown() {
      TileMap.free()
      Asset.clearAll()
    }
  }

  class Main {
    static scene { __scene }

    static init(mapName) {
      __scene = Game.new(mapName)
    }

    static update(dt) {
      __scene.update(dt)
    }

    static draw(w, h) {
      __scene.draw(w, h)
    }

    static shutdown() {
      __scene.shutdown()
      __scene = null
    }
  }
