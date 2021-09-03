import * as Assets from 'assets';
import Dir from '../dir.js';
import CollisionType from '../collisiontype.js';
import { Player } from './player.js';
import Phys from '../phys.js';
import World from '../world.js';
import FSMEntity from '../fsmentity.js';
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
    States[States["ShellSpin"] = 3] = "ShellSpin";
})(States || (States = {}));
class Hermit extends FSMEntity {
    type = 'Hermit';
    drawOfs = [-4, -1];
    sprite = Assets.find('hermit');
    state = States.Walk;
    nextState = States.None;
    nextStateTime = 0;
    spinSpeed = 3;
    walkSpeed = 0.25;
    constructor(args) {
        super(args);
        this.flipBits = 1;
    }
    die() {
        super.die();
        World().spawnDeathParticle(this, Frames.Shell);
    }
    #states = {
        default: {
            enter: () => this.fsmDefaultTransitionTo(States.Walk),
        },
        [States.Walk]: {
            update: (ticks) => {
                this.frame = (ticks / 8) % 2;
                if (Math.abs(this.vel[0]) != this.walkSpeed)
                    this.vel[0] = -this.walkSpeed;
            },
            canCollide: this.standardCanEnemyCollide,
            collide: (other, dir) => {
                if (this.handlePlayerStomp(other, dir, false)) {
                    this.fsmTransitionTo(States.Shell);
                }
                if (other instanceof Player == false) {
                    if (this.vel[0] < 0 && dir == Dir.Left)
                        this.vel[0] *= -1;
                    else if (this.vel[0] > 0 && dir == Dir.Right)
                        this.vel[0] *= -1;
                    if (dir == Dir.Up || dir == Dir.Down)
                        this.vel[1] = 0;
                }
            }
        },
        [States.Shell]: {
            enter: () => {
                this.fsmTransitionAtTime(States.Walk, 300);
                this.frame = Frames.Shell;
                this.vel[0] = 0;
            },
            canCollide: (other, dir) => CollisionType.Enabled,
            collide: (other, dir) => {
                // FIXME: this is still not good, see handleEnemyStomp
                if (other instanceof Player && other.max(1) <= this.min(1))
                    other.stompEnemy();
                this.vel[0] = this.vel[0] == 0 ? this.spinSpeed : 0;
                this.vel[0] *= this.center(0) - other.center(0) > 0 ? 1 : -1;
                this.fsmTransitionTo(States.ShellSpin);
            }
        },
        [States.ShellSpin]: {
            enter: () => {
                this.fsmTransitionAtTime(States.Shell, 0);
                this.frame = Frames.Shell;
            },
            canCollide: this.standardCanEnemyCollide,
            collide: (other, dir) => {
                if (other instanceof Player && other.max(1) <= this.min(1)) {
                    other.stompEnemy();
                    this.fsmTransitionTo(States.Shell);
                    return;
                }
                other.hurt(1);
                // if we can't destroy it, bounce back
                if (!other.destroyed && other instanceof Player == false && (dir == Dir.Left || dir == Dir.Right)) {
                    this.vel[0] *= -1;
                }
            }
        }
    };
    update(ticks, dt) {
        let grounded = this.vel[1] >= 0 && this.collideAt(this.pos[0], this.pos[1] + 1, Dir.Down);
        this.vel[1] = grounded ? 0 : this.vel[1] + Phys.enemyGravity;
        super.fsmUpdate(this.#states, ticks);
        this.moveX(this.vel[0]);
        this.moveY(this.vel[1]);
        if (this.vel[0] != 0)
            this.flipBits = this.vel[0] < 0 ? 1 : 0;
    }
    canCollide(other, dir) {
        return super.fsmCanCollide(this.#states, other, dir);
    }
    collide(other, dir) {
        super.fsmCollide(this.#states, other, dir);
    }
}
export { Hermit };
