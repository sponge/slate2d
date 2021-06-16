import * as Draw from 'draw';

import Entity from './entity.js';
import Dir from './dir.js';

class Spring extends Entity {
  constructor(args: { [key: string]: any }) {
    super(args);
  }

  update(dt: number) {

  }

  draw() {
    Draw.setColor(18, 168, 5, 255);
    Draw.rect(this.pos[0], this.pos[1], this.size[0], this.size[1], false);
  }
}

export default Spring;