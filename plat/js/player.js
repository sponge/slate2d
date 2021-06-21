import * as Draw from 'draw';
import * as SLT from 'slate2d';
import * as Assets from 'assets';
import Buttons from './buttons.js';
import Entity from './entity.js';
import { clamp } from './util.js';
import Dir from './dir.js';
import Tiles from './tiles.js';
import Phys from './phys.js';
class Player extends Entity {
    // entity definition
    type = 'Player';
    sprite = Assets.find('dogspr');
    drawOfs = [-5, -2];
    spawnPos;
    // entity state
    disableControls = false;
    disableMovement = false;
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
    lastSlide = Tiles.Empty;
    currSlide = Tiles.Empty;
    //jumpHnd = null;
    //jumpSound = Asset.create(Asset.Sound, "player_jump", "sound/jump.wav")
    //shootSound = Asset.create(Asset.Sound, "player_shoot", "sound/shoot.wav")
    //hurtSound = Asset.create(Asset.Sound, "player_hurt", "sound/hurt.wav")
    //dieSound = Asset.create(Asset.Sound, "player_die", "sound/die.wav")
    // die(cause) {
    // super(cause)
    // _health = 0
    // Trap.sndStop(_jumpHnd)
    // Trap.sndPlay(_dieSound)
    // world.playerDied(this)
    // }
    // hurt(other, amount) {
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
    // }
    constructor(args) {
        super(args);
        this.spawnPos = [...this.pos];
    }
    die() {
        this.pos = [...this.spawnPos];
    }
    update(ticks, dt) {
        this.frame = ticks / 8 % 6;
        const dir = this.disableControls ? 0 : SLT.buttonPressed(Buttons.Left) ? -1 : SLT.buttonPressed(Buttons.Right) ? 1 : 0;
        const jumpPress = this.disableControls ? false : SLT.buttonPressed(Buttons.Jump);
        const shootPress = this.disableControls ? false : SLT.buttonPressed(Buttons.Shoot);
        const slidePress = this.disableControls ? false : SLT.buttonPressed(Buttons.Down);
        let grounded = this.vel[1] >= 0 && this.collideAt(this.pos[0], this.pos[1] + 1, Dir.Down);
        // checking again because if we're standing on something it should trigger
        if (grounded && this.collideEnt) {
            this.collideEnt.collide(this, Dir.Up);
            grounded = this.vel[1] >= 0 && this.collideAt(this.pos[0], this.pos[1] + 1, Dir.Down);
        }
        // if we're still on the ground, blank out the decimal
        if (grounded) {
            this.remainder[1] = 0;
        }
        // set direction for bullets and sprite drawing
        this.facing = dir != 0 ? dir : this.facing;
        // track frames since leaving platform for late jump presses
        this.fallingFrames = grounded ? 0 : this.fallingFrames + 1;
        // let players jump a few frames early but don't let them hold the button down
        this.jumpHeldFrames = jumpPress ? this.jumpHeldFrames + 1 : 0;
        if (!jumpPress && this.jumpHeld) {
            this.jumpHeld = false;
        }
        // apply gravity if not on the ground. different gravity values depending on holding jump
        this.vel[1] = grounded ? 0 : this.vel[1] + (this.jumpHeld ? Phys.heldGravity : Phys.gravity);
        // slide on ground if holding down
        // big fuckin hack: sometimes you end up on the corner of a solid tile. in this case, allow
        // the slide if the last frame was a slide
        this.lastSlide = this.currSlide;
        this.currSlide = Tiles.Empty;
        const slopes = [Tiles.SlopeL, Tiles.SlopeR];
        if (!this.jumpHeld && slidePress && (slopes.includes(this.lastSlide) || slopes.includes(this.collideTile))) {
            const checkTile = slopes.includes(this.collideTile) ? this.collideTile : this.lastSlide;
            const slideDir = checkTile == Tiles.SlopeL ? -1 : 1;
            this.vel[0] = slideDir * 4;
            this.vel[1] = 4;
            this.remainder[0] = 0;
            this.remainder[1] = 0;
            this.currSlide = this.collideTile;
        }
        // if not pushing anything, slow down if on the ground
        else if (dir == 0) {
            if (this.vel[0] != 0 && grounded) {
                this.vel[0] += Phys.friction * (this.vel[0] > 0 ? -1 : 1);
            }
            // null out small values so we dont keep bouncing around 0
            if (Math.abs(this.vel[0]) <= Phys.friction) {
                this.vel[0] = 0;
                this.remainder[0] = 0;
            }
        }
        else {
            // if holding a direction, figure out how fast we should try and go
            const speed = Math.sign(dir * this.vel[0]) == -1 ? Phys.skidAccel : Phys.accel;
            this.vel[0] = this.vel[0] + speed * dir;
        }
        // if jump is held, and player has let go of it since last jump
        if (jumpPress && !this.jumpHeld) {
            // allow the jump if:
            // - they're on the ground, and haven't been holding for too long
            // - they're not on the ground, but have recently been on the ground
            if ((grounded && this.jumpHeldFrames < Phys.earlyJumpFrames) || (!grounded && this.fallingFrames < Phys.lateJumpFrames)) {
                for (const [speed, height] of Phys.jumpHeights) {
                    if (Math.abs(this.vel[0]) >= speed) {
                        this.vel[1] = -height;
                        this.jumpHeld = true;
                        grounded = false;
                        // this.jumpHnd = SLT.sndPlay(this.jumpSound); // TODO: audio
                        break;
                    }
                }
            }
        }
        // increment the p-meter if you're on the ground and going fast enough
        if (Math.abs(this.vel[0]) >= Phys.runSpeed && grounded) {
            this.pMeter += 2;
            // tick down the p-meter, but don't if you're at 100% and midair
        }
        else {
            if (grounded || this.pMeter != Phys.pMeterCapacity) {
                this.pMeter -= 1;
            }
        }
        this.pMeter = clamp(this.pMeter, 0, Phys.pMeterCapacity);
        // hard cap speed values
        if (this.pMeter == Phys.pMeterCapacity) {
            this.vel[0] = clamp(this.vel[0], -Phys.maxSpeed, Phys.maxSpeed);
        }
        else {
            this.vel[0] = clamp(this.vel[0], -Phys.runSpeed, Phys.runSpeed);
        }
        this.vel[1] = Math.min(this.vel[1], Phys.terminalVelocity);
        // move x first, then move y. don't do it at the same time, else buggy behavior
        // if (!groundEnt || groundEnt.has("spring") == false) { // TODO: spring check
        if (!this.disableMovement) {
            const chkx = this.moveX(this.vel[0]);
            // triggerTouch(chkx.delta // TODO: trigger check
            if (!chkx) {
                this.vel[0] = 0;
                this.remainder[0] = 0;
            }
            const velY = this.vel[1];
            const chky = this.moveY(velY);
            // this.moveY may alter our velocity, so double check vel
            if (!chky && Math.sign(velY) == Math.sign(this.vel[1])) {
                this.vel[1] = this.remainder[1] = 0;
            }
        }
    }
    draw() {
        Draw.setColor(255, 255, 255, 255);
        // Draw.rect(this.pos[0], this.pos[1], this.size[0], this.size[1], false);
        Draw.sprite(this.sprite, this.frame, this.pos[0] + this.drawOfs[0], this.pos[1] + this.drawOfs[1], 1, this.facing > 0 ? 0 : 1, 1, 1);
    }
}
export default Player;
