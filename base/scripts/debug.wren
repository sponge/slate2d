import "engine" for Trap, Draw, Fill

class Debug {
   // if true, store these debug draws in a separate array that is cleared manually used
   // since update isn't necessarily called every frame, but draw is
   static persist(b) {
      __persist = b
   }

   // clear long-lasting debug objects. usually called before update() is run
   static clearPersist() {
      __persistantRects = []
      __persistantTexts = []
   }

   // a bordered rectangle. color is a palette index from __colors
   static rectb(x, y, w, h, c) {
      var dest = __persist ? __persistantRects : __rects
      dest.add([x, y, w, h, c, Fill.Outline])
   }

   // a filled rectangle
   static rect(x, y, w, h, c) {
      var dest = __persist ? __persistantRects : __rects
      dest.add([x, y, w, h, c, Fill.Solid])
   }

   static init() {
      // list of colors used for drawing commands. a bit of a holdover from tic80, but
      // useful because of api brevity, since it should be quick to debug draw stuff.
      __colors = [
         [0, 0, 0, 255],
         [44, 36, 52, 255],
         [48, 52, 109, 255],
         [78, 74, 78, 255],
         [133, 76, 48, 255],
         [52, 101, 36, 255],
         [208, 70, 72, 255],
         [117, 113, 97, 255],
         [89, 125, 206, 255],
         [170, 105, 44, 255],
         [97, 101, 129, 255],
         [109, 170, 44, 255],
         [198, 141, 101, 255],
         [149, 0, 0, 255],
         [218, 212, 94, 255],
         [222, 238, 214, 255],     
      ]

      __persist = false

      __rects = []

      __persistantTexts = []
      __persistantRects = []
   }

   // convenience function so we dont have to import trap
   static printLn(text) {
      Trap.printLn(text)
   }

   // a line of text that will appear in the window named "window"
   static text(window, val) {
      text(window, val, "")
   }

   // a key/val pair will appear in the window named "window"
   // (we don't need to buffer non persistent text here since imgui is always drawn over)
   static text(window, key, val) {
      if (__persist) {
         __persistantTexts.add([window, key, val])
      } else {
         Trap.printWin(window, key, val)
      }
   }

   // a list or a map will have its contents printed in the window named "window"
   static iter(window, collection) {
      var range = collection is List ? 0..collection.count-1 : collection.keys
      for (i in range) {
         text(window, i, collection[i])
      }
   }

   static drawRects_(l) {
      for (r in l) {
         Draw.setColor(__colors[r[4]])
         Draw.rect(r[0], r[1], r[2], r[3], r[5]) 
      }
   }

   // draw out all stored stuff, and then clear per-frame draw items (when persist == false)
   static draw() {
      for (line in __persistantTexts) {
         Trap.printWin(line[0], line[1], line[2])
      }

      drawRects_(__persistantRects)
      drawRects_(__rects)
      
      __rects.clear()
   }
}