import * as Draw from 'draw';
import Entity from '../entity.js';
import Dir from '../dir.js';
import CollisionType from '../collisiontype.js';
import World from '../world.js';
class Switch extends Entity {
    type = 'Switch';
    collidable = CollisionType.Platform;
    target;
    constructor(args) {
        super(args);
        this.target = args.properties?.Target ?? '';
    }
    draw() {
        Draw.setColor(172, 50, 50, 255);
        Draw.rect(this.pos[0], this.pos[1], this.size[0], this.size[1], false);
    }
    collide(other, dir) {
        if (dir != Dir.Up)
            return;
        World().state.entities.forEach(ent => {
            if (ent.name == this.target) {
                ent.trigger(this);
            }
        });
        this.destroyed = true;
    }
}
export { Switch };
