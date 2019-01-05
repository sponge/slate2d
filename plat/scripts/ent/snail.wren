import "entity" for Entity
import "collision" for Dir, Dim
import "engine" for Trap
import "math" for Math
import "debug" for Debug

class Snail is Entity {
  construct new(world, obj, ox, oy) {
    super(world, obj, ox + 1, oy + 1, 6, 7)
    _speed = 0.25
    _terminalVelocity = 2
    _stunTime = 0
    _shell = false
    _shellSpeed = 1

    dx = -_speed

    props["bouncy"] = true
    props["shootable"] = true
  }

  shot(other, time) {
    if (_shell) {
      dx = other.dx > 0 ? _shellSpeed : -_shellSpeed
    } else {
      _stunTime = world.ticks + time
    }
  }

  canCollide(other, side, d) { true }

  touch(other, side) {
    if (_shell) {
      if (side == Dir.Up) {
        dx = dx != 0 ? 0 : centerX > other.centerX ? _shellSpeed : -_shellSpeed
      } else if (side == Dir.Down) {
        dy = other.dy * 2
      } else {
        dx = side == Dir.Left ? -_shellSpeed : _shellSpeed
      }
    } else {
      if (side == Dir.Up) {
        _shell = true
        _stunTime = 0
        dx = 0
      } else {
        other.hurt(this, 1)
      }
    }
  }

  think(dt) {
    runPlatform(dt)

    // give snail shells a little extra boost on springs than they'd normally provide
    if (_shell && groundEnt && groundEnt.has("spring")) {
      var amt = groundEnt.checkSpring()
      if (amt != 0) {
        dy = amt * 1.3
        dx = dx * 1.3
      }
    }

    // check ground here otherwise snail won't get boost from springs
    var ground = snapGround()

    if (grounded && _shell) {
      dx = dx == 0 ? 0 : dx > 0 ? _shellSpeed : - _shellSpeed
    }

    if (!grounded) {
      dy = Math.min(dy + world.gravity, _terminalVelocity)
      var checkY = check(Dim.V, dy)
      if (checkY.t < 1.0) {
        dy = 0
      }
      y = y + checkY.delta
    }

    if (world.ticks > _stunTime && (!groundEnt || groundEnt.has("spring") == false)) {
      var checkX = check(Dim.H, dx)
      if (dx != 0 && checkX.entity) {
        checkX.entity.hurt(this, 1)
      }
      x = x + checkX.delta

      if (checkX.t != 1.0) {
        dx = -dx
      }
    }

    if (y > world.level.maxY + 10) {
      die(null)
    }
  }

  draw(t) {
    var drawX = x
    if (world.ticks < _stunTime) {
      drawX = x + (world.ticks).sin / 2
    }

    var offset = _shell ? 1 : 0 //world.ticks / 8 % 2
    // -1 to account for 6x7 hitbox
    drawSprite(288 + offset, drawX - 1, y - 1, 1, 1, dx > 0 ? 1 : 0)
  }
}