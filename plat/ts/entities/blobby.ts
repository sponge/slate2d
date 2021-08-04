import * as Assets from 'assets';

import Entity from '../entity.js';
import FSMEntity from '../fsmentity.js';
import Dir from '../dir.js';
import { Player } from './player.js';
import World from '../world.js';
import { dbgval } from '../printwin.js';
import { invLerp } from '../util.js';
import CollisionType from '../collisiontype.js';
import Phys from '../phys.js';

enum Frames {
  Idle1 = 0,
  Idle2,
  Sink1,
  Sink2,
  Sunk,
  Pain
}

enum States {
  None,
  Idle,
  Sink,
  Rise,
  Move,
}

class Blobby extends FSMEntity {
  type = 'Blobby';
  sprite = Assets.find('blobby');
  drawOfs: [number, number] = [-1, -2];
  sinkAnim = [Frames.Sink1, Frames.Sink2];
  riseAnim = [Frames.Sink2, Frames.Sink1];
  lastVelX = -1.5;

  die() {
    super.die();
    World().spawnDeathParticle(this, Frames.Pain);
  }

  #states: any = {
    default: {},

    [States.None]: {
      enter: () => this.state = States.Idle,
    },

    [States.Idle]: {
      enter: (ticks: number) => this.transitionAtTime(States.Sink, ticks + 40),
      update: (ticks: number) => this.frame = ticks % 40 <= 20 ? Frames.Idle1 : Frames.Idle2,
    },

    [States.Sink]: {
      enter: (ticks: number) => this.transitionAtTime(States.Move, ticks + 20),
      update: (ticks: number) => this.frame = this.sinkAnim[Math.floor(invLerp(this.startStateTime, this.nextStateTime, ticks) * this.sinkAnim.length)],
    },

    [States.Rise]: {
      enter: (ticks: number) => this.transitionAtTime(States.Idle, ticks + 20),
      update: (ticks: number) => this.frame = this.riseAnim[Math.floor(invLerp(this.startStateTime, this.nextStateTime, ticks) * this.riseAnim.length)],
    },

    [States.Move]: {
      enter: (ticks: number) => {
        this.transitionAtTime(States.Rise, ticks + 60);
        this.vel[0] = this.lastVelX;
        this.frame = Frames.Sunk;
      },
      exit: () => {
        this.lastVelX = this.vel[0];
        this.vel[0] = 0;
      },
    },
  };

  update(ticks: number, dt: number) {
    let grounded = this.vel[1] >= 0 && this.collideAt(this.pos[0], this.pos[1] + 1, Dir.Down);
    this.vel[1] = grounded ? 0 : this.vel[1] + Phys.enemyGravity;

    super.update(ticks, dt);

    if (this.lastState != 0) {
      (this.#states[this.lastState]?.exit ?? this.#states.default?.exit)?.(ticks);
      this.lastState = 0;
    }

    if (this.nextState == 0) {
      (this.#states[this.state]?.enter ?? this.#states.default?.enter)?.(ticks);
    }

    (this.#states[this.state]?.update ?? this.#states.default?.update)?.(ticks);

    this.moveX(this.vel[0]);
    this.moveY(this.vel[1]);
  }

  canCollide(other: Entity, dir: Dir) {
    switch (this.state) {
      case States.Move:
        if (other instanceof Player) return CollisionType.Trigger;
        else return CollisionType.Enabled;

      default:
        return this.standardCanEnemyCollide(other, dir);
    }
  }

  collide(other: Entity, dir: Dir) {
    switch (this.state) {
      case States.Move:
        if (other instanceof Player) {
          other.hurt(1);
        }
        else {
          if (this.vel[0] < 0 && dir == Dir.Left) this.vel[0] *= -1;
          else if (this.vel[0] > 0 && dir == Dir.Right) this.vel[0] *= -1;
          if (dir == Dir.Up || dir == Dir.Down) this.vel[1] = 0;
        }
        break;

      default:
        this.handlePlayerStomp(other, dir);
        break;
    }
  }
}

export { Blobby };