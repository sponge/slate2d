import * as Assets from 'assets';
import World from '../world.js';
import FSMEntity from '../fsmentity.js';
var Frames;
(function (Frames) {
    Frames[Frames["Idle1"] = 0] = "Idle1";
    Frames[Frames["Idle2"] = 1] = "Idle2";
    Frames[Frames["FlapUp"] = 2] = "FlapUp";
    Frames[Frames["FlapMid"] = 3] = "FlapMid";
    Frames[Frames["FlapDown"] = 4] = "FlapDown";
    Frames[Frames["Pain"] = 5] = "Pain";
})(Frames || (Frames = {}));
var States;
(function (States) {
    States[States["None"] = 0] = "None";
    States[States["Idle"] = 1] = "Idle";
    States[States["Rise"] = 2] = "Rise";
    States[States["Float"] = 3] = "Float";
    States[States["Fall"] = 4] = "Fall";
})(States || (States = {}));
class Owl extends FSMEntity {
    type = 'Owl';
    sprite = Assets.find('owl');
    drawOfs = [-3, -1];
    flapAnim = [Frames.FlapMid, Frames.FlapUp, Frames.FlapMid, Frames.FlapDown];
    fallAnim = [Frames.FlapMid, Frames.FlapDown];
    state = States.Idle;
    nextState = States.None;
    nextStateTime = 0;
    die() {
        super.die();
        World().spawnDeathParticle(this, Frames.Pain);
    }
    #states = {
        default: {
            enter: () => this.fsmDefaultTransitionTo(States.Idle),
        },
        [States.Idle]: {
            enter: (ticks) => this.fsmTransitionAtTime(States.Rise, 120),
            update: (ticks) => this.frame = ticks % 40 < 30 ? Frames.Idle1 : Frames.Idle2,
        },
        [States.Rise]: {
            enter: () => this.fsmTransitionAtTime(States.Float, 60),
            update: (ticks) => {
                this.frame = this.flapAnim[Math.floor(ticks / 4) % this.flapAnim.length];
                this.moveY(-1);
            }
        },
        [States.Float]: {
            enter: () => this.fsmTransitionAtTime(States.Fall, 90),
            update: (ticks) => this.frame = this.flapAnim[Math.floor(ticks / 8) % this.flapAnim.length],
        },
        [States.Fall]: {
            update: (ticks) => {
                if (!this.moveY(0.5)) {
                    this.fsmTransitionTo(States.Idle);
                }
                this.frame = this.fallAnim[Math.floor(ticks / 16) % this.fallAnim.length];
            }
        }
    };
    update(ticks, dt) {
        this.fsmUpdate(this.#states, ticks);
    }
    canCollide = this.standardCanEnemyCollide;
    collide(other, dir) {
        this.handlePlayerStomp(other, dir);
    }
}
export { Owl };
