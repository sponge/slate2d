import * as Assets from 'assets';
import Entity from './entity.js';
class Spring extends Entity {
    sprite = Assets.find('spring');
    constructor(args) {
        super(args);
    }
    update(dt) {
    }
    draw() {
        //Draw.setColor(255, 255, 0, 255);
        //Draw.rect(this.pos[0], this.pos[1], this.size[0], this.size[1], false);
        super.draw();
    }
}
export default Spring;
