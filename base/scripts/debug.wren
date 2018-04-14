import "engine" for Trap, Draw, Fill, Color

class Debug {
   static rectb(x, y, w, h, c) {
      __rectbs.add([x, y, w, h, c])
   }

   static rect(x, y, w, h, c) {
      __rects.add([x, y, w, h, c])
   }

   static init() {
      __colors = [
         [0, 0, 0],
         [44, 36, 52],
         [48, 52, 109],
         [78, 74, 78],
         [133, 76, 48],
         [52, 101, 36],
         [208, 70, 72],
         [117, 113, 97],
         [89, 125, 206],
         [170, 105, 44],
         [97, 101, 129],
         [109, 170, 44],
         [198, 141, 101],
         [149, 0, 0],
         [218, 212, 94],
         [222, 238, 214]         
      ]

      __lines = []
      __rectbs = []
      __rects = []
   }

   static text(window, val) {
      text(window, val, "")
   }

   static text(window, key, val) {
      if (!__init) {
         __lines = []
         __init = true
      }
      __lines.add([window, key, val])
   }

   static draw() {
      Draw.clear()

      for (line in __lines) {
         Trap.printWin(line[0], line[1], line[2])
      }

      for (r in __rectbs) {
         var c = __colors[r[4]]
         Draw.setColor(c[0], c[1], c[2], 255)
         Draw.rect(r[0], r[1], r[2], r[3], Fill.Outline)
      }

      for (r in __rects) {
         var c = __colors[r[4]]
         Draw.setColor(c[0], c[1], c[2], 255)
         Draw.rect(r[0], r[1], r[2], r[3], Fill.Solid) 
      }

      Draw.submit()
      
      __lines.clear()
      __rectbs.clear()
      __rects.clear()
   }
}