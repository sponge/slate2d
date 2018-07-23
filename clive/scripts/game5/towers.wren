import "engine" for Asset, Draw, Align, Color
// import "random" for Random
import "bagrandomizer" for BagRandomizer

class Platitudes {
   construct new() {
      _p = [
//          "There’s no I in Team (what about healthy teams that appreciate constructive individuality?)",
//          "Good things come to those who wait",
//          "It was meant to be (then why did we bother trying to make it not be?)",
//          "The definition of insanity is doing the same thing and expecting different results (sometimes you have to do the same exact thing many times to get the result you want – it’s called practice)",
//          "Time heals all wounds (how about losing a limb?)",
//          "Such is life",
//          "Forgive and forget",
//          "Everything happens for a reason (including suffering and early death?)",
         "People are our most important asset",
//          "It is what it is",
         "What the mind can conceive, it can achieve",
//          "Winners never quit",
         "What doesn’t kill me will only make me stronger",
//          "I don’t want a team of champions, I want a champion team. (Note that there is an I in “champion team”)",
//          "Teamwork to make the Dream work",
//          "“C’est la vie”",
         "Hard work always pays off",
//          "God has a plan for you",
//          "Great minds think alike",
         "Money can’t buy happiness",
//          "Live each moment like it’s your last (not very pleasant)",
//          "If at first you don’t succeed, try try again (maybe you should try elsewhere)",
//          "Follow your passion",
//          "Follow your bliss (what if mine is cocaine?)",
//          "That’s just my personal opinion",
//          "Let’s not reinvent the wheel",
//          "It’s not rocket science",
//          "It’s all good",
         "What goes up, goes down",
         "After the storm the sun will shine",
//          "Don’t assume – it makes an ASS out of U and ME",
         "Don’t be sad because it’s over, be glad that it happened",
//          "Love means never having to say you’re sorry (Then please don’t love me)",
//          "We are where we are",
         "What goes around comes around",
//          "What’s done is done",
//          "Waste not want not",
//          "It has to be somewhere (as if this helps you find something that’s lost)",
//          "Nice guys finish last",
//          "Go with the flow",
         "Only dead fish go with the flow",
//          "No offense, but…",
         "Rome wasn’t built in a day",
//          "Work smarter, not harder",
//          "There’s no I in team",
//          "Life doesn’t give you things you can’t handle (depends on what life gives you, doesn’t it?)",
//          "You’re as young as you feel",
//          "Age is just a number",
//          "It’s just software",
//          "We’re all in this together",
         "Everything always works out in the end",
//          "Time heals all wounds",
//          "We’ll all be laughing about this soon",
         "It’s doesn’t matter if you win or lose, only that you try",
//          "Tomorrow is another day",
//          "It could be worse",
//          "You are what you eat",
//          "It’s neither here nor there",
         "Think outside the box",
//          "It will all look better in the morning",
         "Take the lemons and make lemonade",
         "The best things in life are free",
//          "It wasn’t meant to be",
//          "Better to have loved and lost…",
//          "That’s for me to know and you to find out",
//          "Better late than never",
//          "With all due respect",
         "The road to hell is paved with good intentions",
         "Gossip is the devil’s radio",
         "Laugh and the world laughs with you",
         "People regret the things they didn’t do",
         "Beauty is only skin deep",
         "You can’t judge a book by it’s cover",
//          "Work hard, play hard",
//          "Only the good die young",
//          "All’s fair in love and war",
         "All men are created equal",
//          "There are plenty more fish in the sea",
         "The more things change, the more they stay the same",
         "It’s the darkest just before dawn",
//          "Fail harder",
         "Perception is reality",
//          "you can be anything that you want to be",
         "Patience is a virtue",
//          "I’m sorry that’s not what you want to hear",
//          "This will hurt me more than it hurts you (well lets switch places and find out)",
//          "The customer is always right",
//          "If you can’t stand the heat, get out of the kitchen (maybe we can make the kitchen better?)",
//          "Success is the ability to go from one failure to another with no loss of enthusiasm",
         "Be careful what you wish for",
         "With great power comes great responsibility",
//          "Just think about how much worse other people have it",
//          "God never gives us more than we can bear (see death, misery, history of suffering)",
      ]
      _rnd = BagRandomizer.new(_p.count)
   }

   next() {
      return _p[_rnd.next()]
   }
}


class Towers {
   nextScene { _nextScene }
   nextScene=(s) { _nextScene = s }

   construct new(params) {
      _platitudes = Platitudes.new()
      _currentPlatitude = _platitudes.next()
      _platitudeTime = 10
      _platitudeTimer = _platitudeTime

      _bodyFont = Asset.create(Asset.Font, "body", "fonts/Roboto-Regular.ttf")

      Asset.loadAll()
   }

   update(dt) {
      if (_platitudeTimer > 0) {
         _platitudeTimer = _platitudeTimer - dt
      } else {
         _platitudeTimer = _platitudeTime
         _currentPlatitude = _platitudes.next()
      }
   }

   draw(w, h) {
      Draw.setTextStyle(_bodyFont, 48, 1.0, Align.Center|Align.Top)
      Draw.text(20, 20, 1289, "towers by clive")

      var platAlpha = 255
      if (_platitudeTimer < 2) {
         platAlpha = 255 * _platitudeTimer/2
      } else if (_platitudeTimer > (_platitudeTime - 2)) {
         platAlpha = 255 - ((_platitudeTimer - (_platitudeTime - 2)) / 2) * 255
      }
      Draw.setColor(Color.Fill, 255, 255, 255, platAlpha)
      Draw.text(20, 80, 1280, _currentPlatitude)
   }
}
