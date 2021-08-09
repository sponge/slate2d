import * as Draw from 'draw';
import CollisionType from '../collisiontype.js';
import Entity from '../entity.js';
import Phys from '../phys.js';
import World from '../world.js';
class SpinParticle extends Entity {
    type = 'SpinParticle';
    collidable = CollisionType.Disabled;
    start = 0;
    worldCollide = false;
    rotate = true;
    constructor(args) {
        super(args);
        this.vel = [1, -3];
    }
    update(ticks, dt) {
        if (ticks >= this.start + 120)
            this.die();
        this.pos[0] += this.vel[0];
        this.pos[1] += this.vel[1];
        this.vel[1] = Math.min(this.vel[1] + 0.2, Phys.terminalVelocity);
    }
    draw() {
        const rotate = 8 * (World().state.ticks - this.start);
        Draw.translate(this.center(0), this.center(1));
        if (this.rotate)
            Draw.rotate(rotate);
        Draw.sprite(this.sprite, this.frame, -this.size[0] / 2 + this.drawOfs[0], -this.size[1] / 2 + this.drawOfs[1], 1, this.flipBits, 1, 1);
        if (this.rotate)
            Draw.rotate(-rotate);
        Draw.translate(-this.center(0), -this.center(1));
    }
}
export { SpinParticle };
