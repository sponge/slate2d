import * as Assets from 'assets';
import Entity from '../entity.js';
import CollisionType from '../collisiontype.js';
import { Player } from './player.js';
class Flame extends Entity {
    type = 'Flame';
    size = [16, 47];
    sprite = Assets.find('flame');
    collidable = CollisionType.Trigger;
    startOn = false;
    die() { }
    update(ticks, dt) {
        const cyc = (ticks + (this.startOn ? 0 : 240)) % 480;
        const frame = cyc % 8 < 4 ? 0 : 1;
        const offset = Math.abs(270 - cyc);
        if (offset < 60) {
            this.frame = 5 + frame;
        }
        else if (offset < 70) {
            this.frame = 3 + frame;
        }
        else if (offset < 100) {
            this.frame = 1 + frame;
        }
        else {
            this.frame = 0;
        }
    }
    collide(other, dir) {
        if (other instanceof Player == false) {
            return;
        }
        // can't hurt if not on
        if (this.frame != 5 && this.frame != 6) {
            return;
        }
        other.hurt(1);
    }
}
export { Flame };
