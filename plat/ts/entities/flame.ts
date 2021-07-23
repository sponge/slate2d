import * as SLT from 'slate2d';
import * as Assets from 'assets';

import Entity from '../entity.js';
import Dir from '../dir.js';
import CollisionType from '../collisiontype.js';
import { Player } from './player.js';
import Phys from '../phys.js';
import World from '../world.js';
import { dbgval } from '../printwin.js';

class Flame extends Entity {
  type = 'Flame';
  size: [number, number] = [16, 47];
  sprite = Assets.find('flame');
  collidable = CollisionType.Trigger;
  startOn = false;

  die() { }

  update(ticks: number, dt: number) {
    const cyc = (ticks + (this.startOn ? 0 : 240)) % 480;
    const frame = cyc % 8 < 4 ? 0 : 1;
    const offset = Math.abs(270 - cyc);

    if (offset < 60) {
      this.frame = 5 + frame;
    }
    else if (offset < 70) {
      this.frame = 3 + frame;
    }
    else if (offset < 100) {
      this.frame = 1 + frame;
    }
    else {
      this.frame = 0;
    }
  }

  collide(other: Entity, dir: Dir) {
    if (other instanceof Player == false) {
      return;
    }

    // can't hurt if not on
    if (this.frame != 5 && this.frame != 6) {
      return;
    }

    if (other.canHurt(this)) {
      other.hurt(1);
    }
  }
}

export { Flame };