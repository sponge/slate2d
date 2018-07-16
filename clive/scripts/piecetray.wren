import "engine" for Draw, Asset, Trap, Color, Fill, Button, TileMap, CVar
import "math" for Math
import "debug" for Debug
import "bagrandomizer" for BagRandomizer
import "uibutton" for TrayButton
import "tower" for Tower

class PieceTray {
   activeTool { _activeTool }
   queuedPieces { _queuedPieces }

   construct new (td, x, y, w, h) {
      _td = td
      _x = x
      _y = y
      _w = w
      _h = h

      // FIXME: include tower3 if gamemode calls for it
      _buttons = [
         TrayButton.new("tower1", "tower", Tower.Fast, x+0, y+8, 16, 16),
         TrayButton.new("tower2", "tower", Tower.Slow, x+24, y+8, 16, 16),
         TrayButton.new("grass",  "grass", 0, x+24, y+32, 16, 16),
         TrayButton.new("piece0", "piece", 0, x+8, y+56+32*0, 24, 24),
         TrayButton.new("piece1", "piece", 1, x+8, y+56+32*1, 24, 24),
         TrayButton.new("piece2", "piece", 2, x+8, y+56+32*2, 24, 24),
         TrayButton.new("piece3", "piece", 3, x+8, y+56+32*3, 24, 24)
      ]

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

   update(dt) {
      var mouse = Trap.mousePosition()

      for (i in (0..._queuedPieces.count)) {
         if (_queuedPieces[i] == null) {
            _queuedPieces[i] = _pieces[_pieceGen.next()]
         }
      }

      for (button in _buttons) {
         if (button.clicked(mouse[0] / _td.scale, mouse[1] / _td.scale)) {
            _activeTool = button
         }
      }
   }

   drawPiece(px, py, alpha, piece) {
      if (piece == null) {
         return
      }

      for (i in 0...9) {
         var x = px + (i%3) * 8
         var y = py + (i/3).floor * 8

         if (piece[i] > 0) {
            Draw.sprite(_td.spr, 4, x, y, alpha)
         }
      }
   }

   // also used by grid to draw the piece preview
   drawTool(x, y, id) {
      var alpha = 1
      if (id == "tower1") {
         Draw.sprite(_td.spr, 0, x, y, alpha, 1, 0, 2, 2)
      } else if (id == "tower2") {
         Draw.sprite(_td.spr, 2, x, y, alpha, 1, 0, 2, 2)
      } else if (id == "grass") {
         Draw.sprite(_td.spr, 22, x, y, alpha, 1, 0, 2, 2)
      } else if (id == "piece0") {
         drawPiece(x, y, alpha, _queuedPieces[0])
      } else if (id == "piece1") {
         drawPiece(x, y, alpha, _queuedPieces[1])
      } else if (id == "piece2") {
         drawPiece(x, y, alpha, _queuedPieces[2])
      } else if (id == "piece3") {
         drawPiece(x, y, alpha, _queuedPieces[3])
      }
   }

   draw() {
      // Draw.translate(_x, _y)

      Draw.setColor(Color.Stroke, 255, 255, 0, 255)

      // draw gold (number of currencies are game dependent, pass in from TD)

      for (button in _buttons) {
         button.draw()
         drawTool(button.x, button.y, button.id)
      }

      // Draw.translate(-_x, -_y)
   }
}