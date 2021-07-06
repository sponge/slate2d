import * as SLT from 'slate2d';
import * as Draw from 'draw';
import * as Assets from 'assets';

import Entity from '../entity.js';
import Dir from '../dir.js';
import CollisionType from '../collisiontype.js';
import World from '../world.js';
import Player from './player.js';
import { dbg, dbgval } from '../printwin.js';

class Slime extends Entity {
  collidable = CollisionType.Trigger;
  drawOfs: [number, number] = [-1, -2];
  sprite = Assets.find('slime');
  enabled = true;

  constructor(args: { [key: string]: any }) {
    super(args);
  }

  update(ticks: number, dt: number) {
    this.frame = ticks % 40 > 30 ? 1 : 0;
  }

  draw() {
    super.draw();
  }

  collide(other: Entity, dir: Dir) {
    if (other instanceof Player == false) {
      return;
    }

    this.destroyed = true;
  }
}

export default Slime;