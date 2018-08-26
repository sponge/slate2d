import "engine" for Asset, Draw, Align, Trap, Button
import "random" for Random
import "bagrandomizer" for BagRandomizer
import "soundcontroller" for SoundController
import "pausemenu" for PauseMenu

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

      SoundController.stopMusic()
      Asset.clearAll()

      _music = Asset.create(Asset.Sound, "bgm", "sound/game5_bgm.ogg")

      Asset.loadAll()

      SoundController.playMusic(_music)
   }

   next() {
      return _p[_rnd.next()]
   }
}

class Man {
   y { _y }

   construct new(x, y) {
      _x = x
      _y = y

      _jumping = false
      _jumpTime = 0.25
      _jumpTimer = 0
   }

   jump() {
      if (!_jumping) {
         _jumping = true
         _jumpTimer = _jumpTime
         _y = _y - 6
      }
   }

   update(dt) {
     if (_jumping) {
         _jumpTimer = _jumpTimer - dt
         if (_jumpTimer <= 0) {
            _jumping = false
         }
      }
   }

   draw(color) {
      var width = 20
      var height = 40
      var baseY = _y
      Draw.setColor(color, 255)

      if (_jumping) {
         var jumpY = ((1 - (_jumpTimer / _jumpTime)) * Num.pi).sin * 10
         Draw.rect(_x, baseY - jumpY, width, height, false)
         Draw.setColor(0, 0, 0, 64)
         Draw.rect(_x, baseY + height, width, 4, false)
      } else {
         Draw.rect(_x, baseY, width, height, false)
      }
   }
}

class Towers {
   nextScene { _nextScene }
   nextScene=(s) { _nextScene = s }

   construct new(params) {
      _platitudes = Platitudes.new()
      _currentPlatitude = ""
      _platitudeTime = 12
      _platitudeTimer = 10

      _fade = 0

      _bodyFont = Asset.create(Asset.Font, "raleway", "fonts/Raleway-ExtraLight.ttf")
      _goatSfx = Asset.create(Asset.Sound, "towers_goat", "sound/towers_goat.ogg")
      _goatSfxLong = Asset.create(Asset.Sound, "towers_goat_long", "sound/towers_goat2.ogg")

      _player = Man.new(640, 680)

      _rnd = Random.new()
      _others = []
      for (i in 0..40) {
          _others.add(Man.new(640 + _rnd.int(-220, 220), _rnd.int(740, 1240)))
      }
      _focusedOther = 0
      _focusTime = 10
      _focusTimer = 0

      _time = 0
      _nextGoatSound = _rnd.int(5, 10)

      _pauseMenu = PauseMenu.new(490, 260)
      _paused = false

      Asset.loadAll()
   }

   update(dt) {
      _time = _time + dt

      if (Trap.keyPressed(Button.Start, 0, -1)) {
         _paused = _paused ? false : true
      }

      if (_time > _nextGoatSound) {
         _nextGoatSound = _time + _rnd.int(5, 15)
         SoundController.playOnce(_rnd.sample([_goatSfx, _goatSfxLong]), _rnd.float(0.1, 0.3), _rnd.float(-1, 1), false)
      }

      if (_paused) {
         _pauseMenu.update(dt)

         var pauseAction = _pauseMenu.anyClicked()
         if (pauseAction == "menu") {
            _nextScene = "gameselect"
         } else if (pauseAction == "resume") {
            _paused = false
         } else if (pauseAction == "ending") {
            _nextScene = "towers_ending"
         }

         return
      }

      if (_player.y <= 0) {
         _nextScene = "towers_ending"
      }
      
      if (_platitudeTimer > 0) {
         _platitudeTimer = _platitudeTimer - dt
      } else {
         _platitudeTimer = _platitudeTime
         _currentPlatitude = _platitudes.next()
      }

      if (_fade < 255) {
         _fade = _fade + 1
         _fade = _fade > 255 ? 255 : _fade
      }

      if (Trap.keyPressed(Button.B, 0, -1)) {
         _player.jump()
      }

      for (other in _others) {
         var r = ((1 - ((other.y - 40) / 1200)) * (Num.pi / 2)).tan
         if (_rnd.int(r * 10) == 0) {
            other.jump()
         }
         other.update(dt)
      }

      insertionSort(_others) {|el| el.y}

      _player.update(dt)

      _focusTimer = _focusTimer - dt
      if (_focusTimer <= 0) {
         _focusedOther = (_focusedOther + 1) % _others.count
         _focusTimer = _focusTime
      }
   }

   drawTower() {
      var alpha = 0.5//(_height/_maxHeight) * 0.5 + 0.25
      var width = 500
      Draw.setColor(200, 200, 200, alpha*255)
      Draw.rect(640-(width/2), 20, width, 720, false)
   }

   insertionSort(list, fn) {
      var i = 1
      while (i < list.count) {
         var j = i
         while (j > 0 && fn.call(list[j-1]) > fn.call(list[j])) {
            var temp = list[j]
            list[j] = list[j-1]
            list[j-1] = temp
            j = j - 1
         }
         i = i + 1
      } 
   }

   draw(w, h) {
      drawTower()
//       drawMan()
      _player.draw(255)

      for (other in _others) {
         other.draw(127)
      }
//       _others[_focusedOther].draw(((_focusTimer / _focusTime) * Num.pi).sin * 127)

      Draw.setTextStyle(_bodyFont, 32, 1.0, Align.Center|Align.Middle)

      var platAlpha = 255
      if (_platitudeTimer < 2) {
         platAlpha = 255 * _platitudeTimer/2
         platAlpha = platAlpha < 0 ? 0 : platAlpha
      } else if (_platitudeTimer > (_platitudeTime - 2)) {
         platAlpha = 255 - ((_platitudeTimer - (_platitudeTime - 2)) / 2) * 255
      }
      Draw.setColor(255, 255, 255, platAlpha)
      Draw.text(0, 360, 1280, _currentPlatitude)

      Draw.setColor(255, 255, 255, 127)
      Draw.text(0, 680, 1280, "Click to climb")

      var m = ((680 - _player.y) / 680) * 1000
      Draw.setColor(255, 255, 255, 255)
      Draw.text(0, 40, 1280, "%(1000 - m.floor)m to go")

      Draw.setColor(0, 0, 0, 255 - _fade)
      Draw.rect(0, 0, 1280 ,720, false)

      if (_paused) {
         Draw.resetTransform()
         Draw.scale(h/720)
         _pauseMenu.draw()
      }
   }

   shutdown() {
      SoundController.stopMusic()
   }
}
