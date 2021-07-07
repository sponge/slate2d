import * as SLT from 'slate2d';
import * as Assets from 'assets';

import Entity from '../entity.js';
import Dir from '../dir.js';
import CollisionType from '../collisiontype.js';
import Player from './player.js';
import Phys from '../phys.js';

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
  collidable = CollisionType.Platform;
  drawOfs: [number, number] = [-2, -5];
  sprite = Assets.find('bub');

  constructor(args: { [key: string]: any }) {
    super(args);
    this.flipBits = 1;
    this.vel[0] = args.properties?.GoRight ?? true ? 0.25 : -0.25;
    this.flipBits = this.vel[0] > 0 ? 1 : 0;
  }

  update(ticks: number, dt: number) {
    let grounded = this.vel[1] >= 0 && this.collideAt(this.pos[0], this.pos[1] + 1, Dir.Down);

    this.vel[1] = grounded ? 0 : this.vel[1] + Phys.enemyGravity;

    if (!this.moveX(this.vel[0])) {
      this.vel[0] *= -1;
    }

    if (!this.moveY(this.vel[1])) {
      this.vel[1] = 0;
    }

    this.flipBits = this.vel[0] > 0 ? 1 : 0;

    if (this.vel[1] != 0) {
      this.frame = Frames.Pain;
    }
    else if (grounded) {
      this.frame = (ticks / 8) % (Frames.Run7 - Frames.Run1) + Frames.Run1;
    }
  }

  collide(other: Entity, dir: Dir) {
    if (other instanceof Player == false) {
      return;
    }

    this.destroyed = true;
  }
}

export default Bub;