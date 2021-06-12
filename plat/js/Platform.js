import Entity from "./entity.js";
import * as Draw from 'draw';
class Platform extends Entity {
    dim;
    speed;
    start;
    end;
    constructor(args) {
        super(args);
        this.dim = args.properties?.Direction == 'Horizontal' ? 0 : 1;
        this.speed = args.properties?.Speed ?? 1;
        this.start = this.pos[this.dim];
        this.end = this.start + (args.properties?.Distance ?? 100);
    }
    update(dt) {
        if (this.pos[this.dim] < this.start)
            this.speed *= -1;
        if (this.pos[this.dim] > this.end)
            this.speed *= -1;
        this.moveSolid(this.dim == 0 ? this.speed : 0, this.dim == 1 ? this.speed : 0);
    }
    draw() {
        Draw.setColor(255, 255, 0, 255);
        Draw.rect(this.pos[0], this.pos[1], this.size[0], this.size[1], false);
    }
}
export default Platform;
