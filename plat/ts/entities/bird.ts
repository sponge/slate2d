import * as SLT from 'slate2d';
import * as Draw from 'draw';
import * as Assets from 'assets';

import Entity from '../entity.js';
import Dir from '../dir.js';
import CollisionType from '../collisiontype.js';
import World from '../world.js';
import Player from './player.js';
import { dbg, dbgval } from '../printwin.js';

class Bird extends Entity {
  collidable = CollisionType.Trigger;
  drawOfs: [number, number] = [-1, -2];
  sprite = Assets.find('bird');
  enabled = true;
  delta: [number, number] = [0, 0];

  constructor(args: { [key: string]: any }) {
    super(args);

    // points are kinda hard to work with and they're not in any format
    // that maps cleanly to the entity position
    const dest = args.properties?.Destination ?? this.pos;
    const tilePos = [Math.floor(this.pos[0] / 16), Math.floor(this.pos[1] / 16)];
    this.delta[0] = (dest[0] - tilePos[0]) * 16;
    this.delta[1] = (dest[1] - tilePos[1]) * 16;
  }

  update(ticks: number, dt: number) {
    this.frame = Math.floor(ticks / 8 % 4);
    this.frame = this.frame == 3 ? 1 : this.frame;

    const dir = [Math.sign(this.delta[0]), Math.sign(this.delta[1])];
    this.moveX(dir[0]);
    this.moveY(dir[1]);
  }

  draw() {
    SLT.showObj('bird', this);
    super.draw();
  }

  collide(other: Entity, dir: Dir) {
    if (other instanceof Player == false) {
      return;
    }

    this.destroyed = true;
  }
}

export default Bird;