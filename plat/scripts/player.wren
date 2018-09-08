import "entity" for Entity
import "engine" for Trap, Button, Draw, Fill, Asset
import "debug" for Debug
import "main" for Main
import "timer" for Timer
import "math" for Math
import "collision" for Dim, Dir

class Player is Entity {
   disableControls=(b) { _disableControls = b }
   pMeter { _pMeter }
   pMeterCapacity { _pMeterCapacity }
   health { _health }
   shotsActive { _shotsActive }
   shotsActive=(i) { _shotsActive = i }
   isPlayer { true }
   
   construct new(world, ti, ox, oy) {
      super(world, ti, ox, oy - 1, 7, 9)

      _fallingFrames = 0
      _pMeter = 0
      _jumpHeld = false
      _jumpHeldFrames = 0
      _disableControls = false
      _health = 3
      _invulnTime = 0
      _facing = 1
      _shotsActive = 0
      _nextShotTime = 0

      // values from https://cdn.discordapp.com/attachments/191015116655951872/332350193540268033/smw_physics.png
      _friction = 0.03125
      _accel = 0.046875
      _skidAccel = 0.15625
      _runSpeed = 1.125
      _maxSpeed = 1.5
      _pMeterCapacity = 112
      _heldGravity = world.gravity / 2
      _gravity = world.gravity
      _earlyBounceFrames = 8
      _earlyJumpFrames = 6
      _lateJumpFrames = 6
      _terminalVelocity = 2
      _enemyJumpHeld = 3.4
      _enemyJump = 1.9
      _jumpHeights = {
          1.5: 2.875,
         1.25: 2.78125,
            1: 2.71875,
         0.75: 2.625,
          0.5: 2.5625,
         0.25: 2.46875,
            0: 2.40625
      }

      _jumpHnd = null

      _img = Asset.create(Asset.Image, "player", "gfx/notgooddog.png")
      _jumpSound = Asset.create(Asset.Sound, "player_jump", "sound/jump.wav")
      _shootSound = Asset.create(Asset.Sound, "player_shoot", "sound/shoot.wav")
      _hurtSound = Asset.create(Asset.Sound, "player_hurt", "sound/hurt.wav")
      _dieSound = Asset.create(Asset.Sound, "player_die", "sound/die.wav")

      _stunProps = {"player": this}
   }

   die(cause) {
      super(cause)
      _health = 0
      Trap.sndStop(_jumpHnd)
      Trap.sndPlay(_dieSound)
      world.playerDied(this)
   }

   hurt(other, amount) {
      if (world.ticks < _invulnTime) {
         return
      }

      if (world.levelWon) {
         return
      }

      _health = _health - amount
      _invulnTime = world.ticks + 120
      
      if (_health <= 0) {
         die(other)
      } else {
         Trap.sndStop(_jumpHnd)
         Trap.sndPlay(_hurtSound)
      }
   }

   think(dt) {
      var dir = _disableControls ? 0 : Trap.keyPressed(Button.Left) ? -1 : Trap.keyPressed(Button.Right) ? 1 : 0
      var jumpPress = _disableControls ? false : Trap.keyPressed(Button.A)
      var shootPress = _disableControls ? false : Trap.keyPressed(Button.B)
      var speed = 0

      runPlatform(dt)

      // track if on the ground this frame
      var ground = snapGround()
      if (groundEnt && groundEnt.has("spring")) {
         _jumpHeld = jumpPress && _jumpHeldFrames < _earlyBounceFrames
      }

      // set direction for bullets
      _facing = dir != 0 ? dir : _facing

      // track frames since leaving platform for late jump presses
      _fallingFrames = grounded ? 0 : _fallingFrames + 1

      // let players jump a few frames early but don't let them hold the button down
      _jumpHeldFrames = jumpPress ? _jumpHeldFrames + 1 : 0
      if (!jumpPress && _jumpHeld) {
         _jumpHeld = false
      }

      // apply gravity if not on the ground. different gravity values depending on holding jump
      dy = grounded ? 0 : dy + (_jumpHeld ? _heldGravity : _gravity)

      // if jump is held, and player has let go of it since last jump
      if (jumpPress && !_jumpHeld) {
         // allow the jump if:
         // - they're on the ground, and haven't been holding for too long
         // - they're not on the ground, but have recently been on the ground
         if ((grounded && _jumpHeldFrames < _earlyJumpFrames) || (!grounded && _fallingFrames < _lateJumpFrames)) {
            for (speed in _jumpHeights.keys) {
               if (dx.abs >= speed) {
                  dy = -_jumpHeights[speed]
                  _jumpHeld = true
                  _jumpHnd = Trap.sndPlay(_jumpSound)
                  break
               }
            }
         }
      }

      // if not pushing anything, slow down if on the ground
      if (dir == 0) {
         if (dx != 0 && grounded) {
            dx = dx + _friction * (dx > 0 ? -1 : 1)
         }

         // null out small values so we dont keep bouncing around 0
         if (dx.abs <= _friction) {
            dx = 0
         }
      // if holding a direction, figure out how fast we should try and go
      } else {
         speed = Math.sign(dir*dx) == -1 ? _skidAccel : _accel
         dx = dx + speed * dir
      }

      // increment the p-meter if you're on the ground and going fast enough
      if (dx.abs >= _runSpeed && grounded) {
         _pMeter = _pMeter + 2
      // tick down the p-meter, but don't if you're at 100% and midair
      } else {
         if (grounded || _pMeter != _pMeterCapacity) {
            _pMeter = _pMeter - 1
         }
      }
      _pMeter = Math.clamp(0, _pMeter, _pMeterCapacity)

      // hard cap speed values
      if (_pMeter == _pMeterCapacity) {
         dx = Math.clamp(-_maxSpeed, dx, _maxSpeed)
      } else {
         dx = Math.clamp(-_runSpeed, dx, _runSpeed)
      }

      dy = Math.min(dy, _terminalVelocity)

      // move x first, then move y. don't do it at the same time, else buggy behavior
      var chkx = null
      if (!groundEnt || groundEnt.has("spring") == false) {
         chkx = check(Dim.H, dx)
         x = x + chkx.delta
         triggerTouch(chkx)

         // if we hit either direction in x, stop momentum
         if (chkx.t < 1.0) {
            dx = 0
         }
      }

      var chky = check(Dim.V, dy)
      y = y + chky.delta
      triggerTouch(chky)

      if ((chky.side == Dir.Up && (chky.triggers.has("bouncy") || Entity.has(chky.entity, "bouncy"))) ||
       (ground.side == Dir.Up && (ground.triggers.has("bouncy") || Entity.has(ground.entity, "bouncy"))) ) {
         dy = _jumpHeldFrames <= _earlyBounceFrames ? -_enemyJumpHeld : -_enemyJump
         _jumpHeld = jumpPress
      } else if (chky.t < 1.0) {
         // either dir, nullify y movement
         dy = 0
      }

      if (shootPress && _shotsActive < 3 && world.ticks > _nextShotTime) {
         var shot = world.spawn("StunShot", _stunProps, _facing > 0 ? x + 6 : x - 8, y - 2)
         shot.dx = shot.dx * _facing
         _nextShotTime = world.ticks + 30
         Trap.sndPlay(_shootSound)
      }

      if (y > world.level.maxY + 10) {
         die(null)
      }

      // update camera
      world.cam.window(x, y, 20)
   }

   draw(t) {
      if (world.ticks < _invulnTime) {
         if (world.ticks % 9 < 3) {
            return
         }
      }

      //var color = Draw.setColor(218, 212, 94, 255)
      //Draw.rect(x, y, w, h, Fill.Solid)
      Draw.image(_img, x-2, y-3, 12, 12, 1.0, 1.0, _facing > 0 ? 0 : 1)
   }
}