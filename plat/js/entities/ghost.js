import * as Assets from 'assets';
import Entity from '../entity.js';
import Dir from '../dir.js';
import CollisionType from '../collisiontype.js';
import { Player } from './player.js';
import World from '../world.js';
var Frames;
(function (Frames) {
    Frames[Frames["Float1"] = 0] = "Float1";
    Frames[Frames["Float2"] = 1] = "Float2";
    Frames[Frames["Float3"] = 2] = "Float3";
    Frames[Frames["Float4"] = 3] = "Float4";
    Frames[Frames["Pain"] = 4] = "Pain";
})(Frames || (Frames = {}));
class Ghost extends Entity {
    drawOfs = [-2, -1];
    sprite = Assets.find('ghost');
    constructor(args) {
        super(args);
        this.flipBits = 1;
    }
    die() {
        super.die();
        World().spawnDeathParticle(this, Frames.Pain);
    }
    canCollide(other, dir) {
        if (other instanceof Player && !other.stunned && dir == Dir.Up)
            return CollisionType.Enabled;
        else
            return CollisionType.Trigger;
    }
    update(ticks, dt) {
        this.frame = Math.floor(ticks / 8 % 4);
    }
    collide(other, dir) {
        if (other instanceof Player) {
            if (!other.stunned && dir == Dir.Up && other.max(1) <= this.center(1)) {
                other.stompEnemy();
                this.die();
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
export { Ghost };
