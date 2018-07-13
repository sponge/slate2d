import "engine" for Draw, Asset, Trap, Color, Fill, Button, TileMap, CVar
import "math" for Math
import "entities/goat" for Goat
import "tower" for Tower

class Grid {
  entities { _entities }

   construct new (td, x, y, w, h, tw, th) {
      _td = td
      _x = x
      _y = y
      _w = w
      _h = h
      _tw = tw
      _th = th

      _font = Asset.create(Asset.Font, "body", "fonts/Roboto-Regular.ttf")

      _showPathsCVar = CVar.get("debug_pathfinding", false)
      _tiles = {}
      _paths = {}
      _towers = []
      _entities = [
        Goat.new(td, this, 2, 6),
        Goat.new(td, this, 2, 5),
        Goat.new(td, this, 2, 4),
        Goat.new(td, this, 2, 3),
        Goat.new(td, this, 2, 2),
      ]

      // TEMP: copy the bushes out of the layer into the collision layer
      var wallLayer = TileMap.layerByName("walls")
      for (xx in 0..._w) {
         for (yy in 0..._h) {
            var t = TileMap.getTile(wallLayer, xx+_x, yy+_y) > 0
            if (t) {
               _tiles[yy*_w+xx] = t
            }
         }
      }
   }

   // needs to go from screen space -> local
   transformCoords(x, y) {
      
   }

   setTower(x, y, type) {
     _towers.add(Tower.new(_td, x, y, type))
   }

   update(dt) {
      generatePaths() // FIXME: should only do this when the map changes

      var creeps = _entities.where{|e| e.type == "goat"}

      for (tower in _towers) {
        tower.update(dt, creeps)
      }

      for (entity in _entities) {
         entity.update(dt)
      }
      _entities = _entities.where {|c| !c.dead }.toList
   }

   draw() {
      Draw.translate(_x * _tw, _y * _th)

      for (tower in _towers) {
        tower.draw()
      }

      for (entity in _entities) {
         entity.draw()
      }

      if (_showPathsCVar.bool()) {
         Draw.setColor(Color.Stroke, 255, 0, 0, 255)
         Draw.rect(0, 0, _w * _tw, _h * _th, Fill.Outline)
         Draw.setTextStyle(_font, 6)
         for (x in 0..._w) {
            for (y in 0..._h) {
               var dist = _paths[y * _w + x]
               if (dist != null) {
                  var a = 127 - (dist * 4)
                  Draw.text(x * _tw, y * _th + 6, 8, "%(dist)")
               }
            }
         }
      }
      Draw.translate(0 - _x * _tw, 0 - _y * _th)
   }

   setGoal(x, y) {
      _goalX = x
      _goalY = y
      generatePaths()
   }

   getDistance(x, y) {
      return _paths[y * _w + x]
   }

   generatePaths() {
      var frontier = [[_goalX, _goalY]]
      var dist = {}
      dist[_goalY * _w + _goalX] = 0

      while (frontier.count > 0) {
         var current = frontier.removeAt(0)
         var curCoord = current[1] * _w + current[0]

         if (current[0] > 0) {
            expandFrontier(frontier, dist, curCoord, current[0] - 1, current[1])
         }

         if (current[0] < _w) {
            expandFrontier(frontier, dist, curCoord, current[0] + 1, current[1])
         }

         if (current[1] > 0) {
            expandFrontier(frontier, dist, curCoord, current[0], current[1] - 1)
         }

         if (current[1] < _h) {
            expandFrontier(frontier, dist, curCoord, current[0], current[1] + 1)
         }
      }

      _paths = dist
   }

   expandFrontier(frontier, dist, curCoord, nextX, nextY) {
      var next = nextY * _w + nextX
      if (!dist.containsKey(next) && !isBlocked(nextX, nextY)) {
         frontier.insert(-1, [nextX, nextY])
         dist[next] = dist[curCoord] + 1
      }
   }

   isBlocked(x, y) {
      var oob = x < 0 || x >= _w || y < 0 || y >= _h

      if (!oob) {
         var hasTile = _tiles[y * _w + x] != null
         var hasEntity = _entities.any {|ent| ent.x == x && ent.y == y }
         var hasTower = _towers.any {|ent|(ent.x == x || ent.x + 1== x ) && (ent.y == y || ent.y + 1 == y) }
         return hasTile || hasEntity || hasTower
      }

      return true
   }
}