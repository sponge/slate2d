import * as Assets from 'assets';

import Entity from '../entity.js';
import Dir from '../dir.js';
import CollisionType from '../collisiontype.js';
import { Player } from './player.js';
import Phys from '../phys.js';
import World from '../world.js';

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

  canCollide(other: Entity, dir: Dir) {
    if (other instanceof Player && other.canHurt(this) && dir == Dir.Up) return CollisionType.Enabled;
    else if (other instanceof Player && other.stunned) return CollisionType.Disabled;
    else if (this.state == States.Shell) return CollisionType.Enabled;
    else return CollisionType.Trigger;
  }

  update(ticks: number, dt: number) {
    if (this.nextStateTime > 0 && ticks >= this.nextStateTime) {
      this.state = this.nextState;
      this.nextState = States.None;
    }

    let grounded = this.vel[1] >= 0 && this.collideAt(this.pos[0], this.pos[1] + 1, Dir.Down);
    this.vel[1] = grounded ? 0 : this.vel[1] + Phys.enemyGravity;

    switch (this.state) {
      case States.None:
        this.state = States.Walk;
        break;

      case States.Walk:
        this.frame = (ticks / 8) % 2;
        if (Math.abs(this.vel[0]) != this.walkSpeed) this.vel[0] = -this.walkSpeed;
        break;

      case States.Shell:
        this.frame = Frames.Shell;
        break;
    }

    this.moveX(this.vel[0]);
    this.moveY(this.vel[1]);

    if (this.vel[0] != 0) this.flipBits = this.vel[0] < 0 ? 1 : 0;
  }

  collide(other: Entity, dir: Dir) {
    // FIXME: this is bad and fraught with bugs
    if (other.canHurt(this) && dir == Dir.Up && other.max(1) <= this.min(1)) {
      if (other instanceof Player) other.stompEnemy();
      if (this.state == States.Shell) {
        this.vel[0] = this.vel[0] == 0 ? this.spinSpeed : 0;
        this.vel[0] *= this.center(0) - other.center(0) > 0 ? 1 : -1;
      }
      else {
        this.state = States.Shell;
        this.vel[0] = 0;
      }
    }
    else {
      if (this.state == States.Shell && dir != Dir.Down && this.vel[0] == 0) {
        this.vel[0] = Math.sign(this.center(0) - other.center(0)) * this.spinSpeed;
      }
      else {
        other.hurt(1);
        // if we can't destroy it, bounce back
        if (!other.destroyed && (dir == Dir.Left || dir == Dir.Right)) {
          this.vel[0] *= -1;
        }
      }
    }
  }
}

export { Hermit };