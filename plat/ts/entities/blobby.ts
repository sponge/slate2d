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
    default: {
      enter: () => this.fsmDefaultTransitionTo(States.Idle),
      canCollide: (other: Entity, dir: Dir) => this.standardCanEnemyCollide(other, dir),
      collide: (other: Entity, dir: Dir) => this.handlePlayerStomp(other, dir),
    },

    [States.Idle]: {
      enter: () => this.fsmTransitionAtTime(States.Sink, 40),
      update: (ticks: number) => this.frame = ticks % 40 <= 20 ? Frames.Idle1 : Frames.Idle2,
    },

    [States.Sink]: {
      enter: () => this.fsmTransitionAtTime(States.Move, 20),
      update: (ticks: number) => this.frame = this.sinkAnim[Math.floor(invLerp(this.startStateTime, this.nextStateTime, ticks) * this.sinkAnim.length)],
    },

    [States.Rise]: {
      enter: () => this.fsmTransitionAtTime(States.Idle, 20),
      update: (ticks: number) => this.frame = this.riseAnim[Math.floor(invLerp(this.startStateTime, this.nextStateTime, ticks) * this.riseAnim.length)],
    },

    [States.Move]: {
      enter: () => {
        this.fsmTransitionAtTime(States.Rise, 60);
        this.vel[0] = this.lastVelX;
        this.frame = Frames.Sunk;
      },
      exit: () => {
        this.lastVelX = this.vel[0];
        this.vel[0] = 0;
      },
      canCollide: (other: Entity, dir: Dir) => {
        if (other instanceof Player) return CollisionType.Trigger;
        else return CollisionType.Enabled;
      },
      collide: (other: Entity, dir: Dir) => {
        if (other instanceof Player) {
          other.hurt(1);
          return;
        }

        if (this.vel[0] < 0 && dir == Dir.Left) this.vel[0] *= -1;
        else if (this.vel[0] > 0 && dir == Dir.Right) this.vel[0] *= -1;
        if (dir == Dir.Up || dir == Dir.Down) this.vel[1] = 0;
      },
    },
  };

  update(ticks: number, dt: number) {
    let grounded = this.vel[1] >= 0 && this.collideAt(this.pos[0], this.pos[1] + 1, Dir.Down);
    this.vel[1] = grounded ? 0 : this.vel[1] + Phys.enemyGravity;

    this.fsmUpdate(this.#states, ticks);

    this.moveX(this.vel[0]);
    this.moveY(this.vel[1]);
  }

  canCollide(other: Entity, dir: Dir) {
    return this.fsmCanCollide(this.#states, other, dir);
  }

  collide(other: Entity, dir: Dir) {
    this.fsmCollide(this.#states, other, dir);
  }
}

export { Blobby };