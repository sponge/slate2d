import "random" for Random
import "engine" for Draw, Asset, TileMap, Trap, Button
import "math" for Math
import "actionqueue" for ActionQueue

class Cutscene {
   nextScene { _nextScene }
   nextScene=(params) { _nextScene = params }

   construct new(nextScene) {
      _uncle = Asset.create(Asset.Sprite, "uncle", "gfx/uncle.png")
      _lady = Asset.create(Asset.Sprite, "lady", "gfx/lady.png")
      _guy = Asset.create(Asset.Sprite, "guy", "gfx/guy.png")

      Asset.spriteSet(_uncle, 16, 48, 0, 0)
      Asset.spriteSet(_lady, 16, 48, 0, 0)
      Asset.spriteSet(_guy, 16, 48, 0, 0)

      _icons = Asset.create(Asset.Sprite, "icons", "gfx/icons.png")
      Asset.spriteSet(_icons, 16, 16, 0, 0)
      _iconCount = 8

      _bubble = Asset.create(Asset.Image, "bubble", "gfx/bubble.png")

      _rnd = Random.new()
      _paramNextScene = nextScene
      _canSkip = false

      var y = 80
      _uncleState = {
         "x": 140,
         "y": y,
         "yelling": false,
         "scale": 1.0,
         "icon": _rnd.int(_iconCount)
      }
      _scareds = [
         ScaredEntity.new(_lady, 120, y, 1),
         ScaredEntity.new(_guy, 160, y, -1)
      ]

      _queue = ActionQueue.new([
         [50, Fn.new {
            _uncleState["yelling"] = true
            _uncleState["icon"] = (_uncleState["icon"] + 1) % _iconCount
            for (scared in _scareds) {
               scared.scare(16)
            }
         }],
         [100, Fn.new { _uncleState["yelling"] = false }],
         [50, Fn.new {
            _uncleState["yelling"] = true
            _uncleState["icon"] = (_uncleState["icon"] + 1) % _iconCount
            _uncleState["scale"] = 2.0
            for (scared in _scareds) {
               scared.scare(32)
            }
         }],
         [100, Fn.new { _uncleState["yelling"] = false }],
         [50, Fn.new {
            _uncleState["yelling"] = true
            _uncleState["icon"] = (_uncleState["icon"] + 1) % _iconCount
            _uncleState["scale"] = 3.0
            for (scared in _scareds) {
               scared.scare(256)
            }
         }],
         [100, Fn.new { _uncleState["yelling"] = false }],
         [100, Fn.new { _nextScene = nextScene }],
      ])

      _t = 0

      Asset.loadAll()
   }

   update(dt) {
      _t = _t + dt
      for (scared in _scareds) {
         scared.update(dt)
      }
      _queue.update(dt)

      if (!Trap.keyPressed(Button.Start)) {
         _canSkip = true
      }

      if (_canSkip && Trap.keyPressed(Button.Start)) {
         _nextScene = _paramNextScene
      }
   }

   draw(w, h) {
      Draw.clear()
      Draw.resetTransform()
      Draw.scale(h / 180)

      for (scared in _scareds) {
         scared.draw(w, h)
      }

      Draw.sprite(
         _uncle,
         _uncleState["yelling"] ? (_t / 12) % 2 : 0,
         _uncleState["x"],
         _uncleState["y"])

      if (_uncleState["yelling"]) {
         Draw.image(
            _bubble,
            _uncleState["x"] + 16,
            _uncleState["y"] - 17 - (24 * (_uncleState["scale"] - 1)),
            0, 0, 1.0, _uncleState["scale"])
         Draw.sprite(
            _icons,
            _uncleState["icon"],
            _uncleState["x"] + 16 + (6 * _uncleState["scale"]),
            _uncleState["y"] - 14 - (22 * (_uncleState["scale"] - 1)),
            1.0, _uncleState["scale"])
      } else {
      }
   }

   shutdown() {
      Asset.clearAll()
   }  
}

class ScaredEntity {
   construct new(sprite, x, y, dir) {
      _spr = sprite

      _x = x
      _y = y
      _dir = dir

      _xTarget = _x

      _t = 0
      _rnd = Random.new()
   }

   scare(offset) {
      _xTarget = _x - (offset * _dir)
   }

   update(dt) {
      _t = _t + dt

      var t = dt * 0.1
      _x = (1 - t) * _x + t * _xTarget
   }

   draw(w, h) {
      var diff = (_x - _xTarget).abs
      Draw.sprite(_spr, diff > 0.1 ? 1 : 0, _x, _y)
   }
}