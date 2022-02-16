import * as Assets from 'assets';
import { Layer } from '../entity.js';
import CollisionType from '../collisiontype.js';
import { World } from '../game.js';
import { Player } from './player.js';
import { FSMEntity } from '../fsmentity.js';
import { invLerp } from '../util.js';
var Frames;
(function (Frames) {
    Frames[Frames["Idle1"] = 0] = "Idle1";
    Frames[Frames["Idle2"] = 1] = "Idle2";
    Frames[Frames["Idle3"] = 2] = "Idle3";
    Frames[Frames["Activate1"] = 3] = "Activate1";
    Frames[Frames["Activate2"] = 4] = "Activate2";
    Frames[Frames["Activate3"] = 5] = "Activate3";
    Frames[Frames["Active1"] = 6] = "Active1";
    Frames[Frames["Active2"] = 7] = "Active2";
    Frames[Frames["Active3"] = 8] = "Active3";
})(Frames || (Frames = {}));
var States;
(function (States) {
    States[States["None"] = 0] = "None";
    States[States["Idle"] = 1] = "Idle";
    States[States["Bounce"] = 2] = "Bounce";
    States[States["Active"] = 3] = "Active";
})(States || (States = {}));
class Checkpoint extends FSMEntity {
    type = 'Checkpoint';
    drawOfs = [-1, 0];
    collidable = CollisionType.Trigger;
    sprite = Assets.find('checkpoint');
    layer = Layer.Background;
    activated = false;
    idleAnim = [Frames.Idle1, Frames.Idle2, Frames.Idle3];
    bounceAnim = [Frames.Activate1, Frames.Activate2, Frames.Activate3];
    activeAnim = [Frames.Active1, Frames.Active2, Frames.Active3];
    #states = {
        default: {
            enter: () => this.fsmDefaultTransitionTo(States.Idle),
        },
        [States.Idle]: {
            update: (ticks) => this.frame = this.idleAnim[Math.floor(ticks / 8) % this.idleAnim.length],
            collide: (other, dir) => {
                if (other instanceof Player == false) {
                    return;
                }
                this.activated = true;
                World().state.checkpointActive = true;
                World().state.checkpointPos = [this.pos[0], this.pos[1] + this.size[1]];
                World().spawnPuffParticle(this.pos[0], this.pos[1]);
                this.fsmTransitionTo(States.Bounce);
            }
        },
        [States.Bounce]: {
            enter: () => this.fsmTransitionAtTime(States.Active, 12),
            update: (ticks) => this.frame = this.bounceAnim[Math.floor(invLerp(this.startStateTime, this.nextStateTime, ticks) * this.activeAnim.length)],
        },
        [States.Active]: {
            update: (ticks) => this.frame = this.activeAnim[Math.floor(ticks / 8) % this.activeAnim.length],
        },
    };
    constructor(args) {
        super(args);
    }
    update(ticks, dt) {
        this.fsmUpdate(this.#states, ticks);
    }
    collide(other, dir) {
        this.fsmCollide(this.#states, other, dir);
    }
}
export { Checkpoint };
//# sourceMappingURL=checkpoint.js.map