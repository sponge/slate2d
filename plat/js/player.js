// @ts-check
/// <reference path="../decs.d.ts" />
import * as Draw from 'draw';
import * as SLT from 'slate2d';
import * as Assets from 'assets';

import Buttons from './buttons.js';
import Entity from './entity.js';
import { clamp } from './util.js';
import Dir from './dir.js';

class Player extends Entity {
  t = 0;

  // entity definition
  type = 'Player';
  sprite = Assets.find('dogspr');
  size = [14,14];
  drawOfs = [-3, -1];

  // entity state
  disableControls = false;
  pMeter = 0;
  //health = 3;
  //shotsActive = 0;
  //isPlayer = true;
  fallingFrames = 0;
  jumpHeld = false;
  jumpHeldFrames = 0;
  //invulnTime = 0;
  facing = 1;
  //nextShotTime = 0;

  // physics values
  // values from https://cdn.discordapp.com/attachments/191015116655951872/332350193540268033/smw_physics.png
  pMeterCapacity = 112;
  friction = 0.03125 * 2;
  accel = 0.046875 * 2;
  skidAccel = 0.15625 * 2;
  runSpeed = 1.125 * 2;
  maxSpeed = 1.5 * 2;
  heldGravity = 0.1875 / 2 * 2;
  gravity = 0.1875 * 2;
  earlyBounceFrames = 8;
  earlyJumpFrames = 6;
  lateJumpFrames = 6;
  terminalVelocity = 2 * 2;
  enemyJumpHeld = 3.4 * 2;
  enemyJump = 1.9 * 2;
  jumpHeights = [
    [  3, 2.875 * 2],
    [2.5, 2.78125 * 2],
    [  2, 2.71875 * 2],
    [1.5, 2.625 * 2],
    [  1, 2.5625 * 2],
    [0.5, 2.46875 * 2],
    [  0, 2.40625 * 2]
  ];

  //jumpHnd = null;
  //jumpSound = Asset.create(Asset.Sound, "player_jump", "sound/jump.wav")
  //shootSound = Asset.create(Asset.Sound, "player_shoot", "sound/shoot.wav")
  //hurtSound = Asset.create(Asset.Sound, "player_hurt", "sound/hurt.wav")
  //dieSound = Asset.create(Asset.Sound, "player_die", "sound/die.wav")

  die(cause) {
    // super(cause)
    // _health = 0
    // Trap.sndStop(_jumpHnd)
    // Trap.sndPlay(_dieSound)
    // world.playerDied(this)
  }

  hurt(other, amount) {
    // if (world.ticks < _invulnTime) {
    //   return
    // }

    // if (world.levelWon) {
    //   return
    // }

    // _health = _health - amount
    // _invulnTime = world.ticks + 120

    // if (_health <= 0) {
    //   die(other)
    // } else {
    //   Trap.sndStop(_jumpHnd)
    //   Trap.sndPlay(_hurtSound)
    // }
  }

  update(dt) {
    this.t += dt;
    this.frame = Math.floor(this.t * 12) % 6

    const dir = this.disableControls ? 0 : SLT.buttonPressed(Buttons.Left) ? -1 : SLT.buttonPressed(Buttons.Right) ? 1 : 0;
    const jumpPress = this.disableControls ? false : SLT.buttonPressed(Buttons.Jump);
    const shootPress = this.disableControls ? false : SLT.buttonPressed(Buttons.Shoot);
  
    let grounded = this.vel[1] >= 0 && this.collideAt(this.pos[0], this.pos[1] + 1, Dir.Down);
    // TODO: spring code here

    // set direction for bullets and sprite drawing
    this.facing = dir != 0 ? dir : this.facing;

    // track frames since leaving platform for late jump presses
    this.fallingFrames = grounded ? 0 : this.fallingFrames + 1;

    // let players jump a few frames early but don't let them hold the button down
    this.jumpHeldFrames = jumpPress ? this.jumpHeldFrames + 1 : 0;
    if (!jumpPress && this.jumpHeld) {
      this.jumpHeld = false;
    }

    // if not pushing anything, slow down if on the ground
    if (dir == 0) {
      if (this.vel[0] != 0 && grounded) {
        this.vel[0] += this.friction * (this.vel[0] > 0 ? -1 : 1)
      }

      // null out small values so we dont keep bouncing around 0
      if (Math.abs(this.vel[0]) <= this.friction) {
        this.vel[0] = 0;
      }
    } else {
      // if holding a direction, figure out how fast we should try and go
      const speed = Math.sign(dir*this.vel[0]) == -1 ? this.skidAccel : this.accel;
      this.vel[0] = this.vel[0] + speed * dir;
    }

    // apply gravity if not on the ground. different gravity values depending on holding jump
    this.vel[1] = grounded ? 0 : this.vel[1] + (this.jumpHeld ? this.heldGravity : this.gravity);
 
    // if jump is held, and player has let go of it since last jump
    if (jumpPress && !this.jumpHeld) {
      // allow the jump if:
      // - they're on the ground, and haven't been holding for too long
      // - they're not on the ground, but have recently been on the ground
      if ((grounded && this.jumpHeldFrames < this.earlyJumpFrames) || (!grounded && this.fallingFrames < this.lateJumpFrames)) {
        for (const [speed, height] of this.jumpHeights) {
          if (Math.abs(this.vel[0]) >= speed) {
            this.vel[1] = -height;
            this.jumpHeld = true;
            grounded = false;
            // this.jumpHnd = SLT.sndPlay(this.jumpSound); // TODO: audio
            break
          }
        }
      }
    }

    // increment the p-meter if you're on the ground and going fast enough
    if (Math.abs(this.vel[0]) >= this.runSpeed && grounded) {
      this.pMeter += 2;
    // tick down the p-meter, but don't if you're at 100% and midair
    } else {
      if (grounded || this.pMeter != this.pMeterCapacity) {
        this.pMeter -= 1;
      }
    }
    this.pMeter = clamp(this.pMeter, 0, this.pMeterCapacity);

    // hard cap speed values
    if (this.pMeter == this.pMeterCapacity) {
      this.vel[0] = clamp(this.vel[0], -this.maxSpeed, this.maxSpeed);
    } else {
      this.vel[0] = clamp(this.vel[0], -this.runSpeed, this.runSpeed);
    }

    this.vel[1] = Math.min(this.vel[1], this.terminalVelocity);

    // move x first, then move y. don't do it at the same time, else buggy behavior
    // if (!groundEnt || groundEnt.has("spring") == false) { // TODO: spring check
    const chkx = this.moveX(this.vel[0]);
    // triggerTouch(chkx.delta // TODO: trigger check
    if (!chkx) {
      this.vel[0] = 0;
    }

    const chky = this.moveY(this.vel[1]);
    if (!chky) {
      this.vel[1] = 0;
    }

    // TODO: all sorts of shit around jumping on top of enemies
  }

  draw() {
    Draw.setColor(255, 255, 255, 255);
    Draw.sprite(this.sprite, this.frame, this.pos[0] + this.drawOfs[0], this.pos[1] + this.drawOfs[1], 1, this.facing > 0 ? 0 : 1, 1, 1);
  }
}

export default Player;