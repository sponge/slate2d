class Trap {
   foreign static print(text)
   foreign static console(text)

   foreign static assetCreate(type, name, path)
   foreign static assetFind(name)
   foreign static assetLoadAll()
   foreign static assetClearAll()
   foreign static assetBmpfntSet(assetHandle, glyphs, charSpacing, spaceWidth, lineHeight)

   foreign static sndPlay(assetHandle, volume, pan, loop)
   foreign static createSprite(assetHandle, w, h, marginX, marginY)
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

class Scene {
   construct new() {}
   update(dt) {}
   draw(w, h) {}
} 