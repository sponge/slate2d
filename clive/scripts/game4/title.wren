import "timer" for Timer
import "engine" for Draw, Asset, Trap, Color, Fill, Button, Align
import "math" for Math
import "soundcontroller" for SoundController
import "actionqueue" for ActionQueue

class FadeThing {
   construct new(x, y, thing) {
      _x = x
      _y = y
      _thing = thing
      _fadeTime = 0
      _fadeOut = false
      _alpha = 0
      _active = false
      _time = 0
      _textStyle = []
      _type = Asset.Image
   }

   isA(t) {
      _type = t

      if (t == Asset.Image) {
         var sz = Asset.imageSize(_thing)
         _w = sz[0]
         _h = sz[1]
      }
      return this
   }
   
   setTextWidth(w) {
      _w = w
      return this
   }

   setTextStyle(fntId, size, lineHeight, align) {
      _textStyle = [fntId, size, lineHeight, align]
      return this
   }

   fadeIn(t) {
      _fadeTime = t
      _active = true
      _alpha = 0
      _time = 0
   }

   fadeOut(t) {
      _fadeTime = t
      _active = true
      _fadeOut = true
      _alpha = 1
      _time = 0
   }

   hide() {
      _active = false
      _alpha = 0
   }

   update(dt) {
      if (_active == false) {
         return
      }

      _time = _time + dt
      // self owned by inconsistent alpha values
      if (_thing is String) {
         _alpha = _fadeOut ? Math.lerp(255, 0, _time / _fadeTime) : Math.lerp(0, 255, _time / _fadeTime)
         _alpha = Math.clamp(0, _alpha, 255)
      } else if (_type == Asset.Image) {
         _alpha = _fadeOut ? Math.lerp(1, 0, _time / _fadeTime) : Math.lerp(0, 1, _time / _fadeTime)
         _alpha = Math.clamp(0, _alpha, 1)
      }
   }

   draw() {
      if (_alpha == 0) {
         return
      }

      if (_thing is String) {
         Draw.setTextStyle(_textStyle[0], _textStyle[1], _textStyle[2], _textStyle[3])
         Draw.setColor(Color.Fill, 255, 255, 255, _alpha)
         Draw.text(_x, _y, _w, _thing)
      } else if (_type == Asset.Image) {
         Draw.image(_thing, _x, _y, _w, _h, _alpha)
      }
   }
}

class Game4Title {
   nextScene { _nextScene }
   nextScene=(params) { _nextScene = params }

   construct new(params) {
      _nextScene = null
      _time = 0

      _music = Asset.create(Asset.Sound, "game4_title", "sound/game4_title.ogg")
      _font = Asset.create(Asset.Font, "body", "fonts/Roboto-Regular.ttf")
      _ks = Asset.create(Asset.Image, "ks", "gfx/game4/ks.png")

      Asset.loadAll()

      _things = {
         "presents": FadeThing.new(0, 310, "Clive Sturridge presents").setTextWidth(1280).setTextStyle(_font, 48, 1.0, Align.Center+Align.Top),
         "agame": FadeThing.new(0, 310, "A Clive Sturridge Game").setTextWidth(640).setTextStyle(_font, 48, 1.0, Align.Center+Align.Top),
         "byyou": FadeThing.new(0, 360, "...funded by you!").setTextWidth(640).setTextStyle(_font, 48, 1.0, Align.Center+Align.Top),
         "ksimg": FadeThing.new(640, 280, _ks).isA(Asset.Image)
      }

      _actions = [
         [2, Fn.new {
            SoundController.playMusic(_music)
            _things["presents"].fadeIn(2)
         }],
         [3, Fn.new { _things["presents"].fadeOut(2) }],
         [2, Fn.new { _things["agame"].fadeIn(2) }],
         [2, Fn.new { _things["byyou"].fadeIn(2) }],
         [2, Fn.new { _things["ksimg"].fadeIn(2) }],

      ]
      _actionQueue = ActionQueue.new(_actions)
   }

   update(dt) {
      _time = _time + dt
      _actionQueue.update(dt)

      for (k in _things.keys) {
         _things[k].update(dt)
      }

      if (_time > 3 && Trap.keyPressed(Button.B, 0, -1)) {
         _nextScene = ["td", "maps/e4m1.tmx"]
      }
   }

   draw(w, h) {
      Draw.clear()
      Draw.resetTransform()
      Draw.transform(h / 720, 0, 0, h / 720, 0, 0)

      Draw.setColor(Color.Fill, 255, 255, 255, 255)
      Draw.setTextStyle(_font, 48, 1.0, Align.Left+Align.Top)

      for (k in _things.keys) {
         _things[k].draw()
      }

      Draw.submit()
   }

   shutdown() {

   }
}