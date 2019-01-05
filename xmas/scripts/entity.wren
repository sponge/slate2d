class Entity {
  name { _name }
  name=(name) { _name=name }
  dead { _dead }
  x { _x }
  x=(x) { _x = x }
  y { _y }
  y=(y) { _y = y }
  centerX { _x + w / 2 }
  centerY { _y + h / 2 }
  w { _w }
  w=(w) { _w = w }
  h { _h }
  h=(h) { _h = h }
  dx { _dx }
  dx=(dx) { _dx = dx }
  dy { _dy }
  dy=(dy) { _dy = dy }

  world { _world }

  construct new(world, obj, x, y, w, h) {
    _world = world
    _active = true
    _props = {}
    _name = obj ? obj["name"] : ""
    _dead = false
    _x = x
    _y = y
    _w = w
    _h = h
    _dx = 0
    _dy = 0
  }

  think(dt){}
  draw(){}
  die(){
    _dead = true
  }
}