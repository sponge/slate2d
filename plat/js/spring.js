import * as SLT from 'slate2d';
import * as Assets from 'assets';
import Entity from './entity.js';
import Dir from './dir.js';
import CollisionType from './collisiontype.js';
import Player from './player.js';
import World from './world.js';
import Buttons from './buttons.js';
class Spring extends Entity {
    sprite = Assets.find('spring');
    collidable = CollisionType.Platform;
    activated = false;
    activateTicks = 0;
    delay = 12;
    constructor(args) {
        super(args);
    }
    preupdate(ticks, dt) {
        if (!this.activated)
            return;
        if (World().state.ticks < this.activateTicks)
            return;
        for (let other of this.getRidingEntities()) {
            if (other instanceof Player) {
                other.disableControls = false;
                other.disableMovement = false;
                other.vel[1] += SLT.buttonPressed(Buttons.Jump) ? -9 : -6;
                other.jumpHeldFrames = 999; // hack to allow holding down work here to work
                other.fallingFrames = 999;
            }
            else {
                other.vel[1] += -9;
            }
        }
        this.activated = false;
    }
    update(ticks, dt) {
        const oldFrames = this.frame;
        this.frame = Math.floor(this.activated ? (ticks - (this.activateTicks - this.delay)) / 3 : 0);
        this.frame = this.frame >= 3 ? 1 : this.frame;
        this.moveSolid(0, (this.frame - oldFrames) * 4);
    }
    draw() {
        // Draw.setColor(255, 255, 0, 255);
        // Draw.rect(this.pos[0], this.pos[1], this.size[0], this.size[1], false);
        super.draw();
    }
    collide(other, dir) {
        if (dir != Dir.Up)
            return;
        if (!this.activated) {
            this.activated = true;
            this.activateTicks = World().state.ticks + this.delay;
        }
        if (other instanceof Player) {
            other.disableControls = true;
            other.disableMovement = true;
        }
    }
}
export default Spring;
