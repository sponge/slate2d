import Player from './entities/player.js';
import Platform from './entities/platform.js';
import Spring from './entities/spring.js';
import Switch from './entities/switch.js';
import Coin from './entities/coin.js';
import Bird from './entities/bird.js';
const EntityMappings = {
    'Player': Player,
    'Platform': Platform,
    'Spring': Spring,
    'Switch': Switch,
    'Coin': Coin,
    'Bird': Bird,
};
export default EntityMappings;
