import * as Draw from 'draw';
import Entity from './entity.js';
import Dir from './dir.js';
import CollisionType from './collisiontype.js';
class Switch extends Entity {
    collidable = CollisionType.Platform;
    enabled = true;
    constructor(args) {
        super(args);
    }
    update(ticks, dt) {
    }
    draw() {
        if (!this.enabled)
            return;
        // super.draw();
        Draw.setColor(255, 0, 0, 255);
        Draw.rect(this.pos[0], this.pos[1], this.size[0], this.size[1], false);
    }
    collide(other, dir) {
        if (dir != Dir.Up)
            return;
        this.enabled = false;
        this.collidable = CollisionType.Disabled;
    }
}
export default Switch;
