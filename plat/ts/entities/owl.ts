import * as Assets from 'assets';

import Entity from '../entity.js';
import Dir from '../dir.js';
import { Player } from './player.js';
import World from '../world.js';

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

class Owl extends Entity {
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
          this.nextState = States.Rise;
          this.nextStateTime = ticks + 120;
        }

        this.frame = ticks % 40 < 30 ? Frames.Idle1 : Frames.Idle2;
        break;

      case States.Rise:
        if (this.nextState == States.None) {
          this.nextState = States.Float;
          this.nextStateTime = ticks + 60;
        }

        this.frame = this.flapAnim[Math.floor(ticks / 4) % this.flapAnim.length];
        this.moveY(-1);
        break;

      case States.Float:
        if (this.nextState == States.None) {
          this.nextState = States.Fall;
          this.nextStateTime = ticks + 90;
        }

        this.frame = this.flapAnim[Math.floor(ticks / 8) % this.flapAnim.length];
        break;

      case States.Fall:
        if (this.nextState == States.None) {
          this.nextState = States.Idle;
          this.nextStateTime = 0;
        }

        if (!this.moveY(0.5)) {
          this.state = this.nextState;
          this.nextState = States.None;
        }

        this.frame = this.fallAnim[Math.floor(ticks / 16) % this.fallAnim.length];
        break;
    }
  }

  canCollide = this.standardCanEnemyCollide;

  collide(other: Entity, dir: Dir) {
    this.handlePlayerStomp(other, dir);
  }
}

export { Owl };