import * as Assets from 'assets';
import Dir from '../dir.js';
import CollisionType from '../collisiontype.js';
import { Player } from './player.js';
import World from '../world.js';
import { clamp } from '../util.js';
import FSMEntity from '../fsmentity.js';
var Frames;
(function (Frames) {
    Frames[Frames["Idle"] = 0] = "Idle";
    Frames[Frames["Float1"] = 1] = "Float1";
    Frames[Frames["Float2"] = 2] = "Float2";
    Frames[Frames["Float3"] = 3] = "Float3";
    Frames[Frames["Float4"] = 4] = "Float4";
    Frames[Frames["Pain"] = 5] = "Pain";
})(Frames || (Frames = {}));
var States;
(function (States) {
    States[States["None"] = 0] = "None";
    States[States["Idle"] = 1] = "Idle";
    States[States["Float"] = 2] = "Float";
})(States || (States = {}));
class Ghost extends FSMEntity {
    type = 'Ghost';
    drawOfs = [-2, -1];
    sprite = Assets.find('ghost');
    state = States.Idle;
    nextState = States.None;
    nextStateTime = 0;
    constructor(args) {
        super(args);
        this.flipBits = 1;
        this.worldCollide = false;
    }
    die() {
        super.die();
        World().spawnDeathParticle(this, Frames.Pain);
    }
    #states = {
        default: {
            enter: () => this.fsmDefaultTransitionTo(States.Idle),
        },
        [States.Idle]: {
            enter: () => {
                this.fsmTransitionAtTime(States.Float, 90);
                this.vel[0] = 0;
                this.vel[1] = 0;
                this.frame = Frames.Idle;
            },
            update: (ticks) => this.flipBits = this.center(0) < World().player.center(0) ? 1 : 0,
        },
        [States.Float]: {
            enter: () => this.fsmTransitionAtTime(States.Idle, 180),
            update: (ticks) => {
                const player = World().player;
                this.vel[0] += Math.sign(player.center(0) - this.center(0)) * 0.03;
                this.vel[1] += Math.sign(player.center(1) - this.center(1)) * 0.03;
                this.vel[0] = clamp(this.vel[0], -0.6, 0.6);
                this.vel[1] = clamp(this.vel[1], -0.6, 0.6);
                this.frame = Math.floor(ticks / 8 % 4) + Frames.Float1;
                if (this.vel[0] != 0) {
                    this.flipBits = this.vel[0] < 0 ? 1 : 0;
                }
            }
        }
    };
    update(ticks, dt) {
        this.fsmUpdate(this.#states, ticks);
        this.moveX(this.vel[0]);
        this.moveY(this.vel[1]);
    }
    canCollide(other, dir) {
        if (other instanceof Player && dir == Dir.Up)
            return CollisionType.Platform;
        else
            return CollisionType.Trigger;
    }
    collide(other, dir) {
        this.handlePlayerStomp(other, dir);
    }
}
export { Ghost };
