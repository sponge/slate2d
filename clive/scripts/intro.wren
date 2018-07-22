import "timer" for Timer
import "engine" for Draw, Asset, Trap, Color, Fill, Button
import "debug" for Debug

class Intro {
   nextScene { _nextScene }
   nextScene=(params) { _nextScene = params }

   construct new(params) {
      _nextScene = null
      _time = 0

      _font = Asset.create(Asset.BitmapFont, "buttonfont", "gfx/panicbomber_blue.png")
      Asset.bmpfntSet(_font, " !\"#$\%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~", 8, 0, 8, 8)

      Asset.loadAll()
   }

   update(dt) {
      _time = _time + dt

      if (Trap.keyPressed(Button.B, 0, -1)) {
         nextScene = "gameselect"
      }
   }

   draw(w, h) {
      Draw.clear()
      Draw.resetTransform()
      Draw.transform(h / 240, 0, 0, h / 240, 0, 0)

      Draw.setColor(Color.Stroke, 255, 0, 0, 255)
      Draw.rect(160 + _time.sin * 80, 80, 16, 16, Fill.Outline)

      Draw.bmpText(_font, 20, 65, "Welcome to the")
      Draw.bmpText(_font, 20, 80, "Clive Sturridge's Battlement Defense")
      Draw.bmpText(_font, 20, 90, "Legacy Collection")
      Draw.bmpText(_font, 130, 120, "Click to begin")

      Draw.submit()
   }

   shutdown() {

   }
}