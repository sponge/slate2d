import "engine" for Draw, Asset, Trap, Color, Fill, Button, TileMap, CVar
import "debug" for Debug
import "grid" for Grid
import "piecetray" for PieceTray
import "tower" for Tower

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

   construct new(mapName) {
      _nextScene = null
      _time = 0

      _scale = Trap.getResolution()[1] / 180
      _mapName = mapName

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

      // setup rules based on gamemode key
      var gameMode = _mapProps["properties"]["gamemode"]

      if (gameMode == 1) {
         // TODO: this image is loaded twice since the tmx also loads this but we can't use the
         // same name to load the existing texture because it's an image, and not a sprite
         // so we'll just dupe this image since whatever it's so small
         _spr = Asset.create(Asset.Sprite, "spr", "maps/tilesets/e1.png")
         Asset.spriteSet(_spr, 8, 8, 0, 0)
      }

      _grid = Grid.new(this, _gridX, _gridY, _gridW, _gridH, _tw, _th)
      _pieceTray = PieceTray.new(this, 272, 0, 48, 180)

      // FIXME: hardcoded
      _grid.setGoal(29, 12)
      _grid.setTower(12, 4, Tower.Fast)
      _grid.setTower(14, 4, Tower.Slow)
      _grid.setTower(16, 4, Tower.Fast)

      _grid.setTower(12, 8, Tower.Fast)
      _grid.setTower(14, 8, Tower.Slow)
      _grid.setTower(16, 8, Tower.Fast)

      Asset.loadAll()
   }

   gameOver() {
      _nextScene = ["gameover", {"map":_mapName}]
   }

   update(dt) {
      _time = _time + dt

      _grid.update(dt)
      _pieceTray.update(dt)
   }

   draw(w, h) {
      Draw.clear()
      Draw.resetTransform()
      _scale = h / 180
      Draw.transform(_scale, 0, 0, _scale, 0, 0)

      for (i in 0.._layers.count-1) {
         Draw.mapLayer(i)
      }

      _grid.draw()
      _pieceTray.draw()

      Draw.submit()
   }

   shutdown() {

   }
}