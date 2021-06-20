import * as Assets from 'assets';
import Entity from './entity.js';
import Dir from './dir.js';
import CollisionType from './collisiontype.js';
import Player from './player.js';
import World from './world.js';
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
            other.vel[1] = -9;
            other.pos[1] -= 10; // so they aren't grounded this frame
            if (other instanceof Player) {
                other.disableControls = false;
                other.disableMovement = false;
            }
        }
        this.activated = false;
    }
    update(ticks, dt) {
        this.frame = this.activated ? (ticks - (this.activateTicks - this.delay)) / 3 : 0;
        this.frame = this.frame >= 3 ? 1 : this.frame;
    }
    draw() {
        //Draw.setColor(255, 255, 0, 255);
        //Draw.rect(this.pos[0], this.pos[1], this.size[0], this.size[1], false);
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
