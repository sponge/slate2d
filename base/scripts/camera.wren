import "math" for Math

class Camera {
   x { _x }
   y { _y }
   w { _w }
   h { _h }
   tx { _txRange.min }
   ty { _tyRange.min }
   tw { _txRange.max - _txRange.min }
   th { _tyRange.max - _tyRange.min }

   construct new(tw, th, w, h) {
      _tw = tw
      _th = th
      _w = w
      _h = h

      _conx = 0
      _cony = 0
      _conw = 0
      _conh = 0

      _x = 0
      _y = 0
      _txRange = 0..30
      _tyRange = 0..17
   }

   constrain(x, y, w, h) {
      _conx = x
      _cony = y
      _conw = w
      _conh = h

      move(_x, _y)
   }

   move(x, y) {
      _x = x
      _y = y

      if (_conw > 0 && _conh > 0) {
         //Debug.text("max", "%(_conx) %(_x) %(_conx+_conw-_w)")
         _x = Math.clamp(_conx, _x, _conx+_conw-_w)
         _y = Math.clamp(_cony, _y, _cony+_conh-_h)
      }

      _x = Math.max(_x, 0)
      _y = Math.max(_y, 0)

      var tx = (_x / _tw).floor
      var ty = (_y / _th).floor

      _txRange = tx..tx+(_w/_tw).ceil+1
      _tyRange = ty..ty+(_h/_th).ceil+1 
   }

   window(px, py, windowWidth) {
      var center = _x + _w/2

      if ((px - center).abs <= windowWidth) {
         return
      }

      var delta = px - center + (px > center ? -1 : 1) * windowWidth
      move(_x + delta, y)
   }

   center(x,y) {
      move(x - _w/2, y - _h/2)
   }

   entToCamera(ent) {
      ent.cx = ent.x - x
      ent.cy = ent.y - y
   }

   toCamera(px,py) {
      return [px - x, py - y] 
   }

   toWorld(cx, cy) {
      return [cx + x, cy + y]
   }
}