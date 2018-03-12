class Trap {
   foreign static print(text)
   foreign static console(text)
   foreign static sndPlay(assetHandle, volume, pan, loop)
   foreign static mapLoad(name)
   foreign static mapFree()
}

class Asset {
   foreign static create(type, name, path)
   foreign static find(name)
   foreign static loadAll()
   foreign static clearAll()
   foreign static bmpfntSet(assetHandle, glyphs, charSpacing, spaceWidth, lineHeight)
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
   foreign static setTransform(absolute, a, b, c, d, e, f)
   foreign static setScissor(x, y, w, h)
   foreign static resetScissor()
   foreign static rect(x, y, w, h, outline)
   foreign static text(x, y, text, align)
   foreign static bmpText(x, y, scale, text, fntId)
   foreign static image(x, y, w, h, ox, oy, alpha, flipBits, imgId, shaderId)
   foreign static line(x1, y1, x2, y2)
   foreign static circle(x, y, radius, outline)
   foreign static tri(x1, y1, x2, y2, x3, y3, outline)
   foreign static mapLayer(layer, x, y, cellX, cellY, cellW, cellH)
   foreign static sprite(spr, id, x, y, alpha, flipBits, w, h)
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

class Scene {
   construct new(args) {}
   update(dt) {}
   draw(w, h) {}
   shutdown() {}
} 