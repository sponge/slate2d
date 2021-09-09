import * as Assets from 'assets';

import Entity from '../entity.js';
import Dir from '../dir.js';
import { Player } from './player.js';
import { World } from '../game.js';
import { FSMEntity, FSMStates } from '../fsmentity.js';
import Phys from '../phys.js';
import CollisionType from '../collisiontype.js';

enum Frames {
  Idle = 0,
  Rise,
  Fall,
  Pain
}

enum States {
  None,
  Wait,
  Pop,
  Recharge
}

class Daikon extends FSMEntity {
  type = 'Daikon';
  sprite = Assets.find('daikon');
  drawOfs: [number, number] = [-2, -3];

  state = States.Wait;
  nextState = States.None;
  nextStateTime = 0;

  die() {
    super.die();
    World().spawnDeathParticle(this, Frames.Pain);
  }

  #states: FSMStates = {
    default: {
      enter: () => this.fsmDefaultTransitionTo(States.Wait),
    },

    [States.Wait]: {
      enter: () => this.frame = Frames.Idle,
      update: (ticks: number) => {
        if (Math.abs(World().player.center(0) - this.center(0)) < 40) {
          this.fsmTransitionTo(States.Pop);
        }
      },
      canCollide: (other: Entity, dir: Dir) => CollisionType.Disabled,
    },

    [States.Pop]: {
      enter: () => {
        this.vel[1] = -3;
        this.frame = Frames.Rise;
      },
      update: (ticks: number) => {
        this.vel[1] += Phys.enemyGravity;
        if (!this.moveY(this.vel[1])) {
          this.fsmTransitionTo(States.Recharge);
        }
      },
      canCollide: (other: Entity, dir: Dir) => {
        if (other instanceof Player && dir == Dir.Up) return CollisionType.Platform;
        else if (other instanceof Player) return CollisionType.Trigger;
        else return CollisionType.Disabled;
      }
    },

    [States.Recharge]: {
      enter: () => {
        this.fsmTransitionAtTime(States.Wait, 120);
        this.frame = Frames.Idle;
      },
      canCollide: (other: Entity, dir: Dir) => CollisionType.Disabled,
    }
  }

  update(ticks: number, dt: number) {
    this.fsmUpdate(this.#states, ticks);
  }

  canCollide(other: Entity, dir: Dir) {
    return this.fsmCanCollide(this.#states, other, dir);
  }

  collide(other: Entity, dir: Dir) {
    this.handlePlayerStomp(other, dir);
  }
}

export { Daikon };