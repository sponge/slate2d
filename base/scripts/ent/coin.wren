import "entity" for Entity
import "engine" for Asset, Trap

class Coin is Entity {
   construct new(world, obj, ox, oy) {
      super(world, obj, ox, oy, 8, 8)
      world.totalCoins = world.totalCoins + 1
      _collect = Asset.create(Asset.Sound, "coin_collect", "sound/coin.wav")
   }

   canCollide(other, side, d) { other.isPlayer == true }
   trigger { true }

   touch(other, side) {
      active = false
      world.coins = world.coins + 1
      Trap.sndPlay(_collect)
   }

   draw(t) {
      drawSprite(256 + (t / 8 % 4).floor, x, y)
   }
}