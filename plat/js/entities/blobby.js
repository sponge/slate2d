import * as Assets from 'assets';
import Entity from '../entity.js';
import Dir from '../dir.js';
import { Player } from './player.js';
import World from '../world.js';
import { invLerp } from '../util.js';
import CollisionType from '../collisiontype.js';
var Frames;
(function (Frames) {
    Frames[Frames["Idle1"] = 0] = "Idle1";
    Frames[Frames["Idle2"] = 1] = "Idle2";
    Frames[Frames["Sink1"] = 2] = "Sink1";
    Frames[Frames["Sink2"] = 3] = "Sink2";
    Frames[Frames["Sunk"] = 4] = "Sunk";
    Frames[Frames["Pain"] = 5] = "Pain";
})(Frames || (Frames = {}));
var States;
(function (States) {
    States[States["None"] = 0] = "None";
    States[States["Idle"] = 1] = "Idle";
    States[States["Sink"] = 2] = "Sink";
    States[States["Rise"] = 3] = "Rise";
    States[States["Move"] = 4] = "Move";
})(States || (States = {}));
class Blobby extends Entity {
    type = 'Blobby';
    sprite = Assets.find('blobby');
    drawOfs = [-1, -2];
    sinkAnim = [Frames.Sink1, Frames.Sink2];
    riseAnim = [Frames.Sink2, Frames.Sink1];
    state = States.Idle;
    nextState = States.None;
    nextStateTime = 0;
    startStateTime = 0;
    die() {
        super.die();
        World().spawnDeathParticle(this, Frames.Pain);
    }
    update(ticks, dt) {
        if (this.nextStateTime > 0 && ticks >= this.nextStateTime) {
            this.state = this.nextState;
            this.nextState = States.None;
        }
        switch (this.state) {
            case States.None:
                this.state = States.Idle;
                this.nextStateTime = ticks + 90;
                break;
            case States.Idle:
                if (this.nextState == States.None) {
                    this.nextState = States.Sink;
                    this.nextStateTime = ticks + 80;
                    this.startStateTime = ticks;
                }
                this.frame = ticks % 40 <= 20 ? Frames.Idle1 : Frames.Idle2;
                break;
            case States.Sink:
                if (this.nextState == States.None) {
                    this.nextState = States.Move;
                    this.nextStateTime = ticks + 20;
                    this.startStateTime = ticks;
                }
                this.frame = this.sinkAnim[Math.floor(invLerp(this.startStateTime, this.nextStateTime, ticks) * this.sinkAnim.length)];
                break;
            case States.Rise:
                if (this.nextState == States.None) {
                    this.nextState = States.Idle;
                    this.nextStateTime = ticks + 20;
                    this.startStateTime = ticks;
                }
                this.frame = this.riseAnim[Math.floor(invLerp(this.startStateTime, this.nextStateTime, ticks) * this.riseAnim.length)];
                break;
            case States.Move:
                if (this.nextState == States.None) {
                    this.nextState = States.Rise;
                    this.nextStateTime = ticks + 90;
                    this.startStateTime = ticks;
                }
                this.frame = Frames.Sunk;
                break;
        }
    }
    canCollide(other, dir) {
        switch (this.state) {
            case States.Move:
                if (other instanceof Player)
                    return CollisionType.Trigger;
                else
                    return CollisionType.Enabled;
            default:
                return this.standardCanEnemyCollide(other, dir);
        }
    }
    collide(other, dir) {
        switch (this.state) {
            case States.Move:
                if (other instanceof Player) {
                    other.hurt(1);
                }
                break;
            default:
                if (other instanceof Player && dir == Dir.Up && other.max(1) <= this.min(1)) {
                    this.die();
                    other.stompEnemy();
                }
                else if (other instanceof Player) {
                    other.hurt(1);
                }
                break;
        }
    }
}
export { Blobby };
