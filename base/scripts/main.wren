import "engine" for Trap, Draw, Scene, Asset, Fill, Color

class Game is Scene {
   construct new(mapName) {
      Trap.print(mapName+"\n")
      _dog = Asset.create(Asset.Image, "dog", "gfx/dog.png")
      _sprites = Asset.create(Asset.Image, "sprites", "gfx/sprites.gif")
      _music = Asset.create(Asset.Mod, "music", "music/frantic_-_dog_doesnt_care.it")
      _speech = Asset.create(Asset.Speech, "speech", "great job! you are a good dog!")
      _font = Asset.create(Asset.BitmapFont, "font", "gfx/good_neighbors.png")
      Asset.bmpfntSet(_font, "!\"#$\%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~", -1, 7, 16)

      Trap.mapLoad(mapName)

      Asset.loadAll()

      //Trap.sndPlay(_music, 1.0, 0.0, true)
      //Trap.sndPlay(_speech, 1.0, 0.0, false)

      _spr = Asset.createSprite(_sprites, 8, 8, 0, 0)
   }

   update(dt) {
   }

   draw(w, h) {
      Draw.clear()
      Draw.setTransform(h / 180, 0, 0, h / 180, 0, 0, true)

      Draw.mapLayer(0, 0, 0, 0, 0, 0, 0)

      Draw.setColor(Color.Fill, 255, 0, 0, 255)
      Draw.rect(5, 3, 16, 16, Fill.Outline)

      Draw.setColor(Color.Fill, 60, 0, 90, 255)
      Draw.setScissor(0, 0, 280, 110)
      Draw.circle(270, 100, 20, Fill.Outline)
      Draw.resetScissor()

      Draw.setColor(Color.Stroke, 0, 255, 0, 255)
      Draw.line(0, 0, 320, 180)

      Draw.setColor(Color.Stroke, 130, 140, 150, 255)
      Draw.rect(10, 30, 64, 64, Fill.Solid)

      Draw.setColor(Color.Stroke, 255, 255, 0, 255)
      Draw.circle(200, 25, 16, Fill.Solid)

      Draw.setColor(Color.Fill, 140, 90, 40, 255)
      Draw.circle(200, 70, 16, Fill.Outline)

      Draw.setColor(Color.Fill, 40, 90, 40, 255)
      Draw.tri(150, 150, 160, 160, 130, 160, Fill.Outline)

      Draw.setColor(Color.Stroke, 0, 255, 255, 255)
      Draw.tri(150, 180, 170, 170, 180, 180, Fill.Solid)

      Draw.image(_dog, 120, 120)
      Draw.bmpText(_font, 32, 50, "Good Dog!", 1.0)

      Draw.sprite(_spr, 265, 300, 150, 1.0, 1.0, 0, 3, 3)
      Draw.sprite(_spr, 265, 295, 150, 0.5, 1.0, 0, 3, 3)

      Draw.sprite(_spr, 1, 300, 32, 1.0, 2.0, 0, 1, 1)

      Draw.submit()
   }

   shutdown() {
      Trap.mapFree()
      Asset.clearAll()
   }
}