import * as SLT from 'slate2d';
import * as Assets from 'assets';

import Entity from '../entity.js';
import CollisionType from '../collisiontype.js';
import { World } from '../game.js';
import { Flame } from './flame.js';
import { entIntersect } from '../util.js';

class Flamethrower extends Entity {
  type = 'Flamethrower';
  sprite = Assets.find('flamethrower');
  collidable = CollisionType.Enabled;
  startOn: boolean;
  spawned = false;

  constructor(args: { [key: string]: any }) {
    super(args);
    this.startOn = args.properties?.StartOn ?? false;
  }

  preupdate(ticks: number, dt: number) {
    if (this.spawned) return;

    this.spawned = true;
    const ent = World().spawnEntity('Flame') as Flame;
    ent.startOn = this.startOn;
    ent.pos = [...this.pos];
    ent.pos[1] -= ent.size[1];
  }


  die() { }
}

export { Flamethrower };