import * as Draw from 'draw';
import * as Assets from 'assets';

import Entity from './entity.js';
import Dir from './dir.js';
import CollisionType from './collisiontype.js';

class Spring extends Entity {
  sprite = Assets.find('spring');
  collidable = CollisionType.Platform;

  constructor(args: { [key: string]: any }) {
    super(args);
  }

  update(dt: number) {

  }

  draw() {
    //Draw.setColor(255, 255, 0, 255);
    //Draw.rect(this.pos[0], this.pos[1], this.size[0], this.size[1], false);
    super.draw();
  }

  collide(other: Entity, dir: Dir) {
    if (dir != Dir.Up) return;
    other.vel[1] = -9;
  }
}

export default Spring;