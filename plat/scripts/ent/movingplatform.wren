import "entity" for Entity
import "collision" for Dim, Dir
import "engine" for Trap
import "debug" for Debug

class MovingPlatform is Entity {
  resolve { _resolve }
  platform { true }

  construct new(world, obj, ox, oy) {
    super(world, obj, ox, oy, 24, 0)
    _dist = 0 // how far before we reach our target
    _d = 0 // speed
    _dim = 0 // what axis we're moving on
    _resolve = Fn.new { |side, tile, tx, ty, ldx, ldy| false } // never collide with any tile during movement

    _route = []
    _currentRoute = -1

    _enabled = obj["properties"]["startActive"]
    if (_enabled is Bool == false) {
      Trap.error(2, "MovingPlatform startActive not bool at %(ox), %(oy)")
    }

    if (obj["properties"]["path"] is String == false) {
      Trap.error(2, "MovingPlatform without path string property at %(ox), %(oy)")
    }

    var split = obj["properties"]["path"].split(" ")
    for (node in split) {
      var dir = (node[0] == "u") ? Dir.Up : (node[0] == "l") ? Dir.Left : (node[0] == "r") ? Dir.Right : (node[0] == "d") ? Dir.Down : null
      if (dir == null) {
        Trap.error(2, "Invalid direction \"%(node[0])\" in MovingPlatform at %(ox), %(oy)")
        return
      }
      var amt = Num.fromString(node[1..-1])
      if (amt == null) {
        Trap.error(2, "Invalid amount \"%(node[1..-1])\" in MovingPlatform at %(ox), %(oy)")
        return
      }

      _route.add([dir, amt])
    }

    setNextPoint()
  }

  activate(activator) {
    _enabled = !_enabled
  }

  // moving platforms work like one way. only collide if you're falling through from the top
  canCollide(other, side, d) {
    //Debug.text("ret", "%(side) == %(Dir.Up) && %(other.y)+%(other.h) <= %(y) && %(other.y)+%(other.h)+%(other.dy) > %(y)")
    return side == Dir.Up && other.y+other.h <= y && other.y+other.h+d > y
  }

  setNextPoint() {
    // // if we've still got time to go, or we couldn't find a valid target
    if (_dist > 0) {
      return
    }

    _currentRoute = (_currentRoute + 1) % _route.count

    var dir = _route[_currentRoute][0]
    var amt = _route[_currentRoute][1]

    _d = (dir == Dir.Up || dir == Dir.Left) ? -0.5 : 0.5
    _dim = (dir == Dir.Up || dir == Dir.Down) ? Dim.V : Dim.H
    _dist = amt * world.level.tw
  }

  think(dt) {
    if (!_enabled) {
      return
    }

    // figure out if we need a new destination
    setNextPoint()

    // calculate our movement vector
    dx = (_dim == Dim.H ? _d : 0)
    dy = (_dim == Dim.V ? _d : 0)

    // this is only used to detect if we run into a player. we always move our speed every frame
    var chkx = check(Dim.H, dx)
    var chky = check(Dim.V, dy)

    // if the platform is going to lift the entity up, attach them to this and lift them
    for (col in chky.entities.list) {
      var ent = col.entity
      if (ent.groundEnt != this && intersects(ent) == false) {
        ent.groundEnt = this
        ent.y = ent.y + ent.check(Dim.V, dy).delta
        // Debug.printLn("attach")
      }
    }

    x = x + dx
    y = y + dy

    // subtract our distance remaining
    _dist = _dist - _d.abs
  }

  draw(t) {
    drawSprite(244, x, y)
    drawSprite(244, x+8, y)
    drawSprite(244, x+16, y)
  }
}