import Entity from "./entity.js";
import * as Draw from 'draw';
import CollisionType from "./collisiontype.js";
class Platform extends Entity {
    collidable = CollisionType.Platform;
    dim = 1;
    dir = -1;
    bottom;
    top;
    constructor(args) {
        super(args);
        this.bottom = this.pos[this.dim];
        this.top = this.bottom + (this.dim == 0 ? 1 : -1) * 100;
    }
    update(dt) {
        if (this.pos[this.dim] > this.bottom)
            this.dir *= -1;
        if (this.pos[this.dim] < this.top)
            this.dir *= -1;
        this.moveSolid(this.dim == 0 ? this.dir : 0, this.dim == 1 ? this.dir : 0);
    }
    draw() {
        Draw.setColor(255, 255, 0, 255);
        Draw.rect(this.pos[0], this.pos[1], this.size[0], this.size[1], false);
    }
}
export default Platform;
