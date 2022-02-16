import * as Assets from 'assets';

import Entity from '../entity.js';
import Dir from '../dir.js';
import { World } from '../game.js';

enum Frames {
  Idle = 0,
  Walk1,
  Walk2,
  Half1,
  Half2,
  Pain,
  Chunk,
}

class Totem extends Entity {
  type = 'Totem';
  sprite = Assets.find('totem');
  vel: [number, number] = [-0.25, 0];
  drawOfs: [number, number] = [-2, -2];
  walkAnim = [Frames.Walk1, Frames.Walk2];
  halfWalkAnim = [Frames.Half1, Frames.Half2];

  half = false;

  die() {
    super.die();
    World().spawnDeathParticle(this, Frames.Pain);
  }

  update(ticks: number, dt: number) {
    const frames = this.half ? this.halfWalkAnim : this.walkAnim;
    const animSpeed = this.half ? 4 : 8;
    this.frame = frames[Math.floor(ticks / animSpeed) % frames.length];
    this.moveX(this.vel[0]);
    this.flipBits = this.vel[0] > 0 ? 1 : 0;
  }

  canCollide = this.standardCanEnemyCollide;

  collide(other: Entity, dir: Dir) {
    if (this.handlePlayerStomp(other, dir, false)) {
      if (!this.half) {
        const wood = World().spawnDeathParticle(this, Frames.Chunk);
        wood.size = [13, 13];

        this.half = true;
        this.vel[0] *= 3;
        this.size[1] -= 6;
        this.pos[1] += 6;
        this.drawOfs[1] -= 6;
      }
      else {
        this.die();
      }
    }
    else {
      this.vel[0] *= -1;
    }
  }
}

export { Totem };