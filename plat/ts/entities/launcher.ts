import * as SLT from 'slate2d';
import * as Assets from 'assets';

import Entity from '../entity.js';
import Dir from '../dir.js';
import CollisionType from '../collisiontype.js';
import { Player } from './player.js';
import Phys from '../phys.js';
import World from '../world.js';

class Launcher extends Entity {
  sprite = Assets.find('launcher');
  collidable = CollisionType.Enabled;
  launchTime = 180;
  launchWait = 180;

  die() { }

  update(ticks: number, dt: number) {
    if (ticks < this.launchTime) {
      return;
    }

    const ent = World().spawnEntity('Cannonball');
    World().spawnPuffParticle(this.pos[0] - 8, this.pos[1]);
    ent.vel = [-1, 0];
    ent.pos = [this.pos[0] - 16, this.pos[1] + 1];
    this.launchTime = ticks + this.launchWait;
  }
}

export { Launcher };