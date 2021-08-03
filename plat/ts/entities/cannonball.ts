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

class Cannonball extends Entity {
  type = 'Cannonball';
  sprite = Assets.find('launcher');
  frame = 1;
  size: [number, number] = [14, 14];
  drawOfs: [number, number] = [-2, -2];

  constructor(args: { [key: string]: any }) {
    super(args);
    this.vel[0] = args.properties?.GoRight ?? true ? 0.25 : -0.25;
  }

  die() {
    super.die();
    const deathEnt = World().spawnDeathParticle(this, 1);
    deathEnt.vel = [0, 0];
    deathEnt.rotate = false;
  }

  canCollide = this.standardCanEnemyCollide;

  update(ticks: number, dt: number) {
    this.moveX(this.vel[0]);
    this.moveY(this.vel[1]);
  }

  collide(other: Entity, dir: Dir) {
    this.handlePlayerStomp(other, dir);
    this.die();
  }
}

export { Cannonball };