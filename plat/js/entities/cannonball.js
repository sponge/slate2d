import * as Assets from 'assets';
import Entity from '../entity.js';
import { World } from '../game.js';
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
    size = [14, 14];
    drawOfs = [-2, -2];
    constructor(args) {
        super(args);
        this.vel[0] = args.properties?.GoRight ?? true ? 0.25 : -0.25;
    }
    die() {
        super.die();
        const deathEnt = World().spawnDeathParticle(this, 1);
        deathEnt.vel = [0, 0];
        deathEnt.rotate = false;
    }
    canCollide = this.standardCanEnemyCollide;
    update(ticks, dt) {
        this.moveX(this.vel[0]);
        this.moveY(this.vel[1]);
    }
    collide(other, dir) {
        this.handlePlayerStomp(other, dir);
        this.die();
    }
}
export { Cannonball };
//# sourceMappingURL=cannonball.js.map