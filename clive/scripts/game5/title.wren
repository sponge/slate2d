import "engine" for Draw, Color, Trap, Asset, Align, Button

class Game5Title {
   nextScene { _nextScene }
   nextScene=(s) { _nextScene = s } 


   construct new(params) {
      _bodyFont = Asset.create(Asset.Font, "raleway", "fonts/Raleway-ExtraLight.ttf")
      _ripple = 0
      _nextSceneTimer = null
      _fadeTime = 4
      Asset.loadAll()
   }

   update(dt) {
      if (Trap.keyPressed(Button.Start, 0, -1)) {
         _nextScene = "gameselect"
      }
      
      _ripple = _ripple + 3
      if (_ripple >= 255) {
         _ripple = 0
      }

      if (Trap.keyPressed(Button.B, 0, -1) && _nextSceneTimer == null) {
         _nextSceneTimer = _fadeTime
      } else if (_nextSceneTimer != null && _nextSceneTimer <= 0) {
         _nextScene = ["towers", ""]
      } else if (_nextSceneTimer != null) {
         _nextSceneTimer = _nextSceneTimer - dt
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

      if (_nextSceneTimer != null) {
         var alpha = (1 - (_nextSceneTimer/_fadeTime)) * 255
         alpha = alpha > 255 ? 255 : alpha
         Draw.setColor(Color.Fill, 0, 0, 0, _nextSceneTimer > 0 ? alpha : 255)
         Draw.rect(0, 0, 1280, 720, false)
         Draw.setTextStyle(_bodyFont, 48, 1.0, Align.Center|Align.Middle)

         var msgAlpha =  ((_nextSceneTimer/_fadeTime) * Num.pi).sin * 255
         msgAlpha = msgAlpha < 0 ? 0 : msgAlpha
         Draw.setColor(Color.Fill, 255, 255, 255, msgAlpha) 
         Draw.text(0, 360, 1280, "will you reach the top?")
      }
   }
}