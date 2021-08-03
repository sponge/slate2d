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

class Ghost extends Entity {
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

  canCollide(other: Entity, dir: Dir) {
    if (other instanceof Player && other.canHurt(this) && dir == Dir.Up) return CollisionType.Enabled;
    else return CollisionType.Trigger;
  }

  update(ticks: number, dt: number) {
    if (this.nextStateTime > 0 && ticks >= this.nextStateTime) {
      this.state = this.nextState;
      this.nextState = States.None;
    }

    switch (this.state) {
      case States.None:
        this.state = States.Idle;
        this.nextStateTime = ticks + 90;
        break;

      case States.Idle:
        if (this.nextState == States.None) {
          this.nextState = States.Float;
          this.nextStateTime = ticks + 90;
        }

        this.vel[0] = 0;
        this.vel[1] = 0;

        this.frame = Frames.Idle;
        this.flipBits = this.center(0) < World().player.center(0) ? 1 : 0;
        break;

      case States.Float:
        if (this.nextState == States.None) {
          this.nextState = States.Idle;
          this.nextStateTime = ticks + 180;
        }

        const player = World().player;
        this.vel[0] += Math.sign(player.center(0) - this.center(0)) * 0.03;
        this.vel[1] += Math.sign(player.center(1) - this.center(1)) * 0.03;
        this.vel[0] = clamp(this.vel[0], -0.6, 0.6);
        this.vel[1] = clamp(this.vel[1], -0.6, 0.6);

        this.frame = Math.floor(ticks / 8 % 4) + Frames.Float1;
        if (this.vel[0] != 0) {
          this.flipBits = this.vel[0] < 0 ? 1 : 0;
        }
        break;
    }

    this.moveX(this.vel[0]);
    this.moveY(this.vel[1]);
  }

  collide(other: Entity, dir: Dir) {
    if (!this.handlePlayerStomp(other, dir)) {
      this.vel[0] *= -1;
    }
  }
}

export { Ghost };