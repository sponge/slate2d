import "engine" for Draw, Asset, Trap, Color, Fill, Button, TileMap, CVar
import "math" for Math
import "entities/goat" for Goat
import "entities/coin" for Coin
import "tower" for Tower

class Grid {
   entities { _entities }
   goalX { _goalX }
   goalY { _goalY }

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
   getLocalMouse() {
      var mouse = Trap.mousePosition()
      mouse[0] = mouse[0] / _td.scale - (_x  * _tw)
      mouse[1] = mouse[1] / _td.scale - (_y * _th)
      return mouse 
   }

   setTower(x, y, type) {
     _towers.add(Tower.new(_td, x, y, type))
     _tiles[y*_w+x] = 200
     _tiles[y*_w+x+1] = 200
     _tiles[(y+1)*_w+x] = 200
     _tiles[(y+1)*_w+x+1] = 200
   }

   setWall(x, y, piece) {
      // FIXME: do it
   }

   update(dt) {
      generatePaths()

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

      // debug show how far each step is until the goal
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

      // if something is selected, draw the piece shadow
      // snap it to the nearest 8px boundary
      if (_td.pieceTray.activeTool != null) {
         var localMouse = getLocalMouse()
         var tx = localMouse[0] - (localMouse[0] % _tw)
         var ty = localMouse[1] - (localMouse[1] % _th)

         var button = _td.pieceTray.activeTool
         tx = tx - button.w/2
         ty = ty - button.h/2

         _td.pieceTray.drawTool(tx, ty, button.id)

         if (!isBlocked(tx / _tw, ty / _th) && Trap.keyPressed(Button.B, 0, -1)) {
            Trap.printLn("creating tower")
            setTower(tx / _tw, ty / _th, button.id)
         }
      }

      Draw.translate(0 - _x * _tw, 0 - _y * _th)
   }

   setGoal(x, y) {
      _entities = _entities.where {|e| e.type != "coin" }.toList
      _entities.add(Coin.new(_td, x, y))
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

      if (oob) {
         return true
      }

      var hasTile = _tiles[y * _w + x] != null
      var hasEntity = _entities.any {|ent| ent.x == x && ent.y == y }
      return hasTile || hasEntity
   }
}