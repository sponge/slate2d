import "engine" for Trap, Draw, Scene, Asset, AssetType

class Game is Scene {
   construct new() {
      _dog = Asset.create(AssetType.IMAGE, "dog", "gfx/dog.png")
      _sprites = Asset.create(AssetType.IMAGE, "sprites", "gfx/sprites.gif")
      _music = Asset.create(AssetType.MOD, "music", "music/frantic_-_dog_doesnt_care.it")
      _speech = Asset.create(AssetType.SPEECH, "speech", "great job! you are a good dog!")
      _font = Asset.create(AssetType.BITMAPFONT, "font", "gfx/good_neighbors.png")
      Asset.bmpfntSet(_font, "!\"#$\%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~", -1, 7, 16)

      Asset.loadAll()

      Trap.sndPlay(_music, 1.0, 0.0, true)
      Trap.sndPlay(_speech, 1.0, 0.0, false)

      _spr = Asset.createSprite(_sprites, 8, 8, 0, 0)
   }

   update(dt) {
   }

   draw(w, h) {
      Draw.clear()
      Draw.setTransform(true, h / 180, 0, 0, h / 180, 0, 0)
      Draw.rect(0, 0, 180, 180, false)
      Draw.sprite(_spr, 265, 200, 150, 1.0, 0, 3, 3)
      Draw.submit()
   }
}