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
  drawOfs: [number, number] = [-1, -2];
  sprite = Assets.find('bird');
  delta: [number, number] = [0, 0];
  dir: [number, number] = [0, 0];
  moveAmt: [number, number] = [0, 0];

  constructor(args: { [key: string]: any }) {
    super(args);

    // points are kinda hard to work with and they're not in any format
    // that maps cleanly to the entity position
    const dest = args.properties?.Destination ?? this.pos;
    const tilePos = [Math.floor(this.pos[0] / 16), Math.floor(this.pos[1] / 16)];

    this.delta[0] = (dest[0] - tilePos[0]) * 16;
    this.delta[1] = (dest[1] - tilePos[1]) * 16;

    const absDelta = [Math.abs(this.delta[0]), Math.abs(this.delta[1])];

    let dx = absDelta[0] > absDelta[1] ? 1 : absDelta[0] / absDelta[1];
    let dy = absDelta[1] > absDelta[0] ? 1 : absDelta[1] / absDelta[0];

    dx *= Math.sign(this.delta[0]) * 0.5;
    dy *= Math.sign(this.delta[1]) * 0.5;

    this.dir = [dx, dy];
  }

  canCollide(other: Entity, dir: Dir) {
    if (other instanceof Player && !other.stunned && dir == Dir.Up) return CollisionType.Enabled;
    else return CollisionType.Trigger;
  }

  update(ticks: number, dt: number) {
    this.frame = Math.floor(ticks / 8 % 4);
    this.frame = this.frame == 3 ? 1 : this.frame;

    this.moveX(this.dir[0]);
    this.moveY(this.dir[1]);
    this.moveAmt[0] += this.dir[0];
    this.moveAmt[1] += this.dir[1]

    const dim = this.delta[0] > this.delta[1] ? 0 : 1;
    if (Math.abs(this.moveAmt[dim]) >= Math.abs(this.delta[dim])) {
      this.dir[0] *= -1;
      this.dir[1] *= -1;
      this.moveAmt = [0, 0];
    }
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
  }
}

export default Bird;