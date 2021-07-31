import * as Assets from 'assets';

import Entity from '../entity.js';
import Dir from '../dir.js';
import CollisionType from '../collisiontype.js';
import { Player } from './player.js';
import World from '../world.js';

class CrumblingBlock extends Entity {
  type = 'CrumblingBlock';
  sprite = Assets.find('crumblingblock');
  collidable = CollisionType.Enabled;
  broken = false;
  breakTime = 0;
  crumbleTime = 40;
  numFrames = 4;

  die() { }

  update(ticks: number, dt: number) {
    if (!this.broken) return;

    this.frame = Math.ceil((ticks - (this.breakTime - this.crumbleTime)) / (this.crumbleTime / this.numFrames));
    if (ticks < this.breakTime) return;

    this.destroyed = true;
    const part = World().spawnDeathParticle(this, 4);
    part.vel[0] = 0;
    part.vel[1] = 0;
  }

  collide(other: Entity, dir: Dir) {
    if (other instanceof Player && dir == Dir.Up && !this.broken) {
      this.broken = true;
      this.breakTime = World().state.ticks + this.crumbleTime;
    }
  }
}

export { CrumblingBlock };