import "engine" for Trap, CVar, Draw, Asset, Fill, TileMap, Align
import "button" for Button
import "debug" for Debug
import "math" for Math
import "player" for Player
import "meta" for Meta
import "random" for Random
import "sprites/gfx" for GfxSprite

class Title {
  nextScene { null }
  construct new(mapName) {
    Trap.registerButtons(["up", "down", "left", "right", "start", "jump", "shoot"])

    _testcvar = CVar.get("vid.swapinterval", 1)
    _newcvar = CVar.get("some.new.cvar", 0)
    Trap.printLn("Printing cvar values")
    Trap.printLn("-----------------------------------")
    Trap.printLn(_testcvar.number())
    Trap.printLn(_newcvar.number())
    Trap.printLn(_newcvar.bool())
    Trap.printLn(_newcvar.string())
    Trap.printLn("-----------------------------------")
    _newcvar.set("changed the value")

    Trap.print(mapName+"\n")

    Asset.loadINI("testassets.ini")

    _dog = Asset.find("dog")
    _music = Asset.find("music")
    _speech = Asset.find("speech")

    _font = Asset.find("font")

    _fixedFont = Asset.find("fixedfont")
    _fixedFontBlue = Asset.find("fixedfontblue")

    _spr = Asset.find("spr")

    _crunchSpr = Asset.find("crunchspr")

    var tmapId = Asset.create(Asset.TileMap, mapName, mapName)
    TileMap.setCurrent(tmapId)

    _canvas = Asset.find("canvas")

    _shader = Asset.find("shader_plasma")

    Asset.loadAll()

    _rnd = Random.new()
    _time = 0

    _asdf = "hellooo world"
    _testRange = 1..4
    _testMap2 = {Meta: "class key", 1234: "number key", "a string": "string key", true: "bool key", _testRange: "range key", null: "a null"}
    _testMap2["thing"] = "thing2"
    _testClass = Player
    _testClass2 = Trap
    _mouseCoords = null

    Trap.sndPlay(_music, 1.0, 0.0, true)
    //Trap.sndPlay(_speech, 1.0, 0.0, false)

    _bgLayer = TileMap.layerByName("bg")
    _worldLayer = TileMap.layerByName("world")

    var objectLayer = TileMap.layerByName("objects")
    var objs = TileMap.objectsInLayer(objectLayer)
    Trap.printLn(objs)

    var tiles = TileMap.getTileProperties()
    Trap.printLn(tiles)

    var gid = TileMap.getTile(_worldLayer, 0, 20)
    Trap.printLn(gid)

    var mapProps = TileMap.getMapProperties()
    Trap.printLn(mapProps)

    var layerProps = TileMap.getLayerProperties(_worldLayer)
    Trap.printLn(layerProps)

    var len = Asset.textWidth(_font, "hello world")
    Trap.printLn("length of text is %(len)")

    Draw.setTextStyle(_font, 3, 1.0, Align.Left)
    _splitStr = Asset.breakString(400, "Lorem ^6ipsum ^gdolor sit ^0amet, consectetur adipiscing elit. Vivamus nec ^jaugue ut leo tempor pharetra eu vel velit. Vestibulum mattis bibendum imperdiet. Donec pretium ex sapien, bibendum efficitur augue tincidunt vel. Mauris eu erat eu felis accumsan laoreet. Praesent in massa nibh. Vivamus hendrerit enim et posuere aliquet. Vestibulum vitae lectus erat. Nam tempus tincidunt aliquet. Nulla et odio ac erat blandit eleifend sed id est. Integer quis lorem ut massa efficitur ornare nec nec leo. Phasellus eu molestie orci. Nam facilisis massa eu risus maximus, sed consequat lacus rhoncus. Mauris varius accumsan dapibus.")
    //_splitStr = Asset.breakString(400, "^00 ^11 ^22 ^33 ^44 ^55 ^66 ^77 ^88 ^99 ^aa ^bb ^cc ^dd ^ee ^ff ^gg ^hh ^ii ^jj ^kk ^ll ^mm ^nn ^oo ^pp ^qq ^rr ^ss ^tt ^uu ^vv ^ww ^xx ^yy")
  }

  update(dt) {
    _upActive = Trap.buttonPressed(Button.Up)

    for (i in 0..Button.NumButtons) {
      if (Trap.buttonPressed(i, 0, 1000)) { Trap.printLn("button %(i)") }
    }

    _time = _time + dt

    _mouseCoords = Trap.mousePosition()

  }

  draw(w, h) {
    Trap.printWin("window name", "contents", "some value")
    Trap.printWin("window name", "some longer text", "a")
    Trap.printWin("window name", "a pretty large key name", "a pretty large value")
    Trap.printWin("window name", "a list", [1,2,3])
    Trap.printWin("window name", "number", 1.0)
    Trap.printWin("window name", "map", _testMap2["key1"])

    for (i in 0..20) {
      Trap.printWin("window name", "scrolling test", i)
    }

    Draw.resetTransform()

    Draw.setTextStyle(_font, 2, 1.0, Align.Left)
    Draw.text(100, 10, 0, _splitStr, _time / 2 + 1)
    return

    Draw.useCanvas(_canvas)
    Draw.clear(0, 0, 0, 255)

    Draw.useShader(_shader)
    Draw.setColor(0, 0, 0, 255)
    Draw.rect(0, 0, 320, 180, false)
    Draw.useShader(null)

    Draw.setColor(255, 255, 255, 255)
    Draw.mapLayer(_bgLayer)
    Draw.mapLayer(_worldLayer)

    Draw.mapLayer(_worldLayer, 4, 4, 0, 14, 8, 99)
    Draw.mapLayer(_bgLayer, 4, 4, 0, 14, 8, 99)

    Draw.setColor(255, 0, 0, 128)
    Draw.rect(120 + (_time/80).sin * 80, 3, 16, 16, Fill.Outline)

    Draw.setColor(60, 0, 90, 255)
    Draw.circle(270, 100, 20, Fill.Outline)

    Draw.setColor(0, 255, 0, 255)
    Draw.line(0, 0, 320, 180)

    Draw.setColor(130, 140, 150, 255)
    Draw.rect(10, 30, 64, 64, Fill.Solid)

    Draw.setColor(255, 255, 0, 255)
    Draw.circle(200, 25, 16, Fill.Solid)

    Draw.setColor(140, 90, 40, 255)
    Draw.circle(200, 70, 16, Fill.Outline)

    Draw.setColor(40, 90, 40, 255)
    Draw.tri(150, 150, 160, 160, 130, 160, Fill.Outline)

    Draw.setColor(0, 255, 255, 255)
    Draw.tri(150, 180, 170, 170, 180, 180, Fill.Solid)

    Draw.setColor(255, 255, 255, 255)

    Draw.image(_dog, 120, 120)

    Draw.bmpText(_font, 32, 50, "Good Dog!", 3.0)
    Draw.bmpText(_font, 32, 50, "Good Dog!", 2.0)
    Draw.bmpText(_font, 32, 50, "Good Dog!")

    Draw.bmpText(_fixedFont, 32, 34, "The Journey Of The")
    Draw.bmpText(_fixedFontBlue, 32, 42, "~Most Exceptional~")
    Draw.bmpText(_fixedFontBlue, 32, 100, "Type Y to start")


    Draw.sprite(_spr, 265, 300, 150, 1.0, 0, 3, 3)
    Draw.setColor(255, 255, 255, 128)
    Draw.sprite(_spr, 265, 295, 150, 1.0, 0, 3, 3)
    Draw.setColor(255, 255, 255, 255)

    Draw.sprite(_spr, 17, 250, 32)

    Draw.sprite(_spr, 17, 275, 0, 1.0, 0, 2, 1)
    Draw.sprite(_spr, 17, 275, 8, 1.0, 1, 2, 1)
    Draw.sprite(_spr, 17, 275, 16, 1.0, 2, 2, 1)
    Draw.sprite(_spr, 17, 275, 24, 1.0, 3, 2, 1)
    Draw.sprite(_spr, 17, 275, 32, 1.0, 4, 2, 1)
    Draw.sprite(_spr, 17, 275, 48, 1.0, 5, 2, 1)
    Draw.sprite(_spr, 17, 275, 64, 1.0, 6, 2, 1)
    Draw.sprite(_spr, 17, 275, 80, 1.0, 7, 2, 1)

    Draw.sprite(_spr, 17, 275, 96, 2.0, 7, 2, 1)

    Draw.sprite(_crunchSpr, GfxSprite.notgooddog, 8, 8)

    if (_upActive) {
      Draw.rect(100, 0, 100, 100, Fill.Solid)
    }

    Draw.useCanvas(null)

    Draw.resetTransform()

    var scale = Math.min(h / 180, w / 320).floor
    Draw.scale(scale)

    var x = (w - 320 * scale) / 4
    var y = (h - 180 * scale) / 4
    Draw.image(_canvas, x, y)

    Draw.resetTransform()
    Draw.translate(640+320/2, 320+180/2)

    Draw.rotate(_time / 2 % 360)
    Draw.image(_canvas, -320/2, -180/2)
  }

  shutdown() {
    Asset.clearAll()
  }
}
