import "timer" for Timer
import "engine" for Draw, Asset, Trap, Color, Fill, Button
import "debug" for Debug
import "uibutton" for TextButton

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

      _bodyFont = Asset.create(Asset.Font, "body", "fonts/Roboto-Regular.ttf")
      _boxHnd = Asset.create(Asset.Image, _boxPath, _boxPath)
      _font = Asset.create(Asset.BitmapFont, "buttonfont", "gfx/panicbomber_blue.png")
      Asset.bmpfntSet(_font, " !\"#$\%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~", 8, 0, 8, 8)

      _items = [
         TextButton.new("gameselect", 445, 650, 160, 40, "Back"),
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
            _nextScene = [item.id, ""]
         }
      }
   }

   draw(w, h) {
      Draw.clear()
      Draw.resetTransform()
      Draw.transform(h / 720, 0, 0, h / 720, 0, 0)

      Draw.image(_boxHnd, 40, 55)

      Draw.setColor(Color.Fill, 133, 147, 239, 255)
      Draw.setTextStyle(_bodyFont, 72)
      Draw.text(465, 60, 780, _title)

      Draw.setColor(Color.Fill, 255, 255, 255, 255)
      Draw.setTextStyle(_bodyFont, 36)
      Draw.text(465, 100, 780, _year)
      Draw.setTextStyle(_bodyFont, 24, 1.25)
      Draw.text(465, 150, 780, _description)

      for (item in _items) {
          item.draw()
      }

      Draw.submit()
   }

   shutdown() {

   }
}

class Game1Info is GameInfo {
   construct new(params) {
      boxPath = "gfx/box_1.png"
      year = "1985"
      title = "Battlement Defence"
      gameScene = "game1_title"
      description = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Maecenas posuere facilisis fermentum. Pellentesque id magna at nunc ornare aliquet vel et massa. Phasellus eu interdum arcu. Sed in volutpat sapien. Vestibulum tellus lacus, hendrerit a sem id, semper cursus lacus. In eu tempor ante. Fusce congue odio et nisi cursus, in porta tellus semper. Ut nunc eros, molestie iaculis purus ac, fermentum aliquet nibh. Ut fermentum, magna sit amet suscipit dignissim, lectus urna volutpat neque, sed tempor quam urna vel felis. Vestibulum hendrerit hendrerit nisi, cursus laoreet massa pretium ac. Quisque porta elit felis, vitae accumsan diam fermentum a. Praesent ac rutrum velit. Praesent ornare efficitur bibendum. Nulla mollis risus vel vestibulum vehicula. Vivamus quis enim purus.
 
Nulla rutrum urna quis sem vestibulum faucibus id quis mauris. Suspendisse euismod aliquam aliquam. Praesent elementum id urna non vestibulum. Nam lacus leo, egestas a porta vitae, vehicula in libero. Mauris vitae erat orci."

      super(params)
   }
}

class Game2Info is GameInfo {
   construct new(params) {
      boxPath = "gfx/box_2.png"
      year = "1988"
      title = "Battlement Defence 2"
      gameScene = "game2_title"
      description = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Maecenas posuere facilisis fermentum. Pellentesque id magna at nunc ornare aliquet vel et massa. Phasellus eu interdum arcu. Sed in volutpat sapien. Vestibulum tellus lacus, hendrerit a sem id, semper cursus lacus. In eu tempor ante. Fusce congue odio et nisi cursus, in porta tellus semper. Ut nunc eros, molestie iaculis purus ac, fermentum aliquet nibh. Ut fermentum, magna sit amet suscipit dignissim, lectus urna volutpat neque, sed tempor quam urna vel felis. Vestibulum hendrerit hendrerit nisi, cursus laoreet massa pretium ac. Quisque porta elit felis, vitae accumsan diam fermentum a. Praesent ac rutrum velit. Praesent ornare efficitur bibendum. Nulla mollis risus vel vestibulum vehicula. Vivamus quis enim purus.

Nulla rutrum urna quis sem vestibulum faucibus id quis mauris. Suspendisse euismod aliquam aliquam. Praesent elementum id urna non vestibulum. Nam lacus leo, egestas a porta vitae, vehicula in libero. Mauris vitae erat orci."

      super(params)
   }
}

class Game3Info is GameInfo {
   construct new(params) {
      boxPath = "gfx/box_3.png"
      year = "1990"
      title = "Battlement Defense Arcade"
      gameScene = "game3_title"
      description = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Maecenas posuere facilisis fermentum. Pellentesque id magna at nunc ornare aliquet vel et massa. Phasellus eu interdum arcu. Sed in volutpat sapien. Vestibulum tellus lacus, hendrerit a sem id, semper cursus lacus. In eu tempor ante. Fusce congue odio et nisi cursus, in porta tellus semper. Ut nunc eros, molestie iaculis purus ac, fermentum aliquet nibh. Ut fermentum, magna sit amet suscipit dignissim, lectus urna volutpat neque, sed tempor quam urna vel felis. Vestibulum hendrerit hendrerit nisi, cursus laoreet massa pretium ac. Quisque porta elit felis, vitae accumsan diam fermentum a. Praesent ac rutrum velit. Praesent ornare efficitur bibendum. Nulla mollis risus vel vestibulum vehicula. Vivamus quis enim purus.
      
Nulla rutrum urna quis sem vestibulum faucibus id quis mauris. Suspendisse euismod aliquam aliquam. Praesent elementum id urna non vestibulum. Nam lacus leo, egestas a porta vitae, vehicula in libero. Mauris vitae erat orci."

      super(params)
   }
}

class Game4Info is GameInfo {
   construct new(params) {
      boxPath = "gfx/box_1.png"
      year = "2016"
      title = "Battlement ReDefended"
      gameScene = "game4_title"
      description = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Maecenas posuere facilisis fermentum. Pellentesque id magna at nunc ornare aliquet vel et massa. Phasellus eu interdum arcu. Sed in volutpat sapien. Vestibulum tellus lacus, hendrerit a sem id, semper cursus lacus. In eu tempor ante. Fusce congue odio et nisi cursus, in porta tellus semper. Ut nunc eros, molestie iaculis purus ac, fermentum aliquet nibh. Ut fermentum, magna sit amet suscipit dignissim, lectus urna volutpat neque, sed tempor quam urna vel felis. Vestibulum hendrerit hendrerit nisi, cursus laoreet massa pretium ac. Quisque porta elit felis, vitae accumsan diam fermentum a. Praesent ac rutrum velit. Praesent ornare efficitur bibendum. Nulla mollis risus vel vestibulum vehicula. Vivamus quis enim purus.

Nulla rutrum urna quis sem vestibulum faucibus id quis mauris. Suspendisse euismod aliquam aliquam. Praesent elementum id urna non vestibulum. Nam lacus leo, egestas a porta vitae, vehicula in libero. Mauris vitae erat orci."

      super(params)
   }
}

class Game5Info is GameInfo {
   construct new(params) {
      boxPath = "gfx/box_1.png"
      year = "2018"
      title = "Towers."
      gameScene = "game5_title"
      description = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Maecenas posuere facilisis fermentum. Pellentesque id magna at nunc ornare aliquet vel et massa. Phasellus eu interdum arcu. Sed in volutpat sapien. Vestibulum tellus lacus, hendrerit a sem id, semper cursus lacus. In eu tempor ante. Fusce congue odio et nisi cursus, in porta tellus semper. Ut nunc eros, molestie iaculis purus ac, fermentum aliquet nibh. Ut fermentum, magna sit amet suscipit dignissim, lectus urna volutpat neque, sed tempor quam urna vel felis. Vestibulum hendrerit hendrerit nisi, cursus laoreet massa pretium ac. Quisque porta elit felis, vitae accumsan diam fermentum a. Praesent ac rutrum velit. Praesent ornare efficitur bibendum. Nulla mollis risus vel vestibulum vehicula. Vivamus quis enim purus.

Nulla rutrum urna quis sem vestibulum faucibus id quis mauris. Suspendisse euismod aliquam aliquam. Praesent elementum id urna non vestibulum. Nam lacus leo, egestas a porta vitae, vehicula in libero. Mauris vitae erat orci."

      super(params)
   }
}