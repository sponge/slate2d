class Trap {
  foreign static print(text)
  static printLn(text) { print(text.toString + "\n")}
  static printWin(windowTitle, key, value) { printWin_(windowTitle.toString, key.toString, value.toString) }
  foreign static printWin_(windowTitle, key, value)
  foreign static error(level, text)
  foreign static console(text)
  foreign static sndPlay(assetHandle, volume, pan, loop)
  static sndPlay(assetHandle) { sndPlay(assetHandle, 1.0, 0, false) }
  foreign static sndStop(handle)
  foreign static sndPauseResume(handle, pause)
  foreign static registerButtons(names)
  foreign static buttonPressed(key, delay, repeat)
  static buttonPressed(key) { buttonPressed(key, 0, 0) }
  static buttonPressedOnce(key) { buttonPressed(key, 0, -1) }
  foreign static mousePosition()
  foreign static inspect(obj, title)
  static inspect(obj) { inspect(obj, null) }
  foreign static getResolution()
  foreign static setWindowTitle(title)
  foreign static getPlatform()
}

foreign class CVar {
  construct get(name, defaultValue) {}
  foreign bool()
  foreign number()
  foreign string()
  foreign set(val)
}

class Asset {
  foreign static create(type, name, path, flags)
  static create(type, name, path) { create(type, name, path, 0) }
  foreign static find(name)
  foreign static load(assetHandle)
  foreign static loadAll()
  foreign static clearAll()
  foreign static loadINI(path)
  foreign static bmpfntSet(assetHandle, glyphs, glyphWidth, charSpacing, spaceWidth, lineHeight)
  foreign static textWidth(fntId, text, scale)
  static textWidth(fntId, text) { textWidth(fntId, text, 1.0) }
  foreign static breakString(width, text)
  foreign static imageSize(assetHandle)
  foreign static spriteSet(assetHandle, w, h, marginX, marginY)
  foreign static canvasSet(assetHandle, w, h)
  foreign static shaderSet(assetHandle, isFile, vertexShader, fragmentShader)

  static Any { 0 }
  static Image { 1 }
  static Sprite { 2 }
  static Speech { 3 }
  static Sound { 4 }
  static Mod { 5 }
  static Font { 6 }
  static BitmapFont { 7 }
  static TileMap { 8 }
  static Canvas { 9 }
  static Shader { 10 }
  static Max { 11 }
}

class Draw {
  foreign static setColor(r, g, b, a)
  static setColor(rgba) { setColor(rgba[0], rgba[1], rgba[2], rgba[3])}
  foreign static resetTransform()
  foreign static scale(x, y)
  static scale(factor) { scale(factor, factor) }
  foreign static rotate(angle)
  foreign static translate(x, y)
  foreign static setScissor(x, y, w, h) // NOTE: (0,0) is bottom left
  foreign static resetScissor()
  foreign static useCanvas(id)
  foreign static useShader(id)
  foreign static rect(x, y, w, h, outline)
  foreign static setTextStyle(fntId, size, lineHeight, align)
  static setTextStyle(fntId, size, lineHeight) { setTextStyle(fntId, size, 1.0, 1) }
  static setTextStyle(fntId, size) { setTextStyle(fntId, size, 1.0) }
  foreign static text(x, y, w, text, length)
  static text(x, y, w, string) { text(x, y, w, string, 0) }
  static bmpText(fntId, x, y, text, scale) {
    setTextStyle(fntId, scale, 1, Align.Left)
    Draw.text(x, y, 0, text)
  }
  static bmpText(fntId, x, y, text) { bmpText(fntId, x, y, text, 1.0) }
  foreign static image(imgId, x, y, w, h, scale, flipBits, ox, oy)
  static image(imgId, x, y, w, h, scale, flipBits) { image(imgId, x, y, w, h, scale, flipBits, 0, 0) }
  static image(imgId, x, y, w, h, scale) { image(imgId, x, y, w, h, scale, 0, 0, 0) }
  static image(imgId, x, y, w, h) { image(imgId, x, y, w, h, 1.0, 0, 0, 0) }
  static image(imgId, x, y) { image(imgId, x, y, 0, 0, 1.0, 0, 0, 0) }
  foreign static line(x1, y1, x2, y2)
  foreign static circle(x, y, radius, outline)
  foreign static tri(x1, y1, x2, y2, x3, y3, outline)
  foreign static mapLayer(layer, x, y, cellX, cellY, cellW, cellH)
  static mapLayer(layer, x, y, cellX, cellY) { mapLayer(layer, x, y, cellX, cellY, 0, 0) }
  static mapLayer(layer, x, y) { mapLayer(layer, x, y, 0, 0, 0, 0) }
  static mapLayer(layer) { mapLayer(layer, 0, 0, 0, 0, 0, 0) }
  foreign static sprite(spr, id, x, y, scale, flipBits, w, h)
  static sprite(sprId, id, x, y, scale, flipBits) { sprite(sprId, id, x, y, scale, flipBits, 1, 1) }
  static sprite(sprId, id, x, y, scale) { sprite(sprId, id, x, y, scale, 0, 1, 1) }
  static sprite(sprId, id, x, y) { sprite(sprId, id, x, y, 1.0, 0, 1, 1) }

  foreign static submit()
  foreign static clear(r, g, b, a)
}

class ImageFlags {
  static LinearFilter { 1<<0 }
}

class Align {
  static Left { 1<<0 }
  static Center { 1<<1 }
  static Right { 1<<2 }
  static Top { 1<<3 }
  static Middle { 1<<4 }
  static Bottom { 1<<5 }
  static Baseline { 1<<6 }
}

class Fill {
  static Solid { false }
  static Outline { true }
}

class TileMap {
  foreign static setCurrent(assetHandle)
  foreign static layerByName(name)
  foreign static layerNames()
  foreign static objectsInLayer(id)
  foreign static getMapProperties()
  foreign static getLayerProperties(id)
  foreign static getTileProperties()
  foreign static getTile(id, x, y)
}