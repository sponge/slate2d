import "timer" for Timer
import "engine" for Draw, Asset, Trap, Fill, Button, ImageFlags
import "debug" for Debug
import "uibutton" for TextButton
import "soundcontroller" for SoundController
import "fonts" for Fonts

class GameInfo {
   boxPath { _boxPath }
   boxPath=(val) { _boxPath = val }
   title { _title }
   title=(val) { _title = val }
   year { _year }
   year=(val) { _year = val }
   description { _description }
   description=(val) { _description = val }
   gameScene { _gameScene }
   gameScene=(val) { _gameScene = val }

   nextScene { _nextScene }
   nextScene=(params) { _nextScene = params }

   construct new(params) {
      _nextScene = null
      _time = 0

      _rightCol = 500

      _bodyFont = Asset.create(Asset.Font, "body", Fonts.body)
      _descriptionFont = Asset.create(Asset.Font, "description", Fonts.description)

      _boxHnd = Asset.create(Asset.Image, _boxPath, _boxPath, ImageFlags.LinearFilter)
      _gradient = Asset.create(Asset.Image, "menu_gradient", "gfx/menu_gradient.png")

      _items = [
         TextButton.new("gameselect", _rightCol, 650, 160, 40, "Back"),
         TextButton.new(gameScene, 1070, 650, 160, 40, "Play"),
      ]

      Asset.loadAll()
   }

   update(dt) {
      _time = _time + dt
      var mouse = Trap.mousePosition()

      for (item in _items) {
         item.update(dt, mouse[0], mouse[1])
         if (item.clicked(mouse[0], mouse[1])) {
            Trap.printLn("clicked %(item.id)")
            if (item.id != "gameselect") {
               SoundController.stopMusic()
            }
            _nextScene = item.id
         }
      }
   }

   draw(w, h) {
      Draw.clear()
      Draw.resetTransform()

      Draw.setColor(0, 57, 113, 255)
      Draw.rect(0, 0, w, h, false)
      Draw.image(_gradient, 0, 0, w, h)
      
      Draw.scale(h / 720)

      Draw.setColor(0, 0, 0, 200)
      Draw.rect(0, 0, 425+40, 720, Fill.Solid)

      Draw.image(_boxHnd, 40, 55)

      Draw.setColor(176, 205, 234, 255)
      Draw.setTextStyle(_bodyFont, 80)
      Draw.text(_rightCol, 60, 780, _title)

      Draw.setColor(255, 255, 255, 255)
      Draw.setTextStyle(_bodyFont, 48)
      Draw.text(_rightCol, 100, 780, _year)
      Draw.setTextStyle(_descriptionFont, 32, 1.0)
      Draw.text(_rightCol, 150, 740, _description)

      for (item in _items) {
          item.draw()
      }

      Draw.submit()
   }

   shutdown() {

   }
}

// NOTE: nanovg is a bit silly, so to get an extra empty line, add a space before the newline
class Game1Info is GameInfo {
   construct new(params) {
      boxPath = "gfx/box_1.png"
      year = "1985"
      title = "Battlement Defence"
      gameScene = "game1_title"
      description = "The game that started the British home computer revolution! Originally released on the ZX Spectrum, the first Battlement Defence was an overnight sensation, winning Your Sinclair magazine's coveted “Most Pukka Game of 1985” award.\n\nDespite being a lone bedroom programmer, Sir Clive managed to secure nationwide distribution and soon every schoolchild in the UK had their hands on a copy. It was once rare to pass a school playground without hearing children talking about “getting each others goats” or playing “protect the sterling” in which one child, designated as “The Clive” would protect a rolling pound coin from the other children, designated as “The Goats”."
      super(params)
   }
}

class Game2Info is GameInfo {
   construct new(params) {
      boxPath = "gfx/box_2.png"
      year = "1988"
      title = "Battlement Defence 2"
      gameScene = "game2_cracktro"
      description = "Following on from the success of Battle Defence Sir Clive founded his own studio, Reasonable Software, and released Battlement Defence 2. Building on the original, the game features updated sound and graphics as well as some innovative new mechanics. The game was another success, selling over 450,000 units across the many platforms it was ported to.\n\nReasonable software was a stalwart of the Amiga and Commodore era of home computers, publishing such classics as “Real Talk Basketball” and “Iron L.A.D.E Large Angle Dynamite Emitter”.\n\n Unfortunately the original Amiga release has been lost to the sands of time, so presented here is the widely available EGA DOS port of the game."
      super(params)
   }
}

class Game3Info is GameInfo {
   construct new(params) {
      boxPath = "gfx/box_3.png"
      year = "1990"
      title = "Battlement Defense Arcade"
      gameScene = "game3_title"
      description = "Battlement Defence 2 may have been a commercial success, but it was also vigorously pirated. In an attempt to stop this, Reasonable Software made the tactical decision to make the next Battlement Defence game an arcade exclusive.\n\nMuch like its predecessor, the game features updated graphics and gameplay, however the most innovative feature was not actually the game itself, but the patented Clive Monitoring System built into the cabinet. It was designed to intimidate rowdy youths, causing them to question their life decisions.\n\nThe system was a rousing success, with many arcades reporting a drop in rowdiness and many youths returning to the machine day in day out to feed it coins as penance. Over 30,000 units of Battle Defense Arcade were produced, with machines in especially rowdy areas reportedly earning as much as £15,000 a day at their peak."
      super(params)
   }
}

class Game4Info is GameInfo {
   construct new(params) {
      boxPath = "gfx/box_4.png"
      year = "2016"
      title = "Battlement Defense ReDefended"
      gameScene = "game4_title"
      description = "At the start of the 1990s, Sir Clive hung up the Battlement Defense name, announcing “I’ve given all I’ve goat to give!” and retired from public view. That all changed in 2012 however, when the 1980s British sensation became the 2010s crowdfunding darling\n\nReleased in 2016, Battlement Defense ReDefended brings the series into the modern age, with high-resolution sprites inspired by the arcade classic and even more innovative gameplay mechanics. ReDefended is also the first game in the series to feature a digital audio soundtrack, composed by $8000 Guest Music Producer tier backer “noogiejeff”.\n\nDespite numerous delays, Battle Defense ReDefended was a hit amongst it’s 90,451 backers, proving once again the timelessness of Sir Clive’s design."
      super(params)
   }
}

class Game5Info is GameInfo {
   construct new(params) {
      boxPath = "gfx/box_5.png"
      year = "2018"
      title = "Towers"
      gameScene = "game5_title"
      description = "After his triumphant return, Sir Clive decided to return to his roots and distill the Battlement Defense series down to its core components: towers.\n\nTowers strips the gameplay down to the bare minimum, presenting a pure tower climbing experience showcasing how adversity triumphs over self doubt. Peer into the deep creative soul of Sir Clive and you may witness Sir Clive peering back into yours. The central motif of the series is undeniably the tower and Towers seeks to explore this. The humble tower is both a tool used to overcome, and to restrain; Sir Clive demonstrates this with great success in Towers, cementing him as one of the medium's most brilliant thinkers.\n\nWhat lies atop the tower? Few people have seen, until now…\n\nNOTE: THE PRIZE HAS BEEN CLAIMED. DO NOT CONTACT SIR CLIVE UNDER ANY CIRCUMSTANCE."
      super(params)
   }
}