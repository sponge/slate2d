import * as Assets from 'assets';
import Entity from '../entity.js';
import CollisionType from '../collisiontype.js';
import World from '../world.js';
import { Player } from './player.js';
class LevelEnd extends Entity {
    type = 'LevelEnd';
    drawOfs = [-2, -2];
    collidable = CollisionType.Trigger;
    sprite = Assets.find('levelend');
    constructor(args) {
        super(args);
    }
    update(ticks, dt) {
        this.frame = ticks / 8 % 8;
    }
    collide(other, dir) {
        if (other instanceof Player == false) {
            return;
        }
        this.destroyed = true;
        World().completeLevel();
    }
    draw() {
        this.drawOfs[1] = this.frame < 4 ? -3 : -4;
        super.draw();
    }
}
export { LevelEnd };
