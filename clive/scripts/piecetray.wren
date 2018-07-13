import "engine" for Draw, Asset, Trap, Color, Fill, Button, TileMap, CVar
import "math" for Math

class PieceTray {
   construct new (td, x, y, w, h) {
      _td = td
      _x = x
      _y = y
      _w = w
      _h = h

      _activeTool = null
   }

   // needs to go from screen space -> local
   transformCoords(x, y) {
      
   }

   update(dt) {

   }

   draw() {
      Draw.translate(_x, _y)

      Draw.setColor(Color.Stroke, 255, 255, 0, 255)
      Draw.rect(0, 0, _w, _h, Fill.Outline)

      // draw gold (number of currencies are game dependent, pass in from TD)
      // draw towers (towers are game dependent, pass in from TD)
      // draw grass
      // draw available wall pieces

      Draw.translate(-_x, -_y)
   }
}