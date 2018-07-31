import "timer" for Timer
import "engine" for Draw, Asset, Trap, Color, Fill, Button, Align
import "debug" for Debug
import "uibutton" for GameSelectButton
import "soundcontroller" for SoundController
import "fonts" for Fonts

class GameSelect {
   nextScene { _nextScene }
   nextScene=(params) { _nextScene = params }

   construct new(params) {
      _nextScene = null
      _time = 0

      if (params == "tutorial") {
         _tutorial = true
      }

      _banner = Asset.create(Asset.Sprite, "banner", "gfx/gameselect_banner.png")
      Asset.spriteSet(_banner, 150, 16, 0, 0)

      _gradient = Asset.create(Asset.Image, "menu_gradient", "gfx/menu_gradient.png")

      _music = Asset.create(Asset.Sound, "menu_bgm", "sound/menu_bgm.ogg")
      _click = Asset.create(Asset.Sound, "menu_click", "sound/menu_click.ogg")

      var y = 200
      var width = 200
      var spacing = 45
      var x = spacing
      _items = []
      _items.add(GameSelectButton.new("game1", x, y, width, 350, "Battlement Defence", "gfx/box_1.png", _tutorial ? 0 : 0))
      x = x + width + spacing
      _items.add(GameSelectButton.new("game2", x, y, width, 350, "Battlement Defence 2", "gfx/box_2.png", _tutorial ? 2 : 0))
      x = x + width + spacing
      _items.add(GameSelectButton.new("game3", x, y, width, 350, "Battlement Defense Arcade", "gfx/box_3.png", _tutorial ? 2.5 : 0))
      x = x + width + spacing
      _items.add(GameSelectButton.new("game4", x, y, width, 350, "Battlement Defense ReDefended", "gfx/box_4.png", _tutorial ? 3 : 0))
      x = x + width + spacing
      _items.add(GameSelectButton.new("game5", x, y, width, 350, "Towers", "gfx/box_5.png", _tutorial ? 3.5 : 0))

      _bodyFont = Asset.create(Asset.Font, "body", Fonts.body)
      _currentItem = ""

      Asset.loadAll()

      if (SoundController.isMusicPlaying() == false) {
         SoundController.playMusic(_music)
      }
   }

   update(dt) {
      _time = _time + dt
      var mouse = Trap.mousePosition()

      _currentItem = ""
      for (item in _items) {
         item.update(dt, mouse[0], mouse[1])
         if (item.clicked(mouse[0], mouse[1])) {
            Trap.printLn("clicked %(item.id)")
            SoundController.playOnce(_click)
            _nextScene = item.id
         }

         if (item.hover) {
            _currentItem = item.label
         }
      }

      // if (Trap.keyPressed(Button.B, 0, -1)) {
      //    Debug.printLn("hit")
      // }
   }

   draw(w, h) {
      Draw.clear()

      Draw.setColor(Color.Fill, 0, 57, 113, 255)
      Draw.rect(0, 0, w, h, false)
      Draw.image(_gradient, 0, 0, w, h)

      Draw.resetTransform()
      Draw.transform(h / 720, 0, 0, h / 720, 0, 0)

      for (item in _items) {
          item.draw()
      }

      Draw.sprite(_banner, 0, w/2 - 225, 10, 1, 3)

      Draw.setTextStyle(_bodyFont, 64, 1.0, Align.Center|Align.Top)
      // shadow
      Draw.setColor(Color.Fill, 0, 0, 0, 255)
      Draw.text(0, 103, 1280, _currentItem)
      // text
      Draw.setColor(Color.Fill, 255, 255, 255, 255)
      Draw.text(0, 100, 1280, _currentItem)

      Draw.submit()
   }

   shutdown() {

   }
}