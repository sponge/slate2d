import Entity from './entity.js';
import Player from './entities/player.js';
import Platform from './entities/platform.js';
import Spring from './entities/spring.js';
import Switch from './entities/switch.js';
import Coin from './entities/coin.js';
import Bird from './entities/bird.js';
import Slime from './entities/slime.js';
import Bub from './entities/bub.js';

const EntityMappings: { [key: string]: typeof Entity } = {
  'Player': Player,
  'Platform': Platform,
  'Spring': Spring,
  'Switch': Switch,
  'Coin': Coin,
  'Bird': Bird,
  'Slime': Slime,
  'Bub': Bub,
};

export default EntityMappings;