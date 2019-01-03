import "engine" for Draw, Trap, Asset, Align, Button
import "soundcontroller" for SoundController

class Game5Title {
   nextScene { _nextScene }
   nextScene=(s) { _nextScene = s } 


   construct new(params) {
      _bodyFont = Asset.create(Asset.Font, "raleway", "fonts/Raleway-ExtraLight.ttf")
      _ripple = 0
      _nextSceneTimer = null
      _fadeTime = 4
      _fadeIn = 0

      _sonar = Asset.create(Asset.Sound, "bgm", "sound/sonar.ogg")
      Asset.loadAll()
   }

   update(dt) {
      if (Trap.keyPressed(Button.Start, 0, -1)) {
         _nextScene = "gameselect"
      }

      if (_fadeIn < 255) {
         _fadeIn = _fadeIn + 1
         _fadeIn = _fadeIn > 255 ? 255 :_fadeIn 
      }
      
      _ripple = _ripple + 2
      if (_ripple >= 255) {
         _ripple = 0
         SoundController.playOnce(_sonar, 0.5, 0, false)
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
      SoundController.stopAsset(_sonar)
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

      Draw.setColor(255, 255, 255, 255-_ripple)
      Draw.line(x - rippleX, waveY, x + w, waveY + rippleY)
      Draw.line(x - rippleX, waveY, x + w, waveY - rippleY)
      Draw.line(x + (w * 2) + rippleX, waveY, x + w, waveY + rippleY)
      Draw.line(x + (w * 2) + rippleX, waveY, x + w, waveY - rippleY)

      Draw.setColor(132, 132, 132, 255)
      var o = drawSkewedRect(x, y, w, h, -Num.pi/8)

      Draw.setColor(192, 192, 192, 255)
      drawSkewedRect(x + w, y, w, h, Num.pi/8)

      Draw.setColor(227, 227, 227, 255)
      Draw.tri(x, y, x + w, y - o, x + w, y + o, false)
      Draw.tri(x+(w*2), y, x + w, y - o, x + w, y + o, false)

      Draw.setTextStyle(_bodyFont, 48, 1.0, Align.Center|Align.Top)
      Draw.text(0, 600, 1280, "towers")
      Draw.setTextStyle(_bodyFont, 32, 1.0, Align.Center|Align.Top)
      Draw.text(0, 650, 1280, "by clive")
      Draw.setColor(255, 255, 255, 255)

      Draw.setTextStyle(_bodyFont, 48, 1.0, Align.Center|Align.Top)

      if (_nextSceneTimer != null) {
         var alpha = (1 - (_nextSceneTimer/_fadeTime)) * 255
         alpha = alpha > 255 ? 255 : alpha
         Draw.setColor(0, 0, 0, _nextSceneTimer > 0 ? alpha : 255)
         Draw.rect(0, 0, 1280, 720, false)
         Draw.setTextStyle(_bodyFont, 48, 1.0, Align.Center|Align.Middle)

         var msgAlpha =  ((_nextSceneTimer/_fadeTime) * Num.pi).sin * 255
         msgAlpha = msgAlpha < 0 ? 0 : msgAlpha
         Draw.setColor(255, 255, 255, msgAlpha) 
         Draw.text(0, 360, 1280, "will you reach the top?")
      }

      Draw.setColor(0, 0, 0, 255-_fadeIn)
      Draw.rect(0, 0, 1280, 720, false)
   }
}