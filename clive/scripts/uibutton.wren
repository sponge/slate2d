import "engine" for Draw, Asset, Trap, Color, Fill, Button, ImageFlags, Align
import "debug" for Debug
import "math" for Math
import "soundcontroller" for SoundController
import "fonts" for Fonts

class UIButton {
   x { _x }
   x=(val) { _x = val }
   y { _y }
   y=(val) { _y = val }
   w { _w }
   w=(val) { _w = val }
   h { _h }
   h=(val) { _h = val }
   id { _id }
   hover { _hover }

   construct new(id, x, y, w, h) {
      _id = id
      _x = x
      _y = y
      _w = w
      _h = h

      _hover = false
   }

   update(dt, mx, my) {
      _hover = Math.pointInRect(mx, my, x, y, w, h)
   }

   draw() {
      // Draw.setColor(Color.Stroke, 255, 0, 0, 255)
      // Draw.rect(_x, _y, _w, _h, Fill.Outline)
   }

   clicked(mx, my) {
      return Math.pointInRect(mx, my, x, y, w, h) && Trap.keyPressed(Button.B, 0, -1)
   }
}

class TrayButton is UIButton {
   category { _category }
   variation { _variation }
   
   construct new(id, category, variation, x, y, w, h) {
      super(id, x, y, w, h)
      _category = category
      _variation = variation
   }
}

class TextButton is UIButton {
   construct new(id, x, y, w, h, label) {
      super(id, x, y, w, h)
      _label = label

      _font = Asset.create(Asset.Font, "body", Fonts.body)
      _click = Asset.create(Asset.Sound, "menu_click", "sound/menu_click.ogg")
   }

   clicked(mx, my) {
      var result = super(mx, my)
      if (result) {
         SoundController.playOnce(_click)
      }

      return result
   }

   draw() {
      if (hover) {
         Draw.setColor(Color.Fill, 101, 157, 214, 255)
      } else {
         Draw.setColor(Color.Fill, 47, 112, 176, 255)
      }

      Draw.rect(x, y, w, h, Fill.Solid)
      Draw.setColor(Color.Fill, 255, 255, 255, 255)
      Draw.setTextStyle(_font, 36, 1.0, Align.Center+Align.Top)
      Draw.text(x, y+2, w, _label)
   }
}

class GameSelectButton is UIButton {
   label { _label }

   construct new(id, x, y, w, h, label, img) {
      super(id, x, y, w, h)
      _imgHnd = Asset.create(Asset.Image, img, img, ImageFlags.LinearFilter)
      _imgW = 385
      _imgH = 600

      _scale = 1 
      _targetScale = 1

      _label = label
   }

   update(dt, mx, my) {
      super.update(dt, mx, my)
      _scale = Math.lerp(_scale, _targetScale, 0.2)
   }

   draw() {
      if (hover) {
         _targetScale = 1.25
      } else {
         _targetScale = 1
      }

      var scale = _scale * 0.5
      var width = _imgW * scale
      var height = _imgH * scale
      Draw.image(_imgHnd, x + ((w - width) / 2), y+10, width * (1/scale), height * (1/scale), 1.0, scale)
   }
}

class CoinButton is UIButton {
   construct new(td, x, y) {
      super(id, x, y, td.tw / 2, td.th / 2)

      _td = td
      _time = 0
   }

   update(dt, mx, my) {
      super.update(dt, mx, my)
      _time = _time + dt
   }

   draw() {
      // Draw.setColor(Color.Fill, 255, 0, 0, 255)
      // Draw.rect(x, y, w, h, Fill.Solid)

      if ((_time * 4).floor % 2 == 0) {
         Draw.setColor(Color.Fill, 255, 255, 0, 255)
      } else {
         Draw.setColor(Color.Fill, 255, 128, 0, 255)
      }
      Draw.circle(x+w/2, y+h/2, 2, Fill.Solid)
   }   
}