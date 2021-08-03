import * as Assets from 'assets';

import Entity from '../entity.js';
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

class Blobby extends Entity {
  type = 'Blobby';
  sprite = Assets.find('blobby');
  drawOfs: [number, number] = [-1, -2];
  sinkAnim = [Frames.Sink1, Frames.Sink2];
  riseAnim = [Frames.Sink2, Frames.Sink1];

  state = States.Idle;
  nextState = States.None;
  nextStateTime = 0;
  startStateTime = 0;
  lastVelX = -1.5;

  die() {
    super.die();
    World().spawnDeathParticle(this, Frames.Pain);
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
        this.state = States.Idle;
        this.nextStateTime = ticks + 40;
        this.startStateTime = ticks;
        break;

      case States.Idle:
        if (this.nextState == States.None) {
          this.nextState = States.Sink;
          this.nextStateTime = ticks + 40;
          this.startStateTime = ticks;
        }

        this.vel[0] = 0;
        this.frame = ticks % 40 <= 20 ? Frames.Idle1 : Frames.Idle2;
        break;

      case States.Sink:
        if (this.nextState == States.None) {
          this.nextState = States.Move;
          this.nextStateTime = ticks + 20;
          this.startStateTime = ticks;
        }

        this.vel[0] = 0;
        this.frame = this.sinkAnim[Math.floor(invLerp(this.startStateTime, this.nextStateTime, ticks) * this.sinkAnim.length)];
        break;

      case States.Rise:
        if (this.nextState == States.None) {

          this.nextState = States.Idle;
          this.nextStateTime = ticks + 20;
          this.startStateTime = ticks;
        }

        this.vel[0] = 0;
        this.frame = this.riseAnim[Math.floor(invLerp(this.startStateTime, this.nextStateTime, ticks) * this.riseAnim.length)];
        break;

      case States.Move:
        if (this.nextState == States.None) {
          this.nextState = States.Rise;
          this.nextStateTime = ticks + 60;
          this.startStateTime = ticks;
          this.vel[0] = this.lastVelX;
        }

        this.lastVelX = this.vel[0];
        this.frame = Frames.Sunk;
        break;
    }

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