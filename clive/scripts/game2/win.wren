import "engine" for Draw, Trap, Asset, Color, Button, Align
import "random" for Random

class Game2Win {
   nextScene { _nextScene }

   construct new(params) {
      Asset.clearAll()
      
      _nextScene = null
      _time = 0
      _rnd = Random.new()
      _nextGoatTime = 0

      _goatSpr = Asset.create(Asset.Sprite, "endinggoat", "gfx/game1/goat.png")
      Asset.spriteSet(_goatSpr, 16, 16, 0, 0)

      _spr = Asset.create(Asset.Sprite, "endingspr", "maps/tilesets/e2.png")
      Asset.spriteSet(_spr, 8, 8, 0, 0)

      _font = Asset.create(Asset.BitmapFont, "buttonfont", "gfx/panicbomber_blue.png")
      Asset.bmpfntSet(_font, " !\"#$\%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~", 8, 0, 8, 8)

      _goats = [
         [640, _rnd.int(64, 100)]
      ]

      Asset.loadAll()
   }

   drawCenteredText(font, x, y, str, scale) {
      var width = Asset.measureBmpText(font, str) * scale
      Draw.bmpText(font, x - width/2, y, str, scale)
   }

   shutdown() {
   }

   update(dt) {
      _time = _time + dt

      if (Trap.keyPressed(Button.B, 0, -1) || Trap.keyPressed(Button.Start, 0, -1)) {
         _nextScene = "gameselect"
      }

      if (_time >= _nextGoatTime) {
         _goats.add([640, _rnd.int(70, 120)])
         _nextGoatTime = _nextGoatTime + 0.25
      }

      for (i in _goats.count-1..0) {
         var goat = _goats[i]
         goat[0] = goat[0] - 1

         if (goat[0] < -16) {
            _goats.removeAt(i)
         }
      }
   }

   draw(w, h) {
      Draw.clear()
      Draw.transform(h/360, 0, 0, h/360, 0, 0)

      drawCenteredText(_font, 320, 16, "THE POUND IS SAVED!", 2.0)
      drawCenteredText(_font, 320, 32, "THE GOATS ARE ON THE RUN!", 2.0)
      drawCenteredText(_font, 320, 160, "Reasonable Software Is:", 1.0)
       drawCenteredText(_font, 320, 178, "Clive Sturridge", 1.0)

      Draw.bmpText(_font, 100, 204, "Program: Pip Torrens
         Royston Munt

Art: Giles Oldershaw
     Rosalie Crutchley
", 1.0)
      Draw.bmpText(_font, 350, 204, "Manager: Dermot Crowley
Composer: Antonia Pemberton
Scenario: Vincent Brimble
Tester: Nigel Humphreys
", 1.0)

      drawCenteredText(_font, 320, 260, "Thanks to Cyril for all the toast water!", 1.0)

      drawCenteredText(_font, 320, 350, "CRACKED BY ALLiGATOR 1988", 1.0)

      for (goat in _goats) {
         Draw.sprite(_goatSpr, 0, goat[0], goat[1], 1.0, 1, 1)
      }

      var frame = (_time * 8% 4).floor
      Draw.sprite(_spr, 8 + (frame * 2), 550, 70, 1.0, 4, 0, 2, 2)

      Draw.submit()
   }
}