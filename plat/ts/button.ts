import * as SLT from 'slate2d';
import * as Draw from 'draw';
import * as Assets from 'assets';

import Entity from './entity.js';
import Dir from './dir.js';
import CollisionType from './collisiontype.js';


class Switch extends Entity {
  collidable = CollisionType.Platform;
  enabled = true;

  constructor(args: { [key: string]: any }) {
    super(args);
  }

  update(ticks: number, dt: number) {
  }

  draw() {
    if (!this.enabled) return;
    // super.draw();
    Draw.setColor(255, 0, 0, 255);
    Draw.rect(this.pos[0], this.pos[1], this.size[0], this.size[1], false);
  }

  collide(other: Entity, dir: Dir) {
    if (dir != Dir.Up) return;

    this.enabled = false;
    this.collidable = CollisionType.Disabled;
  }
}

export default Switch;