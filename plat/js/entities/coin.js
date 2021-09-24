import * as Assets from 'assets';
import Entity, { Layer } from '../entity.js';
import CollisionType from '../collisiontype.js';
import { World } from '../game.js';
import { Player } from './player.js';
class Coin extends Entity {
    type = 'Coin';
    collidable = CollisionType.Trigger;
    sprite = Assets.find('coin');
    layer = Layer.Background;
    constructor(args) {
        super(args);
    }
    update(ticks, dt) {
        this.frame = ticks / 8 % 4;
    }
    collide(other, dir) {
        if (other instanceof Player == false) {
            return;
        }
        this.destroyed = true;
        World().state.currCoins++;
    }
}
export { Coin };
