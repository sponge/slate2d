import "engine" for Draw, Asset, TileMap, Trap, Button
import "math" for Math

import "player" for Player

class Game {
   nextScene { _nextScene }
   nextScene=(params) { _nextScene = params }

   entities { _entities }

   construct new(params) {
      _entities = []

      _entities.add(Player.new(this, {}, 120, 120, 16, 16))
   }

   update(dt) {
      for (ent in _entities) {
         ent.think(dt)
      }
   }

   draw(w, h) {
      Draw.clear()
      Draw.resetTransform()
      Draw.scale(h / 240)

      for (ent in _entities) {
         ent.draw()
      }
   }

   shutdown() {
      Asset.clearAll()
   }  
}