import Entity from "./entity.js";
import * as Draw from 'draw';
import CollisionType from "./collisiontype.js";
class Platform extends Entity {
    collidable = CollisionType.Platform;
    draw() {
        Draw.setColor(255, 255, 0, 255);
        Draw.rect(this.pos[0], this.pos[1], this.size[0], this.size[1], false);
    }
}
export default Platform;
