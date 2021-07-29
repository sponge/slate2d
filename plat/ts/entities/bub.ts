import * as SLT from 'slate2d';
import * as Assets from 'assets';

import Entity from '../entity.js';
import Dir from '../dir.js';
import CollisionType from '../collisiontype.js';
import { Player } from './player.js';
import Phys from '../phys.js';
import World from '../world.js';

enum Frames {
  Idle = 0,
  Blink,
  Pain,
  Run1,
  Run2,
  Run3,
  Run4,
  Run5,
  Run6,
  Run7,
  Squish
}

class Bub extends Entity {
  type = 'Bub';
  drawOfs: [number, number] = [-2, -5];
  sprite = Assets.find('bub');

  constructor(args: { [key: string]: any }) {
    super(args);
    this.vel[0] = args.properties?.GoRight ?? true ? 0.25 : -0.25;
    this.flipBits = this.vel[0] > 0 ? 1 : 0;
  }

  die() {
    super.die();
    World().spawnDeathParticle(this, Frames.Pain);
  }

  canCollide = this.standardCanEnemyCollide;

  update(ticks: number, dt: number) {
    let grounded = this.vel[1] >= 0 && this.collideAt(this.pos[0], this.pos[1] + 1, Dir.Down);

    this.vel[1] = grounded ? 0 : this.vel[1] + Phys.enemyGravity;

    this.moveX(this.vel[0]);
    this.moveY(this.vel[1]);

    this.flipBits = this.vel[0] > 0 ? 1 : 0;

    if (this.vel[1] != 0) {
      this.frame = Frames.Pain;
    }
    else if (grounded) {
      this.frame = (ticks / 8) % (Frames.Run7 - Frames.Run1) + Frames.Run1;
    }
  }

  collide(other: Entity, dir: Dir) {
    if (other instanceof Player) {
      if (other.canHurt(this) && dir == Dir.Up && other.max(1) <= this.center(1)) {
        other.stompEnemy();
        this.die();
      }
      else {
        other.hurt(1);
      }
    }
    else {
      if (dir == Dir.Left || dir == Dir.Right) this.vel[0] *= -1;
      if (dir == Dir.Up || dir == Dir.Down) this.vel[1] = 0;
    }
  }
}

export { Bub };