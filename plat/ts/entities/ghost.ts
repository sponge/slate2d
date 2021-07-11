import * as SLT from 'slate2d';
import * as Assets from 'assets';

import Entity from '../entity.js';
import Dir from '../dir.js';
import CollisionType from '../collisiontype.js';
import { Player } from './player.js';
import Phys from '../phys.js';
import World from '../world.js';

enum Frames {
  Float1,
  Float2,
  Float3,
  Float4,
  Pain,
}

class Ghost extends Entity {
  drawOfs: [number, number] = [-2, -1];
  sprite = Assets.find('ghost');

  constructor(args: { [key: string]: any }) {
    super(args);
    this.flipBits = 1;
  }

  die() {
    super.die();
    World().spawnDeathParticle(this, Frames.Pain);
  }

  canCollide(other: Entity, dir: Dir) {
    if (other instanceof Player && !other.stunned && dir == Dir.Up) return CollisionType.Enabled;
    else return CollisionType.Trigger;
  }

  update(ticks: number, dt: number) {

    this.frame = Math.floor(ticks / 8 % 4);
  }

  collide(other: Entity, dir: Dir) {
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