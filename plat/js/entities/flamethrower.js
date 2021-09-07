import * as Assets from 'assets';
import Entity from '../entity.js';
import CollisionType from '../collisiontype.js';
import { World } from '../game.js';
class Flamethrower extends Entity {
    type = 'Flamethrower';
    sprite = Assets.find('flamethrower');
    collidable = CollisionType.Enabled;
    startOn;
    spawned = false;
    constructor(args) {
        super(args);
        this.startOn = args.properties?.StartOn ?? false;
    }
    preupdate(ticks, dt) {
        if (this.spawned)
            return;
        this.spawned = true;
        const ent = World().spawnEntity('Flame');
        ent.startOn = this.startOn;
        ent.pos = [...this.pos];
        ent.pos[1] -= ent.size[1];
    }
    die() { }
}
export { Flamethrower };
