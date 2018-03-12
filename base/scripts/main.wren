import "engine" for Trap, Draw, Scene, Asset, AssetType

class Game is Scene {
   construct new(mapName) {
      Trap.print(mapName+"\n")
      _dog = Asset.create(AssetType.IMAGE, "dog", "gfx/dog.png")
      _sprites = Asset.create(AssetType.IMAGE, "sprites", "gfx/sprites.gif")
      _music = Asset.create(AssetType.MOD, "music", "music/frantic_-_dog_doesnt_care.it")
      _speech = Asset.create(AssetType.SPEECH, "speech", "great job! you are a good dog!")
      _font = Asset.create(AssetType.BITMAPFONT, "font", "gfx/good_neighbors.png")
      Asset.bmpfntSet(_font, "!\"#$\%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~", -1, 7, 16)

      Trap.mapLoad(mapName)

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

	   Draw.mapLayer(0, 0, 0, 0, 0, 0, 0)

      Draw.setColor(0, 255, 0, 0, 255)
      Draw.rect(5, 3, 16, 16, false)

      Draw.image(120, 120, 154, 16, 0, 0, 1.0, 0, _dog, 0)
      Draw.bmpText(32, 50, 1.0, "Good Dog!", _font)

      Draw.sprite(_spr, 265, 300, 150, 1.0, 0, 3, 3)
      Draw.sprite(_spr, 265, 280, 150, 0.25, 0, 3, 3) // FIXME: alpha is broke

      Draw.setColor(0, 60, 0, 90, 255)
      Draw.setScissor(0, 0, 280, 110)
      Draw.circle(270, 100, 20, false)
      Draw.resetScissor()

      Draw.setColor(1, 0, 255, 0, 255)
      Draw.line(0, 0, 320, 180)

      Draw.setColor(1, 130, 140, 150, 255)
      Draw.rect(10, 30, 64, 64, true)

      Draw.setColor(1, 255, 255, 0, 255)
      Draw.circle(200, 25, 16, true)

      Draw.setColor(0, 140, 90, 40, 255)
      Draw.circle(200, 70, 16, false)

      Draw.setColor(0, 40, 90, 40, 255)
      Draw.tri(150, 150, 160, 160, 130, 160, false)

      Draw.setColor(1, 0, 255, 255, 255)
      Draw.tri(150, 180, 170, 170, 180, 180, true)

      Draw.submit()
   }
}