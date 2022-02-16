import * as Assets from 'assets';
import Entity, { Layer } from '../entity.js';
import Dir from '../dir.js';
import CollisionType from '../collisiontype.js';
import { Player } from './player.js';
import { World } from '../game.js';
class CrumblingBlock extends Entity {
    type = 'CrumblingBlock';
    sprite = Assets.find('crumblingblock');
    collidable = CollisionType.Enabled;
    layer = Layer.Background;
    broken = false;
    breakTime = 0;
    crumbleTime = 40;
    numFrames = 4;
    die() { }
    update(ticks, dt) {
        if (!this.broken)
            return;
        this.frame = Math.ceil((ticks - (this.breakTime - this.crumbleTime)) / (this.crumbleTime / this.numFrames));
        if (ticks < this.breakTime)
            return;
        this.destroyed = true;
        const part = World().spawnDeathParticle(this, 4);
        part.vel[0] = 0;
        part.vel[1] = 0;
    }
    collide(other, dir) {
        if (other instanceof Player && dir == Dir.Up && !this.broken) {
            this.broken = true;
            this.breakTime = this.ticks + this.crumbleTime;
        }
    }
}
export { CrumblingBlock };
//# sourceMappingURL=crumblingblock.js.map