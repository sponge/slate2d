import "engine" for Asset, Draw

class Ending {
   nextScene { _nextScene }
   nextScene=(params) { _nextScene = params }

   construct new(params) {
      _bigFont = Asset.create(Asset.BitmapFont, "sneakattack", "gfx/sneak-attack-bitmap.png")
      Asset.bmpfntSet(_bigFont, "abcdefghijklmnopqrstuvwxyz'", 0, 1, 2, 5)
      Asset.loadAll()
   }

   update(dt) {
   }

   draw(w, h) {
      Draw.clear()
      Draw.resetTransform()
      Draw.scale(h / 180)

      Draw.bmpText(_bigFont, 10, 10, "dis game ova")
   }

   shutdown() {
      Asset.clearAll()
   }
}