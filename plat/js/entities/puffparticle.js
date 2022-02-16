import * as Assets from 'assets';
import CollisionType from '../collisiontype.js';
import Entity from '../entity.js';
class PuffParticle extends Entity {
    type = 'PuffParticle';
    collidable = CollisionType.Disabled;
    start = 0;
    worldCollide = false;
    sprite = Assets.find('puff');
    constructor(args) {
        super(args);
    }
    update(ticks, dt) {
        this.frame = Math.floor((ticks - this.start) / 4);
        if (this.frame > 7)
            this.destroyed = true;
    }
}
export { PuffParticle };
//# sourceMappingURL=puffparticle.js.map