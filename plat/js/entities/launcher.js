import * as Assets from 'assets';
import Entity from '../entity.js';
import CollisionType from '../collisiontype.js';
import { World } from '../game.js';
class Launcher extends Entity {
    sprite = Assets.find('launcher');
    collidable = CollisionType.Enabled;
    launchTime = 180;
    launchWait = 180;
    die() { }
    update(ticks, dt) {
        if (ticks < this.launchTime) {
            return;
        }
        const ent = World().spawnEntity('Cannonball');
        World().spawnPuffParticle(this.pos[0] - 8, this.pos[1]);
        ent.vel = [-1, 0];
        ent.pos = [this.pos[0] - 16, this.pos[1] + 1];
        this.launchTime = ticks + this.launchWait;
    }
}
export { Launcher };
//# sourceMappingURL=launcher.js.map