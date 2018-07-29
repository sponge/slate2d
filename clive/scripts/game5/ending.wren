import "engine" for Asset, Trap, Draw, Align, Color, Button
import "soundcontroller" for SoundController

class TowersEnding {
   nextScene { _nextScene }
   nextScene=(s) { _nextScene = s }

   construct new(params) {
      _audio = Asset.create(Asset.Sound, "towers_ending", "sound/towers_ending.ogg")
      _bg = Asset.create(Asset.Image, "towers_ending_bg", "gfx/game5/ending.png")

      _text = "Hello, This is Clive speaking. I can't express just how much pleasure it brings me, to see so many tens of dozens of people trying to climb my tower. Whether it's the diehard goatheads and battlement defencers, or the random man on the street who just hears some hubbub in his local Wimpy, about my tower, and decides to have a go for himself.
 
Now, you, young man, or woman, appear to be the person who climbed the tower first. Now, either you've clicked more than anyone else or, perhaps you were lucky and joined right at the end, either way, good job. Or, as we sometimes say, \"top banana!\"
 
Now, your prize is not a physical object, although it is very valuable. It's not something that I can send you, no, in fact, you're going to have to send it to me, and by it, I mean you. That's right, you'll have to get on a train, or a boat, or a car, or possibly even a plane. Perhaps a bicycle would suffice, if you are within the local area, of course.
 
Now, what your prize is, is, you will come to my studio and for 25 minutes you will be able to pitch any idea you would like, to none other than my personal assistant, Jeremy, at which point he will write it down, fax it to me (because I have of course, since moved to Spain) at which point I will fax it back and tell him \"yes\" or \"no\" and then we will move forward into production. Now, of course, I will own any ideas that you happen to mention to Jeremy but, I do promise, that if we like your idea, you will get a special thanks somewhere near the bottom of the credits and perhaps at the back of the manual, underneath all the copyright and open source licenses.
 
\"Thank you\" very much for reaching the top of my tower. I hope to hear from Jeremy quite soon.
 
This is Clive, signing off.
Make sure your idea's a \"goat\" one!

Sir Clive Sturridge, 2018"
      _bodyFont = Asset.create(Asset.Font, "raleway", "fonts/Raleway-ExtraLight.ttf")
      _textY = 720

      Asset.loadAll()

      SoundController.playOnce(_audio, 1.25, 0, false)
   }

   update(dt) {
      if (Trap.keyPressed(Button.Start, 0, -1)) {
         _nextScene = "gameselect"
      }

      _textY = _textY - 0.15

      if (_textY < -1400) {
         _nextScene = "intro"
      }
   }

   draw(w, h) {
      Draw.image(_bg, 0, 0)

      Draw.setColor(Color.Fill, 0, 0, 0, 255)
      Draw.setTextStyle(_bodyFont, 28, 1.0, Align.Left|Align.Top)
      Draw.text(600, _textY, 640, _text)
   }

   shutdown() {
      SoundController.stopAsset(_audio)
   }
}