import "engine" for Draw, Asset
import "entity" for Entity
import "random" for Random

class MineText is Entity {
   construct new(spr, x, y) {
      _rnd = Random.new()
      _text = _rnd.sample(MineText.Text[spr]["sentences"])
      _x = x
      _y = y

      _font = Asset.find("font")

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
         "sentences": ["nigel was right!!", "hard brexit is the only brexit"],
      },
      {
         "name": "Money",
         "sentences": ["i'll pay you back in january", "hey you can help me out a bit right?"],
      },
      {
         "name": "Marriage",
         "sentences": ["so have you met anyone nice yet?", "when you gonna put a ring on it?"],
      },
      {
         "name": "Football",
         "sentences": ["football!!!", "we gotta watch the game!", "we gotta stay up until midnight to watch the game!"],
      },
      {
         "name": "Also Football",
         "sentences": ["also football!!!", "we gotta watch the game!", "we gotta wake up at 6am to watch the game!"],
      },
      {
         "name": "Aliens",
         "sentences": ["where you think velcro came from??", "project blue beam is happening soon", "fuck nasa"],
      },
      {
         "name": "Ugly Baby",
         "sentences": ["he's got his fathers unibrow", "what an ugly baby", "when ya havin kids?"],
      },
      {
         "name": "The Wall",
         "sentences": ["we're gonna build that wall", "gotta bring those jobs back"],
      },
      {
         "name": "MLM",
         "sentences": ["hashtag be your own boss", "it's only $2000 for the starter kit!", "come to my beauty party!"],
      },
   ]}
}