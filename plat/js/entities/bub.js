import * as Assets from 'assets';
import Entity from '../entity.js';
import Dir from '../dir.js';
import CollisionType from '../collisiontype.js';
import { Player } from './player.js';
import Phys from '../phys.js';
import World from '../world.js';
var Frames;
(function (Frames) {
    Frames[Frames["Idle"] = 0] = "Idle";
    Frames[Frames["Pain"] = 1] = "Pain";
    Frames[Frames["Run1"] = 2] = "Run1";
    Frames[Frames["Run2"] = 3] = "Run2";
    Frames[Frames["Run3"] = 4] = "Run3";
    Frames[Frames["Run4"] = 5] = "Run4";
    Frames[Frames["Run5"] = 6] = "Run5";
    Frames[Frames["Run6"] = 7] = "Run6";
    Frames[Frames["Run7"] = 8] = "Run7";
})(Frames || (Frames = {}));
class Bub extends Entity {
    type = 'Bub';
    drawOfs = [-2, -5];
    spikey = false;
    chargin = false;
    sprite = Assets.find('bub');
    constructor(args) {
        super(args);
        this.vel[0] = args.properties?.GoRight ?? true ? 0.25 : -0.25;
        this.flipBits = this.vel[0] > 0 ? 1 : 0;
    }
    die() {
        super.die();
        World().spawnDeathParticle(this, Frames.Pain);
    }
    canCollide(other, dir) { return this.standardCanEnemyCollide(other, dir); }
    ;
    update(ticks, dt) {
        let grounded = this.vel[1] >= 0 && this.collideAt(this.pos[0], this.pos[1] + 1, Dir.Down);
        this.vel[1] = grounded ? 0 : this.vel[1] + Phys.enemyGravity;
        const extra = this.chargin && Math.abs(this.center(1) - World().player.center(1)) < 10 ? 3 : 1;
        this.moveX(this.vel[0] * extra);
        this.moveY(this.vel[1]);
        this.flipBits = this.vel[0] > 0 ? 1 : 0;
        if (this.vel[1] != 0) {
            this.frame = Frames.Pain;
        }
        else if (grounded) {
            const frameSpeed = extra > 1 ? 4 : 8;
            this.frame = (ticks / frameSpeed) % (Frames.Run7 - Frames.Run1) + Frames.Run1;
        }
    }
    collide(other, dir) {
        if (other instanceof Player) {
            if (!this.spikey && other.canHurt(this) && dir == Dir.Up && other.max(1) <= this.center(1)) {
                other.stompEnemy();
                this.die();
            }
            else {
                other.hurt(1);
            }
        }
        else {
            if (dir == Dir.Left || dir == Dir.Right)
                this.vel[0] *= -1;
            if (dir == Dir.Up || dir == Dir.Down)
                this.vel[1] = 0;
        }
    }
}
class SpikeyBub extends Bub {
    type = 'SpikeyBub';
    spikey = true;
    sprite = Assets.find('spikeybub');
    canCollide(other, dir) {
        if (other instanceof Player)
            return CollisionType.Trigger;
        else
            return CollisionType.Enabled;
    }
}
class CharginBub extends Bub {
    type = 'CharginBub';
    chargin = true;
    sprite = Assets.find('charginbub');
}
export { Bub, SpikeyBub, CharginBub };
