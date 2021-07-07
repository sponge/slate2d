import * as Assets from 'assets';
import Entity from '../entity.js';
import Dir from '../dir.js';
import CollisionType from '../collisiontype.js';
import Player from './player.js';
import Phys from '../phys.js';
var Frames;
(function (Frames) {
    Frames[Frames["Idle"] = 0] = "Idle";
    Frames[Frames["Blink"] = 1] = "Blink";
    Frames[Frames["Pain"] = 2] = "Pain";
    Frames[Frames["Run1"] = 3] = "Run1";
    Frames[Frames["Run2"] = 4] = "Run2";
    Frames[Frames["Run3"] = 5] = "Run3";
    Frames[Frames["Run4"] = 6] = "Run4";
    Frames[Frames["Run5"] = 7] = "Run5";
    Frames[Frames["Run6"] = 8] = "Run6";
    Frames[Frames["Run7"] = 9] = "Run7";
    Frames[Frames["Squish"] = 10] = "Squish";
})(Frames || (Frames = {}));
class Bub extends Entity {
    drawOfs = [-2, -5];
    sprite = Assets.find('bub');
    constructor(args) {
        super(args);
        this.flipBits = 1;
        this.vel[0] = args.properties?.GoRight ?? true ? 0.25 : -0.25;
        this.flipBits = this.vel[0] > 0 ? 1 : 0;
    }
    canCollide(other, dir) {
        if (other.type == 'Player')
            return CollisionType.Platform;
        else
            return CollisionType.Enabled;
    }
    update(ticks, dt) {
        let grounded = this.vel[1] >= 0 && this.collideAt(this.pos[0], this.pos[1] + 1, Dir.Down);
        this.vel[1] = grounded ? 0 : this.vel[1] + Phys.enemyGravity;
        if (!this.moveX(this.vel[0])) {
            this.vel[0] *= -1;
        }
        if (!this.moveY(this.vel[1])) {
            this.vel[1] = 0;
        }
        this.flipBits = this.vel[0] > 0 ? 1 : 0;
        if (this.vel[1] != 0) {
            this.frame = Frames.Pain;
        }
        else if (grounded) {
            this.frame = (ticks / 8) % (Frames.Run7 - Frames.Run1) + Frames.Run1;
        }
    }
    collide(other, dir) {
        if (other instanceof Player) {
            other.stompEnemy();
            this.destroyed = true;
        }
        else {
            this.vel[0] *= -1;
            return;
        }
    }
}
export default Bub;
