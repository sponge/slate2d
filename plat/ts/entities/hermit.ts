import * as SLT from 'slate2d';
import * as Assets from 'assets';

import Entity from '../entity.js';
import Dir from '../dir.js';
import CollisionType from '../collisiontype.js';
import { Player } from './player.js';
import Phys from '../phys.js';
import World from '../world.js';
import { clamp } from '../util.js';

enum Frames {
  Walk1,
  Walk2,
  Shell,
}

enum States {
  None,
  Walk,
  Shell,
}

class Hermit extends Entity {
  drawOfs: [number, number] = [0, 0];
  sprite = Assets.find('hermit');
  state = States.Walk;
  nextState = States.None;
  nextStateTime = 0;

  constructor(args: { [key: string]: any }) {
    super(args);
    this.flipBits = 1;
    this.worldCollide = false;
  }

  die() {
    super.die();
    World().spawnDeathParticle(this, Frames.Shell);
  }

  canCollide(other: Entity, dir: Dir) {
    if (other instanceof Player && !other.stunned && dir == Dir.Up) return CollisionType.Enabled;
    else return CollisionType.Trigger;
  }

  update(ticks: number, dt: number) {
    if (ticks >= this.nextStateTime) {
      this.state = this.nextState;
      this.nextState = States.None;
    }

    switch (this.state) {
      case States.None:
        this.state = States.Walk;
        break;

      case States.Walk:
        break;

      case States.Shell:
        break;
    }

    this.moveX(this.vel[0]);
    this.moveY(this.vel[1]);
  }

  collide(other: Entity, dir: Dir) {
    if (other instanceof Player) {
      if (!other.stunned && dir == Dir.Up && other.max(1) <= this.center(1)) {
        other.stompEnemy();
        this.state = States.Shell;
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

export { Hermit };