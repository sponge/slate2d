import "engine" for Draw, Asset, Trap, Color, Fill, Button
import "debug" for Debug
import "math" for Math

class UIButton {
   x { _x }
   x=(val) { _x = val }
   y { _y }
   y=(val) { _y = val }
   w { _w }
   w=(val) { _w = val }
   h { _h }
   h=(val) { _h = val }
   name { _name }
   name=(val) { _name = val }
   id { _id }
   hover { _hover }

   construct new(id, x, y, w, h, name) {
      _id = id
      _x = x
      _y = y
      _w = w
      _h = h
      _name = name

      _hover = false
   }

   update(dt, mx, my) {
      _hover = Math.pointInRect(mx, my, x, y, w, h)
   }

   draw() {

   }

   clicked(mx, my) {
      return Trap.keyPressed(Button.B, 0, -1) && Math.pointInRect(mx, my, x, y, w, h)
   }
}

class GameSelectButton is UIButton {
   construct new(id, x, y, w, h, name, img) {
      super(id, x, y, w, h, name)
      _imgHnd = Asset.create(Asset.Image, img, img)
      _imgW = 192
      _imgH = 300

      _font = Asset.create(Asset.BitmapFont, "buttonfont", "gfx/panicbomber_blue.png")
      Asset.bmpfntSet(_font, " !\"#$\%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~", 8, 0, 8, 8)
   }

   draw() {
      if (hover) {
         Draw.setColor(Color.Stroke, 255, 255, 0, 255)
         Draw.rect(x, y, w, h, Fill.Outline)
      }
      Draw.image(_imgHnd, x + (_imgW / 2), y+10, _imgW, _imgH)
      var textw = Asset.measureBmpText(_font, name) * 2
      Draw.bmpText(_font, x+(w-textw)/2, y+_imgH+20, name, 2)
   }
}