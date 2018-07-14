import "engine" for Draw, Asset, Trap, Color, Fill, Button, TileMap, CVar
import "math" for Math
import "debug" for Debug
import "bagrandomizer" for BagRandomizer

class PieceTray {
   construct new (td, x, y, w, h) {
      _td = td
      _x = x
      _y = y
      _w = w
      _h = h

      _pieces = [
         [1,1,0, // U
          0,1,0,
          1,1,0],

         [1,1,1, // big T
          0,1,0,
          0,1,0],

         [0,0,0, // T
          1,1,1,
          0,1,0],

         [0,1,0, // 3x1
          0,1,0,
          0,1,0],

         [0,1,0, // 2x1
          0,1,0,
          0,0,0],

         [0,0,0, // 1x1
          0,1,0,
          0,0,0],

         [0,1,0, // corner
          1,1,0,
          0,0,0],

         [0,0,1, // big Z
          1,1,1,
          1,0,0],

         [0,1,0, // Z
          1,1,0,
          1,0,0],

         [1,0,0, // S
          1,1,0,
          0,1,0],

         [0,1,0, // J
          0,1,0,
          1,1,0],

         [0,1,0, // L
          0,1,0,
          1,1,0],

      ]

      _pieceGen = BagRandomizer.new(_pieces.count)
      _queuedPieces = [null, null, null, null]
      _activeTool = null
   }

   // needs to go from screen space -> local
   transformCoords(x, y) {
      
   }

   update(dt) {
      for (i in (0..._queuedPieces.count)) {
         if (_queuedPieces[i] == null) {
            _queuedPieces[i] = _pieces[_pieceGen.next()]
         }
      }
   }

   drawPiece(centerX, centerY, piece) {
      for (i in 0...9) {
         var x = centerX + (i%3) * 8 - 8
         var y = centerY + (i/3).floor * 8 - 8

         if (piece[i] > 0) {
            Draw.sprite(_td.spr, 4, x, y)
         }
      }
   }

   draw() {
      Draw.translate(_x, _y)

      Draw.setColor(Color.Stroke, 255, 255, 0, 255)
      // Draw.rect(0, 0, _w, _h, Fill.Outline)

      // draw gold (number of currencies are game dependent, pass in from TD)
      // draw towers (towers are game dependent, pass in from TD)
      // draw grass

      var baseY = 56
      for (piece in _queuedPieces) {
         if (piece != null) {
            drawPiece(24, baseY, piece)
            baseY = baseY + 32
         }
      }

      Draw.translate(-_x, -_y)
   }
}