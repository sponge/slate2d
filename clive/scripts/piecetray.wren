import "engine" for Draw, Asset, Trap, Color, Fill, Button, TileMap, CVar, Align
import "math" for Math
import "debug" for Debug
import "bagrandomizer" for BagRandomizer
import "uibutton" for TrayButton
import "tower" for Tower
import "soundcontroller" for SoundController

class PieceTray {
   activeTool { _activeTool } // null, otherwise is a TrayButton
   activePiece { _activePiece } // null, or a 3x3 array 
   queuedPieces { _queuedPieces } // 4 element array contains the 3x3 piece arrays

   construct new (td, x, y, w, h) {
      _mult = td.vHeight / 180

      _td = td
      _x = x * _mult
      _y = y * _mult
      _w = w * _mult
      _h = h * _mult

      // properties for filling in pieces
      _nextPieceGenTime = 0
      _pieceRespawnTime = 1

      _click = Asset.find("game_click")
      _plop = Asset.find("game_plop")
      _rotate = Asset.find("game_rotate")

      var grassSize = 8 * _mult
      var towerSize = 16 * _mult
      var pieceSize = 24 * _mult

      _buttons = [
         TrayButton.new("tower1", "tower", Tower.Fast, _x+4*_mult, _y+8*_mult, towerSize, towerSize),
         TrayButton.new("tower2", "tower", Tower.Slow, _x+28*_mult, _y+8*_mult, towerSize, towerSize),
         TrayButton.new("grass",  "grass", 0, _x+28*_mult, _y+32*_mult, towerSize, towerSize),
         TrayButton.new("piece0", "piece", 0, _x+12*_mult, _y+52*_mult+32*0*_mult, pieceSize, pieceSize),
         TrayButton.new("piece1", "piece", 1, _x+12*_mult, _y+52*_mult+32*1*_mult, pieceSize, pieceSize),
         TrayButton.new("piece2", "piece", 2, _x+12*_mult, _y+52*_mult+32*2*_mult, pieceSize, pieceSize),
         TrayButton.new("piece3", "piece", 3, _x+12*_mult, _y+52*_mult+32*3*_mult, pieceSize, pieceSize)
      ]

      if (td.enableMagicTower) {
         _buttons.add(TrayButton.new("tower3", "tower", Tower.Magic, _x+4*_mult, _y+32*_mult, towerSize, towerSize))
      }

      _pieces = [
         [4,4,0, // sideways U
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

         [0,4,0, // 2x1
          0,4,0,
          0,0,0],

         [0,0,0, // 1x1
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
          0,4,4],

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
         if (_queuedPieces[i] == null && _td.time > _nextPieceGenTime) {
            _queuedPieces[i] = _pieces[_pieceGen.next()]
            _nextPieceGenTime = _td.time + _pieceRespawnTime
         }
      }

      // if a button is clicked, set it as the active tool
      for (button in _buttons) {
         button.update(dt, mouse[0] / _td.scale, mouse[1] / _td.scale)
         if (button.clicked(mouse[0] / _td.scale, mouse[1] / _td.scale)) {
            // track _activePiece here so we can rotate it
            SoundController.playOnce(_click)
            if (button.category == "piece") {
               var piece = _queuedPieces[button.variation]
               if (piece != null) {
                  _activeTool = button
                  _activePiece = piece
               }
            } else {
               _activeTool = button
               _activePiece = null
            }
         }
      }
   }

   // returns true if there is enough money to build the active piece
   canAfford() {
      for (i in 0..._td.currencies.count) {
         if (_td.costs[_activeTool.id][i] > _td.currencies[i]) {
            return false
         }
      }

      return true
   }

   deselectPiece() {
      _activeTool = null
      _activePiece = null
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

      SoundController.playOnce(_plop)
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

      SoundController.playOnce(_rotate)
   }

   // pass in a piece array to draw it at px, py. used for piece shadows and ui
   drawPiece(px, py, alpha, piece) {
      if (piece == null) {
         return
      }

      for (i in 0...9) {
         var x = px + (i%3) * _td.tw
         var y = py + (i/3).floor * _td.th

         if (piece[i] > 0) {
            Draw.sprite(_td.spr, piece[i], x, y, alpha)
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
      } else if (id == "tower3") {
         Draw.sprite(_td.spr, 37, x, y, alpha, 1, 0, 2, 2)
      } else if (id == "grass") {
         Draw.sprite(_td.spr, 22, x, y, alpha, 1, 0, 1, 1)
         Draw.sprite(_td.spr, 22, x+_td.tw, y, alpha, 1, 0, 1, 1)
         Draw.sprite(_td.spr, 22, x, y+_td.th, alpha, 1, 0, 1, 1)
         Draw.sprite(_td.spr, 22, x+_td.tw, y+_td.th, alpha, 1, 0, 1, 1)
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
      Draw.setTextStyle(_td.font, 12*_mult, 1.0, Align.Center+Align.Top)

      // draw gold (number of currencies are game dependent, pass in from TD)
      if (_activeTool == null || canAfford() == true) {
         Draw.setColor(Color.Fill, 255, 255, 255, 255)
      } else {
         Draw.setColor(Color.Fill, 255, 0, 0, 255)
      }
      Draw.text(_x,_y-(4*_mult),_w, "%(_td.currSymbol)%(_td.currencies[0])")

      Draw.setColor(Color.Fill, 255, 255, 255, 255)
      for (button in _buttons) {
         button.draw()
         drawTool(button.x, button.y, button.id)
         // show the price hover for all towers and spawned pieces
         if (button.hover && (button.category != "piece" || _queuedPieces[button.variation] != null)) {
            Draw.text(button.x, button.y+button.w-4, 16*_mult, "%(_td.currSymbol)%(_td.costs[button.id][0])")
            //Draw.rect(button.x, button.y+button.h, button.w, button.h, Fill.Outline)
         }
      }

      // Draw.translate(-_x, -_y)
   }
}