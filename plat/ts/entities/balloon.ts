import * as Assets from 'assets';

import Entity from '../entity.js';
import Dir from '../dir.js';
import CollisionType from '../collisiontype.js';
import { Player } from './player.js';
import World from '../world.js';

class Balloon extends Entity {
  sprite = Assets.find('balloon');

  update(ticks: number, dt: number) {
    this.frame = ticks % 26 < 13 ? 0 : 1;
    const cycle = ticks % 120;
    if (cycle == 0 || cycle == 30) {
      this.moveSolid(0, 1);
    }
    else if (cycle == 60 || cycle == 90) {
      this.moveSolid(0, -1);
    }
  }

  canCollide = this.standardCanEnemyCollide;

  collide(other: Entity, dir: Dir) {
    if (other instanceof Player && dir == Dir.Up && other.canHurt(this)) {
      this.die();
      other.stompEnemy();
      World().spawnPuffParticle(this.pos[0], this.pos[1]);
    }
  }
}

export { Balloon };