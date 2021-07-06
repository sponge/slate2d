import * as Assets from 'assets';
import Entity from '../entity.js';
import CollisionType from '../collisiontype.js';
import Player from './player.js';
class Slime extends Entity {
    collidable = CollisionType.Trigger;
    drawOfs = [-1, -2];
    sprite = Assets.find('slime');
    enabled = true;
    constructor(args) {
        super(args);
    }
    update(ticks, dt) {
        this.frame = ticks % 40 > 30 ? 1 : 0;
    }
    draw() {
        super.draw();
    }
    collide(other, dir) {
        if (other instanceof Player == false) {
            return;
        }
        this.destroyed = true;
    }
}
export default Slime;
