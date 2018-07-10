import "timer" for Timer
import "engine" for Draw, Asset, Trap, Color, Fill, Button, TileMap, CVar
import "math" for Math
import "Debug" for Debug
import "Goat" for Goat

class TD {
   nextScene { _nextScene }
   nextScene=(params) { _nextScene = params }

   time { _time }

   construct new(mapName) {
      _nextScene = null
      _time = 0

      // load the requested map
      TileMap.load(mapName)
      _layers = TileMap.layerNames()

      _mapProps = TileMap.getMapProperties()
      var gameMode = _mapProps["properties"]["gamemode"]

      _creeps = [
        Goat.new(this, 1, 12)
      ]

      _showPathsCVar = CVar.get("debug_pathfinding", false)

      // setup rules based on gamemode key
      // use a global static class here so other modules can easily access it?
      Debug.printLn("gamemode is %(gameMode)")

      Asset.loadAll()
   }

   getDistance(x, y) {
      return _paths[y * _mapProps["width"] + x]
   }

   generatePaths(x, y) {
      var mapWidth = _mapProps["width"]
      var mapHeight = _mapProps["height"]

      var frontier = [[x, y]]
      var dist = {}
      dist[y * mapWidth + x] = 0

      while (frontier.count > 0) {
         var current = frontier.removeAt(0)
         var curCoord = current[1] * mapWidth + current[0]

         if (current[0] > 0) {
            expandFrontier(frontier, dist, curCoord, current[0] - 1, current[1])
         }

         if (current[0] < mapWidth) {
            expandFrontier(frontier, dist, curCoord, current[0] + 1, current[1])
         }

         if (current[1] > 0) {
            expandFrontier(frontier, dist, curCoord, current[0], current[1] - 1)
         }

         if (current[1] < mapHeight) {
            expandFrontier(frontier, dist, curCoord, current[0], current[1] + 1)
         }
      }

      return dist
   }

   expandFrontier(frontier, dist, curCoord, nextX, nextY) {
      var next = nextY * _mapProps["width"] + nextX
      if (!dist.containsKey(next) && !isBlocked(nextX, nextY)) {
         frontier.insert(-1, [nextX, nextY])
         dist[next] = dist[curCoord] + 1
      }
   }

   isBlocked(x, y) {
      var wallLayer = TileMap.layerByName("walls")
      return TileMap.getTile(wallLayer, x, y) > 0
   }

   update(dt) {
      _time = _time + dt
      _paths = generatePaths(29, 12)

      for (creep in _creeps) {
        creep.update(dt)
      }
   }

   draw(w, h) {
      Draw.clear()
      Draw.resetTransform()
      Draw.transform(h / 180, 0, 0, h / 180, 0, 0)

      for (i in 0.._layers.count-1) {
         Draw.mapLayer(i)
      }

      for (creep in _creeps) {
        creep.draw()
      }

      if (_showPathsCVar.bool()) {
         Draw.setTextStyle(0, 6)
         for (x in 1.._mapProps["width"]) {
            for (y in 0.._mapProps["height"]) {
               var dist = _paths[y * _mapProps["width"] + x]
               if (dist != null) {
                  var a = 127 - (dist * 4)
                  Draw.text(x * 8, y * 8 + 6, 8, "%(dist)")
               }
            }
         }
      }

      Draw.submit()
   }

   shutdown() {

   }
}