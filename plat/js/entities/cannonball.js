import * as Assets from 'assets';
import Entity from '../entity.js';
import Dir from '../dir.js';
import CollisionType from '../collisiontype.js';
import { Player } from './player.js';
import World from '../world.js';
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
class Cannonball extends Entity {
    type = 'Cannonball';
    sprite = Assets.find('launcher');
    frame = 1;
    constructor(args) {
        super(args);
        this.vel[0] = args.properties?.GoRight ?? true ? 0.25 : -0.25;
        this.flipBits = this.vel[0] > 0 ? 1 : 0;
    }
    die() {
        super.die();
        World().spawnDeathParticle(this, 1);
    }
    canCollide(other, dir) {
        if (other instanceof Player && other.canHurt(this) && dir == Dir.Up)
            return CollisionType.Enabled;
        else if (other instanceof Player)
            return CollisionType.Trigger;
        else
            return CollisionType.Enabled;
    }
    update(ticks, dt) {
        this.moveX(this.vel[0]);
        this.moveY(this.vel[1]);
    }
    collide(other, dir) {
        console.log('collide!');
        if (other instanceof Player) {
            if (other.canHurt(this) && dir == Dir.Up && other.max(1) <= this.center(1)) {
                other.stompEnemy();
            }
            else {
                other.hurt(1);
            }
        }
        this.die();
    }
}
export { Cannonball };
