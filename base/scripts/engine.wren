class Trap {
   foreign static print(text)
   static printLn(text) { print(text.toString + "\n")}
   foreign static dbgWin(title, text)
   foreign static console(text)
   foreign static sndPlay(assetHandle, volume, pan, loop)
   foreign static keyActive(key)
   foreign static keyPressed(key, delay, repeat)
   foreign static mousePosition()
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
   foreign static create(type, name, path)
   foreign static find(name)
   foreign static loadAll()
   foreign static clearAll()
   foreign static bmpfntSet(assetHandle, glyphs, charSpacing, spaceWidth, lineHeight)
   foreign static measureBmpText(fntId, text, scale)
   static measureBmpText(fntId, text) { measureBmpText(fntId, text, 1.0) }
   foreign static createSprite(assetHandle, w, h, marginX, marginY)

   static Any { 0 }
   static Image { 1 }
   static Speech { 2 }
   static Sound { 3 }
   static Mod { 4 }
   static Font { 5 }
   static BitmapFont { 6 }
   static Max { 7 }
}

class Draw {
   foreign static setColor(which, r, g, b, a)
   foreign static resetTransform()
   foreign static transform(a, b, c, d, e, f)
   foreign static rotate(angle)
   foreign static translate(x, y)
   foreign static setScissor(x, y, w, h)
   foreign static resetScissor()
   foreign static rect(x, y, w, h, outline)
   foreign static text(x, y, text, align)
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
   static sprite(spr, id, x, y, alpha, scale, flipBits) { sprite(spr, id, x, y, alpha, scale, flipBits, 1, 1) }
   static sprite(spr, id, x, y, alpha, scale) { sprite(spr, id, x, y, alpha, scale, 0, 1, 1) }
   static sprite(spr, id, x, y, alpha) { sprite(spr, id, x, y, alpha, 1.0, 0, 1, 1) }
   static sprite(spr, id, x, y) { sprite(spr, id, x, y, 1.0, 1.0, 0, 1, 1) }

   foreign static submit()
   foreign static clear()
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
   foreign static load(name)
   foreign static free()
   foreign static layerByName(name)
   foreign static objectsInLayer(id)
   foreign static getMapProperties()
   foreign static getLayerProperties(id)
   foreign static getTileProperties()
   foreign static getTile(id, x, y)
}

class Scene {
   construct new(args) {}
   update(dt) {}
   draw(w, h) {}
   shutdown() {}
}
