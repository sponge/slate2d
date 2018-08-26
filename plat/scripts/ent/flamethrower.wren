import "collision" for Dim
import "entity" for Entity
import "engine" for Draw, Trap

// flames are always active in the world, while the flamethrower entity is just a box
// that does nothing except spawn the Flame
class Flame is Entity {
   construct new(world, dim, dir, delayed, ox, oy) {
      _dim = dim
      _flipped = false
      var w = _dim == Dim.H ? 16 : 6
      var h = _dim == Dim.H ? 6 : 16
      
      _delay = 0
      if (delayed) {
         _delay = 240
      }

      if (dir == "up" || dir == "left") {
         ox = ox + (_dim == Dim.H ? -16 : 1)
         oy = oy + (_dim == Dim.H ? 1 : -16)
      } else {
         ox = ox + (_dim == Dim.H ? 8 : 1)
         oy = oy + (_dim == Dim.H ? 1 : 8)
         _flipped = true
      }

      super(world, null, ox, oy, w, h)
   }  

   isHurting() {
      var cyc = (world.ticks + _delay) % 480
      return cyc > 300
   }

   isFiringUp() {
      var cyc = (world.ticks + _delay) % 480
      return cyc > 240 && cyc < 300
   }

   canCollide(other, side, d) { true }
   trigger { true }

   touch(other, side) {
      if (other.isPlayer == false) {
         return
      }

      if (isHurting() == false) {
         return
      }

      other.hurt(this, 1)
   }

   draw(t) {
      var spr = 263
      if (isHurting() == false) {
         if (isFiringUp()) {
            spr = 264
         } else {
            return
         }     
      }

      // Draw.setColor(255, 0, 0, 255)
      // Draw.rect(x, y, w, h, Fill.Solid)

      var flicker = (t / 3 % 2).floor == 0

      if (_dim == Dim.V) {
         var f = flicker ? 1 : 0
         var flip = _flipped ? 2+f : 0+f
         Draw.sprite(world.spr, spr, x-1, y, 1, 1, flip, 1, 2)
      } else {
         var f = flicker ? 2 : 0
         var flip = _flipped ? 4+1+f : 4+f
         Draw.sprite(world.spr, spr, x, y-1, 1, 1, flip, 1, 2)
      }
   }
}

// flamethrowers just sit and be solid. the fun stuff is in Flame
class Flamethrower is Entity {
   construct new(world, obj, ox, oy) {
      super(world, obj, ox, oy, 8, 8)

      var dir = obj["properties"]["direction"]
      if (dir == "up") {
         _tile = 7
         _dim = Dim.V
      } else if (dir == "right") {
         _tile = 8
         _dim = Dim.H
      } else if (dir == "down") {
         _tile = 9
         _dim = Dim.V
      } else if (dir == "left") {
         _tile = 10
         _dim = Dim.H
      } else {
         Trap.error(2, "Flamethrower at %(ox),%(oy) has no valid direction")
      }

      var altCycle = obj["properties"]["altCycle"] ? true : false

      var ent = Flame.new(world, _dim, dir, altCycle, ox, oy)
      world.entities.add(ent)
   }
   
   draw(t) {
      drawSprite(_tile, x, y)
   }
}