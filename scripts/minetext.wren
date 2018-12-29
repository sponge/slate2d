import "engine" for Draw, Asset
import "entity" for Entity
import "random" for Random

class MineText is Entity {
   construct new(spr, x, y) {
      _rnd = Random.new()
      _text = _rnd.sample(MineText.Text[spr]["sentences"])
      _x = x
      _y = y

      _font = Asset.create(Asset.BitmapFont, "font", "gfx/font.png")
      Asset.bmpfntSet(_font, "abcdefghijklmnopqrstuvwxyz!?'$1234567890", 0, 1, 2, 5)
      Asset.loadAll()

      _bright = true
   }

   think(dt) {
      _y = _y - dt

      if (_y < -16) {
         die()
      }

      if (_y % 12 == 0) {
         _bright = !_bright
      }
   }

   draw() {
      // outline
      Draw.setColor(27, 38, 50, 255)
      Draw.bmpText(_font, _x, _y + 1, _text)
      Draw.bmpText(_font, _x, _y - 1, _text)
      Draw.bmpText(_font, _x + 1, _y, _text)
      Draw.bmpText(_font, _x - 1, _y, _text)

      if (_bright) {
         Draw.setColor(247, 226, 107, 255)
      } else {
         Draw.setColor(235, 137, 49, 255)
      }
      Draw.bmpText(_font, _x, _y, _text)
      Draw.setColor(255, 255, 255, 255)
   }

   static Text {[
      {
         "name": "Brexit",
         "sentences": ["nigel was right!!"],
      },
      {
         "name": "Money",
         "sentences": ["i'll pay you back in january"],
      },
      {
         "name": "Marriage",
         "sentences": ["marriage"],
      },
      {
         "name": "Football",
         "sentences": ["football"],
      },
      {
         "name": "Also Football",
         "sentences": ["also football"],
      },
      {
         "name": "Aliens",
         "sentences": ["where you think velcro came from??"],
      },
      {
         "name": "Ugly Baby",
         "sentences": ["he's got his fathers unibrow"],
      },
      {
         "name": "The Wall",
         "sentences": ["the wall"],
      },
      {
         "name": "MLM",
         "sentences": ["it's only $2000 for the starter kit!"],
      },
   ]}
}