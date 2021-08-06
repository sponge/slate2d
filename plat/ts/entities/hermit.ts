import * as Assets from 'assets';

import Entity from '../entity.js';
import Dir from '../dir.js';
import CollisionType from '../collisiontype.js';
import { Player } from './player.js';
import Phys from '../phys.js';
import World from '../world.js';
import FSMEntity from '../fsmentity.js';
import { dbgval } from '../printwin.js';

enum Frames {
  Walk1,
  Walk2,
  Shell,
}

enum States {
  None,
  Walk,
  Shell,
  ShellSpin,
}

class Hermit extends FSMEntity {
  type = 'Hermit';
  drawOfs: [number, number] = [-4, -1];
  sprite = Assets.find('hermit');
  state = States.Walk;
  nextState = States.None;
  nextStateTime = 0;
  spinSpeed = 3;
  walkSpeed = 0.25;

  constructor(args: { [key: string]: any }) {
    super(args);
    this.flipBits = 1;
  }

  die() {
    super.die();
    World().spawnDeathParticle(this, Frames.Shell);
  }

  #states: any = {
    default: {
      enter: () => this.fsmDefaultTransitionTo(States.Walk),
    },

    [States.Walk]: {
      update: (ticks: number) => {
        this.frame = (ticks / 8) % 2;
        if (Math.abs(this.vel[0]) != this.walkSpeed) this.vel[0] = -this.walkSpeed;
      },
      canCollide: this.standardCanEnemyCollide,
      collide: (other: Entity, dir: Dir) => {
        if (this.handlePlayerStomp(other, dir, false)) {
          this.fsmTransitionTo(States.Shell);
        }

        if (other instanceof Player == false) {
          if (this.vel[0] < 0 && dir == Dir.Left) this.vel[0] *= -1;
          else if (this.vel[0] > 0 && dir == Dir.Right) this.vel[0] *= -1;
          if (dir == Dir.Up || dir == Dir.Down) this.vel[1] = 0;
        }
      }
    },

    [States.Shell]: {
      enter: () => {
        this.fsmTransitionAtTime(States.Walk, 300);
        this.frame = Frames.Shell;
        this.vel[0] = 0;
      },
      canCollide: (other: Entity, dir: Dir) => CollisionType.Enabled,
      collide: (other: Entity, dir: Dir) => {
        // FIXME: this is still not good, see handleEnemyStomp
        if (other instanceof Player && other.max(1) <= this.min(1)) other.stompEnemy();
        this.vel[0] = this.vel[0] == 0 ? this.spinSpeed : 0;
        this.vel[0] *= this.center(0) - other.center(0) > 0 ? 1 : -1;
        this.fsmTransitionTo(States.ShellSpin);
      }
    },

    [States.ShellSpin]: {
      enter: () => {
        this.fsmTransitionAtTime(States.Shell, 0);
        this.frame = Frames.Shell;
      },
      canCollide: this.standardCanEnemyCollide,
      collide: (other: Entity, dir: Dir) => {
        if (other instanceof Player && other.max(1) <= this.min(1)) {
          other.stompEnemy();
          this.fsmTransitionTo(States.Shell);
          return;
        }

        other.hurt(1);
        // if we can't destroy it, bounce back
        if (!other.destroyed && other instanceof Player == false && (dir == Dir.Left || dir == Dir.Right)) {
          this.vel[0] *= -1;
        }
      }
    }

  };

  update(ticks: number, dt: number) {
    let grounded = this.vel[1] >= 0 && this.collideAt(this.pos[0], this.pos[1] + 1, Dir.Down);
    this.vel[1] = grounded ? 0 : this.vel[1] + Phys.enemyGravity;

    super.fsmUpdate(this.#states, ticks);

    this.moveX(this.vel[0]);
    this.moveY(this.vel[1]);

    if (this.vel[0] != 0) this.flipBits = this.vel[0] < 0 ? 1 : 0;
  }

  canCollide(other: Entity, dir: Dir) {
    return super.fsmCanCollide(this.#states, other, dir);
  }

  collide(other: Entity, dir: Dir) {
    super.fsmCollide(this.#states, other, dir);
  }
}

export { Hermit };