import * as SLT from 'slate2d';
import * as Draw from 'draw';
import * as Assets from 'assets';

import Entity from './entity.js';
import Dir from './dir.js';
import CollisionType from './collisiontype.js';
import Player from './player.js';
import World from './world.js';
import Phys from './phys.js';
import { dbgval } from './printwin.js';
import Buttons from './buttons.js';

class Spring extends Entity {
  sprite = Assets.find('spring');
  collidable = CollisionType.Platform;
  activated = false;
  activateTicks = 0;
  delay = 12

  constructor(args: { [key: string]: any }) {
    super(args);
  }

  preupdate(ticks: number, dt: number) {
    if (!this.activated) return;
    if (World().state.ticks < this.activateTicks) return;

    for (let other of this.getRidingEntities()) {
      if (other instanceof Player) {
        other.disableControls = false;
        other.disableMovement = false;

        other.vel[1] += SLT.buttonPressed(Buttons.Jump) ? -Phys.springJumpHeld : -Phys.springJump;
        other.jumpHeld = true;
      }
      else {
        other.vel[1] += -Phys.springJump;
      }
    }

    this.activated = false;
  }

  update(ticks: number, dt: number) {
    const oldFrames = this.frame;
    this.frame = Math.floor(this.activated ? (ticks - (this.activateTicks - this.delay)) / 3 : 0);
    this.frame = this.frame >= 3 ? 1 : this.frame;

    const amt = (this.frame - oldFrames) * 4;
    this.moveSolid(0, amt);
    this.size[1] -= amt;
  }

  draw() {
    super.draw();
    // Draw.setColor(255, 255, 0, 200);
    // Draw.rect(this.pos[0], this.pos[1], this.size[0], this.size[1], false);
  }

  collide(other: Entity, dir: Dir) {
    if (dir != Dir.Up) return;

    if (!this.activated) {
      this.activated = true;
      this.activateTicks = World().state.ticks + this.delay;
    }

    if (other instanceof Player) {
      other.disableControls = true;
      other.disableMovement = true;
    }
  }
}

export default Spring;