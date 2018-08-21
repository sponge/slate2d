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
   foreign static keyActive(key)
   foreign static keyPressed(key, delay, repeat)
   foreign static mousePosition()
   foreign static inspect(obj, title)
   static inspect(obj) { inspect(obj, null) }
   foreign static getResolution()
   foreign static clearKeyPressed()
   // HACK: because i'm sometimes skipping update() to run at 60, key inputs may be delayed a frame. calling
   // this after we run an update frame lets me continue to know if the button was pressed on this frame
   // even if an update frame was skipped 
}

foreign class CVar {
   construct get(name, defaultValue) {}
   foreign bool()
   foreign number()
   foreign string()
   foreign set(val)
}

class Button {
   static Up { 0 }
   static Down { 1 }
   static Left { 2 }
   static Right { 3 }
   static A { 4 }
   static B { 5 }
   static X { 6 }
   static Y { 7 }
   static L { 8 }
   static R { 9 }
   static Start { 10 }
   static Select { 11 }
}

class Asset {
   foreign static create(type, name, path, flags)
   static create(type, name, path) { create(type, name, path, 0) }
   foreign static find(name)
   foreign static load(assetHandle)
   foreign static loadAll()
   foreign static clearAll()
   foreign static bmpfntSet(assetHandle, glyphs, glyphWidth, charSpacing, spaceWidth, lineHeight)
   foreign static measureBmpText(fntId, text, scale)
   static measureBmpText(fntId, text) { measureBmpText(fntId, text, 1.0) }
   foreign static imageSize(assetHandle)
   foreign static spriteSet(assetHandle, w, h, marginX, marginY)

   static Any { 0 }
   static Image { 1 }
   static Sprite { 2 }
   static Speech { 3 }
   static Sound { 4 }
   static Mod { 5 }
   static Font { 6 }
   static BitmapFont { 7 }
   static TileMap { 8 }
   static Max { 9 }
}

class Draw {
   foreign static setColor(which, r, g, b, a)
   static setColor(which, rgba) { setColor(which, rgba[0], rgba[1], rgba[2], rgba[3])}
   foreign static resetTransform()
   foreign static transform(a, b, c, d, e, f)
   foreign static rotate(angle)
   foreign static translate(x, y)
   foreign static setScissor(x, y, w, h)
   foreign static resetScissor()
   foreign static rect(x, y, w, h, outline)
   foreign static setTextStyle(fntId, size, lineHeight, align)
   static setTextStyle(fntId, size, lineHeight) { setTextStyle(fntId, size, 1.0, 1) }
   static setTextStyle(fntId, size) { setTextStyle(fntId, size, 1.0) }
   foreign static text(x, y, w, text)
   foreign static bmpText(fntId, x, y, text, scale)
   static bmpText(fntId, x, y, text) { bmpText(fntId, x, y, text, 1.0) }
   foreign static image(imgId, x, y, w, h, alpha, scale, flipBits, ox, oy, shaderId)
   static image(imgId, x, y, w, h, alpha, scale, flipBits, ox, oy) { image(imgId, x, y, w, h, alpha, scale, flipBits, ox, oy, 0) }
   static image(imgId, x, y, w, h, alpha, scale, flipBits) { image(imgId, x, y, w, h, alpha, scale, flipBits, 0, 0, 0) }
   static image(imgId, x, y, w, h, alpha, scale) { image(imgId, x, y, w, h, alpha, scale, 0, 0, 0, 0) }
   static image(imgId, x, y, w, h, alpha) { image(imgId, x, y, w, h, alpha, 1.0, 0, 0, 0) }
   static image(imgId, x, y, w, h) { image(imgId, x, y, w, h, 1.0, 1.0, 0, 0, 0) }
   static image(imgId, x, y) { image(imgId, x, y, 0.0, 0.0, 1.0, 1.0, 0, 0, 0) }
   foreign static line(x1, y1, x2, y2)
   foreign static circle(x, y, radius, outline)
   foreign static tri(x1, y1, x2, y2, x3, y3, outline)
   foreign static mapLayer(layer, x, y, cellX, cellY, cellW, cellH)
   static mapLayer(layer, x, y, cellX, cellY) { mapLayer(layer, x, y, cellX, cellY, 0, 0) }
   static mapLayer(layer, x, y) { mapLayer(layer, x, y, 0, 0, 0, 0) }
   static mapLayer(layer) { mapLayer(layer, 0, 0, 0, 0, 0, 0) }
   foreign static sprite(spr, id, x, y, alpha, scale, flipBits, w, h)
   static sprite(sprId, id, x, y, alpha, scale, flipBits) { sprite(sprId, id, x, y, alpha, scale, flipBits, 1, 1) }
   static sprite(sprId, id, x, y, alpha, scale) { sprite(sprId, id, x, y, alpha, scale, 0, 1, 1) }
   static sprite(sprId, id, x, y, alpha) { sprite(sprId, id, x, y, alpha, 1.0, 0, 1, 1) }
   static sprite(sprId, id, x, y) { sprite(sprId, id, x, y, 1.0, 1.0, 0, 1, 1) }

   foreign static submit()
   foreign static clear()
}

class ImageFlags {
   static GenerateMipmaps { 1<<0 }
	static RepeatX { 1<<1 }
	static RepeatY { 1<<2 }
	static FlipY { 1<<3 }
	static PremultipiedAlpha { 1<<4 }
	static LinearFilter { 1<<5 }
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

class Color {
   static Fill { 0 }
   static Stroke { 1 }
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