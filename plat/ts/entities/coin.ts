import * as SLT from 'slate2d';
import * as Draw from 'draw';
import * as Assets from 'assets';

import Entity from '../entity.js';
import Dir from '../dir.js';
import CollisionType from '../collisiontype.js';
import World from '../world.js';
import { Player } from './player.js';

class Coin extends Entity {
  collidable = CollisionType.Trigger;
  sprite = Assets.find('coin');

  constructor(args: { [key: string]: any }) {
    super(args);
  }

  update(ticks: number, dt: number) {
    this.frame = ticks / 8 % 4;
  }

  collide(other: Entity, dir: Dir) {
    if (other instanceof Player == false) {
      return;
    }

    this.destroyed = true;
    World().state.currCoins++;
  }
}

export { Coin };