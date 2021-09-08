import * as Assets from 'assets';
import Entity from '../entity.js';
import CollisionType from '../collisiontype.js';
import { Player } from './player.js';
import { clamp } from '../util.js';
class Health extends Entity {
    type = 'Health';
    collidable = CollisionType.Trigger;
    sprite = Assets.find('health');
    drawOfs = [0, -3];
    constructor(args) {
        super(args);
    }
    update(ticks, dt) {
        const cycle = ticks % 40;
        if (cycle == 0 || cycle == 10) {
            this.pos[1] += 1;
        }
        else if (cycle == 20 || cycle == 30) {
            this.pos[1] -= 1;
        }
    }
    collide(other, dir) {
        if (other instanceof Player) {
            if (other.health >= other.maxHealth) {
                return;
            }
            this.destroyed = true;
            other.health = clamp(other.health + 1, 0, other.maxHealth);
        }
    }
}
export { Health };
