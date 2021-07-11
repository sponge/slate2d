import * as SLT from 'slate2d';
import * as Assets from 'assets';

import Entity from '../entity.js';
import Dir from '../dir.js';
import CollisionType from '../collisiontype.js';
import { Player } from './player.js';
import Phys from '../phys.js';
import World from '../world.js';

enum Frames {
  Idle = 0,
  Blink,
  Pain,
  Crouch,
  Jump,
  Fall
}

class Slime extends Entity {
  drawOfs: [number, number] = [-1, -4];
  sprite = Assets.find('slime');
  nextJump = 120;
  jumping = false;
  landTime = -999;
  dir: number;

  constructor(args: { [key: string]: any }) {
    super(args);
    this.flipBits = 1;
    this.dir = args.properties?.GoRight ?? true ? 1 : -1;
  }

  die() {
    super.die();
    World().spawnDeathParticle(this, Frames.Pain);
  }

  canCollide(other: Entity, dir: Dir) {
    if (other instanceof Player && !other.stunned && dir == Dir.Up) return CollisionType.Enabled;
    else if (other instanceof Player) return CollisionType.Trigger;
    else return CollisionType.Enabled;
  }

  update(ticks: number, dt: number) {
    let grounded = this.vel[1] >= 0 && this.collideAt(this.pos[0], this.pos[1] + 1, Dir.Down);

    this.vel[1] = grounded ? 0 : this.vel[1] + Phys.enemyGravity;
    if (grounded) {
      if (this.jumping) {
        this.nextJump = ticks + 120;
        this.landTime = ticks;
      }

      this.jumping = false;
      this.vel[0] = 0;
    } else {
      this.jumping = true;
    }

    if (!this.jumping && ticks >= this.nextJump) {
      this.vel[0] = this.dir;
      this.vel[1] = -3;
      this.jumping = true;
    }

    if (!this.moveX(this.vel[0])) {
      this.dir *= -1;
      this.vel[0] *= -0.5;
    }

    if (!this.moveY(this.vel[1])) {
      this.vel[1] = 0;
    }

    this.flipBits = this.dir < 0 ? 1 : 0;

    if (this.vel[1] < 0) {
      this.frame = Frames.Jump;
    }
    else if (this.vel[1] > 0) {
      this.frame = Frames.Fall;
    }
    else if (grounded && (this.nextJump - 20 <= ticks || ticks <= this.landTime + 10)) {
      this.frame = Frames.Crouch;
    }
    else if (grounded) {
      this.frame = ticks % 40 > 30 ? Frames.Blink : Frames.Idle;
    }
  }

  collide(other: Entity, dir: Dir) {
    if (other instanceof Player) {
      if (!other.stunned && dir == Dir.Up && other.max(1) <= this.center(1)) {
        other.stompEnemy();
        this.die();
      }
      else {
        other.hurt(1);
      }
    }
    else {
      this.dir *= -1;
      this.vel[0] *= -0.5;
      return;
    }
  }
}

export { Slime };