import "engine" for Draw, Trap
import "math" for Math
import "entities/entity" for Entity

class Projectile is Entity {
   construct new(td, startX, startY, target, speed, damage) {
      super(_sx, _sy, "projectile")
      _td = td
      _sx = startX
      _sy = startY
      _target = target
      _speed = speed
      _damage = damage
      _time = 0
   }

   update(dt) {
      _time = _time + (dt * _speed)
      if (_time <= 1) {
         x = Math.lerp(_sx, _target.x, _time)
         y = Math.lerp(_sy, _target.y, _time)
      } else {
         _target.hurt(_damage)
         die()
      }
   }

   drawSprite(id) {
      Draw.sprite(_td.spr, id, x * 8, y * 8)
   }
}

class Cannonball is Projectile {
   construct new(td, startX, startY, target) {
      super(td, startX, startY, target, 2, 4)
   }

   draw() {
      drawSprite(32)
   }
}

class Arrow is Projectile {
   construct new(td, startX, startY, target) {
      super(td, startX, startY, target, 5, 1)
   }

   draw() {
      drawSprite(48)
   }
}