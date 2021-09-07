import * as SLT from 'slate2d';
import * as Draw from 'draw';
import * as Assets from 'assets';

import Entity from '../entity.js';
import Dir from '../dir.js';
import CollisionType from '../collisiontype.js';
import { World } from '../game.js';
import { Player } from './player.js';

class LevelEnd extends Entity {
  type = 'LevelEnd';
  drawOfs: [number, number] = [-2, -2];
  collidable = CollisionType.Trigger;
  sprite = Assets.find('levelend');

  constructor(args: { [key: string]: any }) {
    super(args);
  }

  update(ticks: number, dt: number) {
    this.frame = ticks / 8 % 8;
  }

  collide(other: Entity, dir: Dir) {
    if (other instanceof Player == false) {
      return;
    }

    this.destroyed = true;
    World().completeLevel();
  }

  draw() {
    this.drawOfs[1] = this.frame < 4 ? -3 : -4;
    super.draw();
  }
}

export { LevelEnd };