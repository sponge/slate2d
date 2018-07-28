import "engine" for Trap

class SoundController {
   static init() {
      __handlesByAsset = {}
      __musicHnd = null
   }

   static playOnce(asset, volume, pan, loop) {
      stopAsset(asset)
      __handlesByAsset[asset] = Trap.sndPlay(asset, volume, pan, loop)
   }

   static playOnce(asset) {
      playOnce(asset, 1.0, 0, false)
   }

   static stopAsset(asset) {
      Trap.sndStop(__handlesByAsset[asset])
      __handlesByAsset[asset] = null
   }

   static isMusicPlaying() {
      return __musicHnd != null
   }

   static playMusic(asset) {
      stopMusic()
      __musicHnd = Trap.sndPlay(asset, 0.75, 0, true)
   }

   static stopMusic() {
      Trap.sndStop(__musicHnd)
      __musicHnd = null
   }
}