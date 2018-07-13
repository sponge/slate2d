class Entity {
   x { _x }
   x=(x) { _x = x }
   y { _y }
   y=(y) { _y = y }

   dead { _dead }
   type { _type }

   construct new(x, y, type) {
      _x = x
      _y = y
      _type = type
      _dead = false
   }

   die() {
      _dead = true
   }
}