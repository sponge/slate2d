import "engine" for Draw, Asset, Trap, Color, Fill, Button, TileMap, CVar, Align
import "math" for Math
import "debug" for Debug
import "bagrandomizer" for BagRandomizer
import "uibutton" for TrayButton
import "tower" for Tower

class PieceTray {
   activeTool { _activeTool } // null, otherwise is a TrayButton
   activePiece { _activePiece } // null, or a 3x3 array 
   queuedPieces { _queuedPieces } // 4 element array contains the 3x3 piece arrays

   construct new (td, x, y, w, h) {
      _td = td
      _x = x
      _y = y
      _w = w
      _h = h

      // properties for filling in pieces
      _nextPieceGenTime = 0
      _pieceRespawnTime = 2

      // FIXME: include tower3 if gamemode calls for it
      _buttons = [
         TrayButton.new("tower1", "tower", Tower.Fast, x+0, y+8, 16, 16),
         TrayButton.new("tower2", "tower", Tower.Slow, x+24, y+8, 16, 16),
         TrayButton.new("grass",  "grass", 0, x+24, y+32, 16, 16),
         TrayButton.new("piece0", "piece", 0, x+8, y+48+32*0, 24, 24),
         TrayButton.new("piece1", "piece", 1, x+8, y+48+32*1, 24, 24),
         TrayButton.new("piece2", "piece", 2, x+8, y+48+32*2, 24, 24),
         TrayButton.new("piece3", "piece", 3, x+8, y+48+32*3, 24, 24)
      ]

      _pieces = [
         [4,4,0, // U
          0,4,0,
          4,4,0],

         [4,4,4, // big T
          0,4,0,
          0,4,0],

         [0,0,0, // T
          4,4,4,
          0,4,0],

         [0,4,0, // 3x4
          0,4,0,
          0,4,0],

         [0,4,0, // 2x4
          0,4,0,
          0,0,0],

         [0,0,0, // 4x4
          0,4,0,
          0,0,0],

         [0,4,0, // corner
          4,4,0,
          0,0,0],

         [0,0,4, // big Z
          4,4,4,
          4,0,0],

         [0,4,0, // Z
          4,4,0,
          4,0,0],

         [4,0,0, // S
          4,4,0,
          0,4,0],

         [0,4,0, // J
          0,4,0,
          4,4,0],

         [0,4,0, // L
          0,4,0,
          4,4,0],

      ]

      _pieceGen = BagRandomizer.new(_pieces.count)
      _queuedPieces = [null, null, null, null]
      _activeTool = null
      _activePiece = null
   }

   update(dt) {
      var mouse = Trap.mousePosition()

      // for each piece in the UI, populate it if it's empty and there's time
      for (i in (0..._queuedPieces.count)) {
         // TODO: can attempt to place null pieces
         if (_queuedPieces[i] == null && _td.time > _nextPieceGenTime) {
            _queuedPieces[i] = _pieces[_pieceGen.next()]
            _nextPieceGenTime = _td.time + _pieceRespawnTime
         }
      }

      // if a button is clicked, set it as the active tool
      for (button in _buttons) {
         button.update(dt, mouse[0] / _td.scale, mouse[1] / _td.scale)
         if (button.clicked(mouse[0] / _td.scale, mouse[1] / _td.scale)) {
            _activeTool = button
            // track _activePiece here so we can rotate it
            if (button.category == "piece") {
               _activePiece = _queuedPieces[button.variation]
            } else {
               _activePiece = null
            }
         }
      }
   }

   canAfford() {
      for (i in 0..._td.currencies.count) {
         if (_td.costs[_activeTool.id][i] > _td.currencies[i]) {
            return false
         }
      }

      return true
   }

   // instruct the piece tray that the piece has been placed
   spendCurrent() {
      // spend currency
      for (i in 0..._td.currencies.count) {
         _td.currencies[i] = _td.currencies[i] - _td.costs[_activeTool.id][i]
      }

      // if it's a piece, we want to remove the piece and replace it with a new one
      if (_activeTool.category == "piece") {
         _queuedPieces[_activeTool.variation] = null
         _activeTool = null
         _activePiece = null
         _nextPieceGenTime = _td.time + _pieceRespawnTime
      }
   }

   // rotate the active piece
   rotateActivePiece() {
      if (_activePiece == null) {
         return
      }

      var newPiece = List.filled(9, 0)
      for (y in 0...3) {
         for (x in 0...3) {
            var rotateX = -y + 2
            var rotateY = x
            var tile = _activePiece[y*3+x]
            newPiece[rotateY*3+rotateX] = tile
         }
      }
      _activePiece = newPiece
   }

   // pass in a piece array to draw it at px, py. used for piece shadows and ui
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

   // used when looping through buttons to draw the correct element
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
      Draw.setTextStyle(_td.font, 12, 1.0, Align.Left+Align.Top)

      // draw gold (number of currencies are game dependent, pass in from TD)
      if (_activeTool == null || canAfford() == true) {
         Draw.setColor(Color.Fill, 255, 255, 255, 255)
      } else {
         Draw.setColor(Color.Fill, 255, 0, 0, 255)
      }
      Draw.text(_x,_y-4,_w, "$:%(_td.currencies[0])")

      Draw.setColor(Color.Fill, 255, 255, 255, 255)
      for (button in _buttons) {
         button.draw()
         drawTool(button.x, button.y, button.id)
         if (button.hover) {
            Draw.text(button.x, button.y+button.w-4, button.w, "$:%(_td.costs[button.id][0])")
            //Draw.rect(button.x, button.y+button.h, button.w, button.h, Fill.Outline)
         }
      }

      // Draw.translate(-_x, -_y)
   }
}