import "player" for Player
import "ent/flamethrower" for Flamethrower
import "ent/levelexit" for LevelExit
import "ent/spring" for Spring
import "ent/cannonball" for Cannonball
import "ent/coin" for Coin
import "ent/movingplatform" for MovingPlatform
import "ent/fallingplatform" for FallingPlatform
import "ent/cannon" for Cannon
import "ent/spike" for Spike
import "ent/walker" for Walker
import "ent/snail" for Snail
import "ent/gate" for Gate
import "ent/switch" for Switch
import "ent/delay" for Delay
import "ent/loop" for Loop
import "ent/playertrigger" for PlayerTrigger
import "ent/counter" for Counter
import "ent/stunshot" for StunShot

var EntMappings = {
   "Player": Player,
   "LevelExit": LevelExit,
   "Coin": Coin,
   "MovingPlatform": MovingPlatform,
   "FallingPlatform": FallingPlatform,
   "Spring": Spring,
   "Spike": Spike,
   "Cannon": Cannon,
   "Flamethrower": Flamethrower,
   "Walker": Walker,
   "Snail": Snail,
   "Gate": Gate,
   "Switch": Switch,
   "Delay": Delay,
   "Loop": Loop,
   "PlayerTrigger": PlayerTrigger,
   "Counter": Counter,
   "StunShot": StunShot
}