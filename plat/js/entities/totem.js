import * as Assets from 'assets';
import Entity from '../entity.js';
import { World } from '../game.js';
var Frames;
(function (Frames) {
    Frames[Frames["Idle"] = 0] = "Idle";
    Frames[Frames["Walk1"] = 1] = "Walk1";
    Frames[Frames["Walk2"] = 2] = "Walk2";
    Frames[Frames["Half1"] = 3] = "Half1";
    Frames[Frames["Half2"] = 4] = "Half2";
    Frames[Frames["Pain"] = 5] = "Pain";
    Frames[Frames["Chunk"] = 6] = "Chunk";
})(Frames || (Frames = {}));
class Totem extends Entity {
    type = 'Totem';
    sprite = Assets.find('totem');
    vel = [-0.25, 0];
    drawOfs = [-2, -2];
    walkAnim = [Frames.Walk1, Frames.Walk2];
    halfWalkAnim = [Frames.Half1, Frames.Half2];
    half = false;
    die() {
        super.die();
        World().spawnDeathParticle(this, Frames.Pain);
    }
    update(ticks, dt) {
        const frames = this.half ? this.halfWalkAnim : this.walkAnim;
        const animSpeed = this.half ? 4 : 8;
        this.frame = frames[Math.floor(ticks / animSpeed) % frames.length];
        this.moveX(this.vel[0]);
        this.flipBits = this.vel[0] > 0 ? 1 : 0;
    }
    canCollide = this.standardCanEnemyCollide;
    collide(other, dir) {
        if (this.handlePlayerStomp(other, dir, false)) {
            if (!this.half) {
                const wood = World().spawnDeathParticle(this, Frames.Chunk);
                wood.size = [13, 13];
                this.half = true;
                this.vel[0] *= 3;
                this.size[1] -= 6;
                this.pos[1] += 6;
                this.drawOfs[1] -= 6;
            }
            else {
                this.die();
            }
        }
        else {
            this.vel[0] *= -1;
        }
    }
}
export { Totem };
//# sourceMappingURL=totem.js.map