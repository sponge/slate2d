import * as Assets from 'assets';
import Entity from '../entity.js';
import Dir from '../dir.js';
import CollisionType from '../collisiontype.js';
import { Player } from './player.js';
import World from '../world.js';
var Frames;
(function (Frames) {
    Frames[Frames["Walk1"] = 0] = "Walk1";
    Frames[Frames["Walk2"] = 1] = "Walk2";
    Frames[Frames["Shell"] = 2] = "Shell";
})(Frames || (Frames = {}));
var States;
(function (States) {
    States[States["None"] = 0] = "None";
    States[States["Walk"] = 1] = "Walk";
    States[States["Shell"] = 2] = "Shell";
})(States || (States = {}));
class Hermit extends Entity {
    drawOfs = [0, 0];
    sprite = Assets.find('hermit');
    state = States.Walk;
    nextState = States.None;
    nextStateTime = 0;
    constructor(args) {
        super(args);
        this.flipBits = 1;
        this.worldCollide = false;
    }
    die() {
        super.die();
        World().spawnDeathParticle(this, Frames.Shell);
    }
    canCollide(other, dir) {
        if (other instanceof Player && !other.stunned && dir == Dir.Up)
            return CollisionType.Enabled;
        else
            return CollisionType.Trigger;
    }
    update(ticks, dt) {
        if (ticks >= this.nextStateTime) {
            this.state = this.nextState;
            this.nextState = States.None;
        }
        switch (this.state) {
            case States.None:
                this.state = States.Walk;
                break;
            case States.Walk:
                break;
            case States.Shell:
                break;
        }
        this.moveX(this.vel[0]);
        this.moveY(this.vel[1]);
    }
    collide(other, dir) {
        if (other instanceof Player) {
            if (!other.stunned && dir == Dir.Up && other.max(1) <= this.center(1)) {
                other.stompEnemy();
                this.state = States.Shell;
            }
            else {
                other.hurt(1);
            }
        }
        else {
            this.vel[0] *= -1;
        }
    }
}
export { Hermit };
