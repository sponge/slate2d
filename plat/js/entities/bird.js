import * as SLT from 'slate2d';
import * as Assets from 'assets';
import Entity from '../entity.js';
import CollisionType from '../collisiontype.js';
import Player from './player.js';
class Bird extends Entity {
    collidable = CollisionType.Trigger;
    drawOfs = [-1, -2];
    sprite = Assets.find('bird');
    enabled = true;
    delta = [0, 0];
    constructor(args) {
        super(args);
        // points are kinda hard to work with and they're not in any format
        // that maps cleanly to the entity position
        const dest = args.properties?.Destination ?? this.pos;
        const tilePos = [Math.floor(this.pos[0] / 16), Math.floor(this.pos[1] / 16)];
        this.delta[0] = (dest[0] - tilePos[0]) * 16;
        this.delta[1] = (dest[1] - tilePos[1]) * 16;
    }
    update(ticks, dt) {
        this.frame = Math.floor(ticks / 8 % 4);
        this.frame = this.frame == 3 ? 1 : this.frame;
        const dir = [Math.sign(this.delta[0]), Math.sign(this.delta[1])];
        this.moveX(dir[0]);
        this.moveY(dir[1]);
    }
    draw() {
        SLT.showObj('bird', this);
        super.draw();
    }
    collide(other, dir) {
        if (other instanceof Player == false) {
            return;
        }
        this.destroyed = true;
    }
}
export default Bird;
