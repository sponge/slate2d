import * as Assets from 'assets';
import { WorldEnt } from '../entity.js';
import Dir from '../dir.js';
import { Player } from './player.js';
import { World } from '../game.js';
import { FSMEntity } from '../fsmentity.js';
import Phys from '../phys.js';
import CollisionType from '../collisiontype.js';
var Frames;
(function (Frames) {
    Frames[Frames["Idle"] = 0] = "Idle";
    Frames[Frames["Rise"] = 1] = "Rise";
    Frames[Frames["Fall"] = 2] = "Fall";
    Frames[Frames["Pain"] = 3] = "Pain";
})(Frames || (Frames = {}));
var States;
(function (States) {
    States[States["None"] = 0] = "None";
    States[States["Wait"] = 1] = "Wait";
    States[States["Pop"] = 2] = "Pop";
    States[States["Recharge"] = 3] = "Recharge";
})(States || (States = {}));
class Daikon extends FSMEntity {
    type = 'Daikon';
    sprite = Assets.find('daikon');
    drawOfs = [-2, -3];
    state = States.Wait;
    nextState = States.None;
    nextStateTime = 0;
    die() {
        super.die();
        World().spawnDeathParticle(this, Frames.Pain);
    }
    #states = {
        default: {
            enter: () => this.fsmDefaultTransitionTo(States.Wait),
        },
        [States.Wait]: {
            enter: () => this.frame = Frames.Idle,
            update: (ticks) => {
                if (Math.abs(World().player.center(0) - this.center(0)) < 40) {
                    this.fsmTransitionTo(States.Pop);
                }
            },
            canCollide: (other, dir) => CollisionType.Disabled,
        },
        [States.Pop]: {
            enter: () => {
                this.vel[1] = -3;
                this.frame = Frames.Rise;
            },
            update: (ticks) => {
                this.vel[1] += Phys.enemyGravity;
                this.moveY(this.vel[1]);
            },
            canCollide: (other, dir) => {
                if (other instanceof Player && dir == Dir.Up)
                    return CollisionType.Platform;
                else if (other instanceof Player)
                    return CollisionType.Trigger;
                else
                    return CollisionType.Disabled;
            }
        },
        [States.Recharge]: {
            enter: () => {
                this.fsmTransitionAtTime(States.Wait, 120);
                this.frame = Frames.Idle;
            },
            canCollide: (other, dir) => CollisionType.Disabled,
        }
    };
    update(ticks, dt) {
        this.fsmUpdate(this.#states, ticks);
    }
    canCollide(other, dir) {
        return this.fsmCanCollide(this.#states, other, dir);
    }
    collide(other, dir) {
        this.handlePlayerStomp(other, dir);
        if (other instanceof WorldEnt) {
            this.fsmTransitionTo(States.Recharge);
        }
    }
}
export { Daikon };
