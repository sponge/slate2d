import "timer" for Timer
import "engine" for Draw, Asset, Trap, Color, Fill, Button, ImageFlags
import "debug" for Debug
import "uibutton" for TextButton
import "soundcontroller" for SoundController

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

      _bodyFont = Asset.create(Asset.Font, "body", "fonts/Roboto-Regular.ttf")
      _boxHnd = Asset.create(Asset.Image, _boxPath, _boxPath, ImageFlags.LinearFilter)
      _font = Asset.create(Asset.BitmapFont, "buttonfont", "gfx/panicbomber_blue.png")
      Asset.bmpfntSet(_font, " !\"#$\%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~", 8, 0, 8, 8)

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

      Draw.setColor(Color.Fill, 0, 57, 113, 255)
      Draw.rect(0, 0, w, h, false)
      
      Draw.transform(h / 720, 0, 0, h / 720, 0, 0)

      Draw.setColor(Color.Fill, 0, 0, 0, 200)
      Draw.rect(0, 0, 425+40, 720, Fill.Solid)

      Draw.image(_boxHnd, 40, 55)

      Draw.setColor(Color.Fill, 176, 205, 234, 255)
      Draw.setTextStyle(_bodyFont, 60)
      Draw.text(_rightCol, 60, 780, _title)

      Draw.setColor(Color.Fill, 255, 255, 255, 255)
      Draw.setTextStyle(_bodyFont, 36)
      Draw.text(_rightCol, 100, 780, _year)
      Draw.setTextStyle(_bodyFont, 24, 1.25)
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
      description = "The game that kicked off the British sensation! First released on the ZX Spectrum, the first Battlement Defence became an immediate sensation. Starting off as a lone bedroom programmer, his first game would eventually get a limited retail release, with the box art seen to the left.

Much of the core Battlement Defence elements can be seen in this first game. You couldn't pass a schoolyard in the UK without hearing kids talking about \"getting each other's goats\" and Clive's proud depiction of defending the Pound Sterling would be referenced in a famous Parliament session during discussions of adopting the Euro.
"
      super(params)
   }
}

class Game2Info is GameInfo {
   construct new(params) {
      boxPath = "gfx/box_2.png"
      year = "1988"
      title = "Battlement Defence 2"
      gameScene = "game2_title"
      description = "Released under the Reasonable Software banner, Battlement Defence 2 would be ported to almost every platform available at the time. Founded in 1986 by Clive himself, Reasonable Software became a stalwart of the Amiga and Commodore era. This time in Clive's life would also see the releases of his other hit games, \"Real Talk Basketball\" and the shoot-em-up \"Iron L.A.D.E: Large Angle Dynamite Emitter\" 
 
Unfortunately, the original release has been lost to time. Included in this collection is a very widespread version of the EGA DOS port of the game. Overall, Battlement Defence 2 would go on to sell over 450,000 units across all platforms.
"
      super(params)
   }
}

class Game3Info is GameInfo {
   construct new(params) {
      boxPath = "gfx/box_3.png"
      year = "1990"
      title = "Battlement Defense Arcade"
      gameScene = "game3_title"
      description = "Rampant piracy of Battlement Defence 2 would result in a tactical decision for the next game in the Clive Sturridge's Battlement Defense series. For the first time, Reasonable Software would be hitting the arcades.
 
Battlement Defense's most innovative feature wasn't actually in the game itself, but in the arcade cabinet. The Clive Monitoring System was designed to harshly judge youths, to bring them in-line by making them consider their behavior. When the first cabinets were released to a test market in Wales, arcades reported a drop in rowdyness. The day after release, locations reported their coin boxes were full, the cause of which was later to be determined to be field mice nesting.
 
Over 30,000 units of Battlement Defense would be built throughout the game's run. Aging arcade hardware has made finding working cabinets almost impossible.
"
      super(params)
   }
}

class Game4Info is GameInfo {
   construct new(params) {
      boxPath = "gfx/box_4.png"
      year = "2016"
      title = "Battlement Defense ReDefended"
      gameScene = "game4_title"
      description = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Maecenas posuere facilisis fermentum. Pellentesque id magna at nunc ornare aliquet vel et massa. Phasellus eu interdum arcu. Sed in volutpat sapien. Vestibulum tellus lacus, hendrerit a sem id, semper cursus lacus. In eu tempor ante. Fusce congue odio et nisi cursus, in porta tellus semper. Ut nunc eros, molestie iaculis purus ac, fermentum aliquet nibh. Ut fermentum, magna sit amet suscipit dignissim, lectus urna volutpat neque, sed tempor quam urna vel felis. Vestibulum hendrerit hendrerit nisi, cursus laoreet massa pretium ac. Quisque porta elit felis, vitae accumsan diam fermentum a. Praesent ac rutrum velit. Praesent ornare efficitur bibendum. Nulla mollis risus vel vestibulum vehicula. Vivamus quis enim purus.

Nulla rutrum urna quis sem vestibulum faucibus id quis mauris. Suspendisse euismod aliquam aliquam. Praesent elementum id urna non vestibulum. Nam lacus leo, egestas a porta vitae, vehicula in libero. Mauris vitae erat orci."

      super(params)
   }
}

class Game5Info is GameInfo {
   construct new(params) {
      boxPath = "gfx/box_5.png"
      year = "2018"
      title = "Towers."
      gameScene = "game5_title"
      description = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Maecenas posuere facilisis fermentum. Pellentesque id magna at nunc ornare aliquet vel et massa. Phasellus eu interdum arcu. Sed in volutpat sapien. Vestibulum tellus lacus, hendrerit a sem id, semper cursus lacus. In eu tempor ante. Fusce congue odio et nisi cursus, in porta tellus semper. Ut nunc eros, molestie iaculis purus ac, fermentum aliquet nibh. Ut fermentum, magna sit amet suscipit dignissim, lectus urna volutpat neque, sed tempor quam urna vel felis. Vestibulum hendrerit hendrerit nisi, cursus laoreet massa pretium ac. Quisque porta elit felis, vitae accumsan diam fermentum a. Praesent ac rutrum velit. Praesent ornare efficitur bibendum. Nulla mollis risus vel vestibulum vehicula. Vivamus quis enim purus.

Nulla rutrum urna quis sem vestibulum faucibus id quis mauris. Suspendisse euismod aliquam aliquam. Praesent elementum id urna non vestibulum. Nam lacus leo, egestas a porta vitae, vehicula in libero. Mauris vitae erat orci."

      super(params)
   }
}