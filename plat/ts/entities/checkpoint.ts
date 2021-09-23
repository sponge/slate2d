import * as SLT from 'slate2d';
import * as Draw from 'draw';
import * as Assets from 'assets';

import Entity from '../entity.js';
import Dir from '../dir.js';
import CollisionType from '../collisiontype.js';
import { World } from '../game.js';
import { Player } from './player.js';
import { FSMEntity, FSMStates } from '../fsmentity.js';
import { invLerp } from '../util.js';

enum Frames {
  Idle1 = 0,
  Idle2,
  Idle3,
  Activate1,
  Activate2,
  Activate3,
  Active1,
  Active2,
  Active3,
}

enum States {
  None,
  Idle,
  Bounce,
  Active,
}

class Checkpoint extends FSMEntity {
  type = 'Checkpoint';
  drawOfs: [number, number] = [-1, 0];
  collidable = CollisionType.Trigger;
  sprite = Assets.find('checkpoint');
  activated = false;

  idleAnim = [Frames.Idle1, Frames.Idle2, Frames.Idle3];
  bounceAnim = [Frames.Activate1, Frames.Activate2, Frames.Activate3];
  activeAnim = [Frames.Active1, Frames.Active2, Frames.Active3];

  #states: FSMStates = {
    default: {
      enter: () => this.fsmDefaultTransitionTo(States.Idle),
    },

    [States.Idle]: {
      update: (ticks: number) => this.frame = this.idleAnim[Math.floor(ticks / 8) % this.idleAnim.length],
      collide: (other: Entity, dir: Dir) => {
        if (other instanceof Player == false) {
          return;
        }

        this.activated = true;
        World().state.checkpointActive = true;
        World().state.checkpointPos = [this.pos[0], this.pos[1] + this.size[1]];
        World().spawnPuffParticle(this.pos[0], this.pos[1]);
        this.fsmTransitionTo(States.Bounce);
      }
    },

    [States.Bounce]: {
      enter: () => this.fsmTransitionAtTime(States.Active, 12),
      update: (ticks: number) => this.frame = this.bounceAnim[Math.floor(invLerp(this.startStateTime, this.nextStateTime, ticks) * this.activeAnim.length)],
    },

    [States.Active]: {
      update: (ticks: number) => this.frame = this.activeAnim[Math.floor(ticks / 8) % this.activeAnim.length],
    },
  };

  constructor(args: { [key: string]: any }) {
    super(args);
  }

  update(ticks: number, dt: number) {
    this.fsmUpdate(this.#states, ticks);
  }

  collide(other: Entity, dir: Dir) {
    this.fsmCollide(this.#states, other, dir);
  }
}

export { Checkpoint };