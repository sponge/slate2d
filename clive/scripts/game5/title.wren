import "engine" for Draw, Color, Trap, Asset, Align

class Game5Title {
   nextScene { null }
   construct new(params) {
      _bodyFont = Asset.create(Asset.Font, "body", "fonts/Roboto-Regular.ttf")
      _ripple = 0
      Asset.loadAll()
   }

   update(dt) {
      _ripple = _ripple + 3
      if (_ripple >= 255) {
         _ripple = 0
      }
   }

   shutdown() {
   }

   drawSkewedRect(x, y, w, h, skew) {

      var o = skew.tan * w
      if (o <= 0) {
         Draw.tri(x, y, x+w, y-o, x, y-o, false)
         Draw.tri(x, y+h, x+w, (y+h), x+w, (y+h)-o, false)
         Draw.rect(x, y+o.abs, w, h-o.abs, false)
      } else {
         y = y + o
         Draw.tri(x, y, x+w, y-o, x+w, y, false)
         Draw.tri(x, y+h, x+w, y+(h-o), x, y+(h-o), false)
         Draw.rect(x, y.abs, w, h-o.abs, false)
      }
      // Draw.tri(x, y, x+w, y, x+w, y-o, false)
      return o
   }

   draw(ww, hh) {
      var w = 100
      var h = 360
      var x = (1280/2)-w
      var y = (720/2)-(h/2)

      var rippleX = _ripple
      var rippleY = (Num.pi/8).tan * (w + rippleX)

      var waveY = y + h

      Draw.setColor(Color.Stroke, 255, 255, 255, 255-_ripple)
      Draw.line(x - rippleX, waveY, x + w, waveY + rippleY)
      Draw.line(x - rippleX, waveY, x + w, waveY - rippleY)
      Draw.line(x + (w * 2) + rippleX, waveY, x + w, waveY + rippleY)
      Draw.line(x + (w * 2) + rippleX, waveY, x + w, waveY - rippleY)

      Draw.setColor(Color.Fill, 132, 132, 132, 255)
      var o = drawSkewedRect(x, y, w, h, -Num.pi/8)

      Draw.setColor(Color.Fill, 192, 192, 192, 255)
      drawSkewedRect(x + w, y, w, h, Num.pi/8)

      Draw.setColor(Color.Fill, 227, 227, 227, 255)
      Draw.tri(x, y, x + w, y - o, x + w, y + o, false)
      Draw.tri(x+(w*2), y, x + w, y - o, x + w, y + o, false)

      Draw.setTextStyle(_bodyFont, 48, 1.0, Align.Center|Align.Top)
      Draw.text(0, 600, 1280, "towers")
   }
}