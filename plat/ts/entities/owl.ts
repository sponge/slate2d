import * as Assets from 'assets';

import Entity from '../entity.js';
import Dir from '../dir.js';
import { Player } from './player.js';
import { World } from '../game.js';
import FSMEntity from '../fsmentity.js';

enum Frames {
  Idle1 = 0,
  Idle2,
  FlapUp,
  FlapMid,
  FlapDown,
  Pain
}

enum States {
  None,
  Idle,
  Rise,
  Float,
  Fall
}

class Owl extends FSMEntity {
  type = 'Owl';
  sprite = Assets.find('owl');
  drawOfs: [number, number] = [-3, -1];
  flapAnim = [Frames.FlapMid, Frames.FlapUp, Frames.FlapMid, Frames.FlapDown];
  fallAnim = [Frames.FlapMid, Frames.FlapDown];

  state = States.Idle;
  nextState = States.None;
  nextStateTime = 0;

  die() {
    super.die();
    World().spawnDeathParticle(this, Frames.Pain);
  }

  #states: any = {
    default: {
      enter: () => this.fsmDefaultTransitionTo(States.Idle),
    },

    [States.Idle]: {
      enter: (ticks: number) => this.fsmTransitionAtTime(States.Rise, 120),
      update: (ticks: number) => this.frame = ticks % 40 < 30 ? Frames.Idle1 : Frames.Idle2,
    },

    [States.Rise]: {
      enter: () => this.fsmTransitionAtTime(States.Float, 60),
      update: (ticks: number) => {
        this.frame = this.flapAnim[Math.floor(ticks / 4) % this.flapAnim.length];
        this.moveY(-1);
      }
    },

    [States.Float]: {
      enter: () => this.fsmTransitionAtTime(States.Fall, 90),
      update: (ticks: number) => this.frame = this.flapAnim[Math.floor(ticks / 8) % this.flapAnim.length],
    },

    [States.Fall]: {
      update: (ticks: number) => {
        if (!this.moveY(0.5)) {
          this.fsmTransitionTo(States.Idle);
        }

        this.frame = this.fallAnim[Math.floor(ticks / 16) % this.fallAnim.length];
      }
    }
  }

  update(ticks: number, dt: number) {
    this.fsmUpdate(this.#states, ticks);
  }

  canCollide = this.standardCanEnemyCollide;

  collide(other: Entity, dir: Dir) {
    this.handlePlayerStomp(other, dir);
  }
}

export { Owl };