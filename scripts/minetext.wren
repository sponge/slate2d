import "engine" for Draw, Asset
import "entity" for Entity
import "random" for Random

class MineText is Entity {
   construct new(spr, x, y) {
      _rnd = Random.new()
      _text = _rnd.sample(MineText.Text[spr]["sentences"])
      _x = x
      _y = y

      _font = Asset.create(Asset.BitmapFont, "font", "gfx/good_neighbors.png")
      Asset.bmpfntSet(_font, "!\"#$\%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~", 0, -1, 7, 16)
      // _font = Asset.create(Asset.Font, "roboto", "fonts/Sneak Attack.ttf")
      Asset.loadAll()
   }

   think(dt) {
      _y = _y - (dt)
      if (_y < -16) {
         die()
      }
   }

   draw() {
      // Draw.setTextStyle(_font, 1)
      // Draw.setColor(255, 255, 255, 255)
      // Draw.text(1, 1, 100, _text)
      Draw.bmpText(_font, _x, _y, _text)
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
   ]}
}