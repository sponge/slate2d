import "timer" for Timer
import "engine" for Draw, Asset, Trap, Color, Fill, Button, TileMap
import "math" for Math
import "Debug" for Debug

class TD {
   nextScene { _nextScene }
   nextScene=(params) { _nextScene = params }

   construct new(mapName) {
      _nextScene = null
      _time = 0

      // load the requested map
      TileMap.load(mapName)
      _layers = TileMap.layerNames()

      var mapProps = TileMap.getMapProperties()
      var gameMode = mapProps["properties"]["gamemode"]

      // setup rules based on gamemode key
      // use a global static class here so other modules can easily access it?
      Debug.printLn("gamemode is %(gameMode)")

      Asset.loadAll()
   }

   update(dt) {
      _time = _time + dt
   }

   draw(w, h) {
      Draw.clear()
      Draw.resetTransform()
      Draw.transform(h / 180, 0, 0, h / 180, 0, 0)

      for (i in 0.._layers.count-1) {
         Draw.mapLayer(i)
      }

      Draw.submit()
   }

   shutdown() {

   }
}