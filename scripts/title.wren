import "engine" for Draw, Asset, TileMap, Trap, Button
import "math" for Math

class Title {
   nextScene { _nextScene }
   nextScene=(params) { _nextScene = params }

   construct new(params) {
      _font = Asset.create(Asset.BitmapFont, "font", "gfx/good_neighbors.png")
      Asset.bmpfntSet(_font, "!\"#$\%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~", 0, -1, 7, 16)

      Asset.loadAll()
   }

   update(dt) {
      if (Trap.keyPressed(Button.Start, 0, -1)) {
         nextScene = ["cutscene", "game"]
      }
   }

   draw(w, h) {
      Draw.clear()
      Draw.resetTransform()
      Draw.scale(h / 240)

      Draw.bmpText(_font, 150, 80, "Not Balloon Fight")
      Draw.bmpText(_font, 140, 120, "Press Start to begin")
   }

   shutdown() {
      Asset.clearAll()
   }  
}