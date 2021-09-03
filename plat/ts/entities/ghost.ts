import * as Assets from 'assets';

import Entity from '../entity.js';
import Dir from '../dir.js';
import CollisionType from '../collisiontype.js';
import { Player } from './player.js';
import World from '../world.js';
import { clamp } from '../util.js';
import FSMEntity from '../fsmentity.js';

enum Frames {
  Idle,
  Float1,
  Float2,
  Float3,
  Float4,
  Pain,
}

enum States {
  None,
  Idle,
  Float,
}

class Ghost extends FSMEntity {
  type = 'Ghost';
  drawOfs: [number, number] = [-2, -1];
  sprite = Assets.find('ghost');
  state = States.Idle;
  nextState = States.None;
  nextStateTime = 0;

  constructor(args: { [key: string]: any }) {
    super(args);
    this.flipBits = 1;
    this.worldCollide = false;
  }

  die() {
    super.die();
    World().spawnDeathParticle(this, Frames.Pain);
  }

  #states: any = {
    default: {
      enter: () => this.fsmDefaultTransitionTo(States.Idle),
    },

    [States.Idle]: {
      enter: () => {
        this.fsmTransitionAtTime(States.Float, 90);
        this.vel[0] = 0;
        this.vel[1] = 0;

        this.frame = Frames.Idle;
      },
      update: (ticks: number) => this.flipBits = this.center(0) < World().player.center(0) ? 1 : 0,
    },

    [States.Float]: {
      enter: () => this.fsmTransitionAtTime(States.Idle, 180),
      update: (ticks: number) => {
        const player = World().player;
        this.vel[0] += Math.sign(player.center(0) - this.center(0)) * 0.03;
        this.vel[1] += Math.sign(player.center(1) - this.center(1)) * 0.03;
        this.vel[0] = clamp(this.vel[0], -0.6, 0.6);
        this.vel[1] = clamp(this.vel[1], -0.6, 0.6);

        this.frame = Math.floor(ticks / 8 % 4) + Frames.Float1;
        if (this.vel[0] != 0) {
          this.flipBits = this.vel[0] < 0 ? 1 : 0;
        }
      }
    }
  };

  update(ticks: number, dt: number) {
    this.fsmUpdate(this.#states, ticks);
    this.moveX(this.vel[0]);
    this.moveY(this.vel[1]);
  }

  canCollide(other: Entity, dir: Dir) {
    if (other instanceof Player && dir == Dir.Up) return CollisionType.Platform;
    else return CollisionType.Trigger;
  }

  collide(other: Entity, dir: Dir) {
    this.handlePlayerStomp(other, dir);
  }
}

export { Ghost };