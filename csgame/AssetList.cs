using static Slate2D.Assets;
using static Slate2D.Assets.AssetConfig;

class AssetList {
  static readonly AssetConfig[] _Assets = new AssetConfig[] {
    new Sprite(
      Name: "dogspr",
      Path: "gfx/dog.png",
      SpriteWidth: 24,
      SpriteHeight: 18,
      MarginX: 0,
      MarginY: 0
    ),
    new Sprite(
      Name: "health",
      Path: "gfx/health.png",
      SpriteWidth: 16,
      SpriteHeight: 16,
      MarginX: 0,
      MarginY: 0
    ),
    new Sprite(
      Name: "sign",
      Path: "gfx/sign.png",
      SpriteWidth: 15,
      SpriteHeight: 14,
      MarginX: 0,
      MarginY: 0
    ),
    new Sprite(
      Name: "checkpoint",
      Path: "gfx/checkpoint.png",
      SpriteWidth: 32,
      SpriteHeight: 32,
      MarginX: 0,
      MarginY: 0
    ),
    new Sprite(
      Name: "coin",
      Path: "gfx/coin.png",
      MarginX: 0,
      MarginY: 0,
      SpriteWidth: 14,
      SpriteHeight: 14
    ),
    new BitmapFont(
      Name: "blueFont",
      Path: "gfx/panicbomber_blue.png",
      Glyphs: " !\"#$%&\"()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz(|)~",
      GlyphWidth: 8,
      CharSpacing: 0,
      SpaceWidth: 8,
      LineHeight: 8
    ),
    new Sprite(
      Name: "spring",
      Path: "gfx/spring.png",
      MarginX: 0,
      MarginY: 0,
      SpriteWidth: 16,
      SpriteHeight: 16
    ),
    new Sprite(
      Name: "bird",
      Path: "gfx/bird.png",
      MarginX: 0,
      MarginY: 0,
      SpriteWidth: 16,
      SpriteHeight: 16
    ),
    new Sprite(
      Name: "slime",
      Path: "gfx/slime.png",
      MarginX: 0,
      MarginY: 0,
      SpriteWidth: 16,
      SpriteHeight: 16
    ),
    new Sprite(
      Name: "bub",
      Path: "gfx/bub.png",
      MarginX: 0,
      MarginY: 0,
      SpriteWidth: 16,
      SpriteHeight: 16
    ),
    new Sprite(
      Name: "spikeybub",
      Path: "gfx/spikeybub.png",
      MarginX: 0,
      MarginY: 0,
      SpriteWidth: 16,
      SpriteHeight: 16
    ),
    new Sprite(
      Name: "charginbub",
      Path: "gfx/charginbub.png",
      MarginX: 0,
      MarginY: 0,
      SpriteWidth: 16,
      SpriteHeight: 16
    ),
    new Sprite(
      Name: "ghost",
      Path: "gfx/ghost.png",
      MarginX: 0,
      MarginY: 0,
      SpriteWidth: 18,
      SpriteHeight: 16
    ),
    new Sprite(
      Name: "puff",
      Path: "gfx/puff.png",
      MarginX: 0,
      MarginY: 0,
      SpriteWidth: 16,
      SpriteHeight: 16
    ),
    new Sprite(
      Name: "hermit",
      Path: "gfx/hermit.png",
      MarginX: 0,
      MarginY: 0,
      SpriteWidth: 19,
      SpriteHeight: 13
    ),
    new Sprite(
      Name: "launcher",
      Path: "gfx/launcher.png",
      MarginX: 0,
      MarginY: 0,
      SpriteWidth: 16,
      SpriteHeight: 16
    ),
    new Sprite(
      Name: "flame",
      Path: "gfx/flame.png",
      MarginX: 0,
      MarginY: 0,
      SpriteWidth: 14,
      SpriteHeight: 47
    ),
    new Sprite(
      Name: "flamethrower",
      Path: "gfx/flamethrower.png",
      MarginX: 0,
      MarginY: 0,
      SpriteWidth: 16,
      SpriteHeight: 16
    ),
    new Sprite(
      Name: "crumblingblock",
      Path: "gfx/crumblingblock.png",
      MarginX: 0,
      MarginY: 0,
      SpriteWidth: 16,
      SpriteHeight: 16
    ),
    new Sprite(
      Name: "balloon",
      Path: "gfx/balloon.png",
      MarginX: 0,
      MarginY: 0,
      SpriteWidth: 14,
      SpriteHeight: 28
    ),
    new Sprite(
      Name: "owl",
      Path: "gfx/owl.png",
      MarginX: 0,
      MarginY: 0,
      SpriteWidth: 21,
      SpriteHeight: 19
    ),
    new Sprite(
      Name: "blobby",
      Path: "gfx/blobby.png",
      MarginX: 0,
      MarginY: 0,
      SpriteWidth: 16,
      SpriteHeight: 16
    ),
    new Sprite(
      Name: "daikon",
      Path: "gfx/daikon.png",
      MarginX: 0,
      MarginY: 0,
      SpriteWidth: 16,
      SpriteHeight: 19
    ),
    new Sprite(
      Name: "totem",
      Path: "gfx/totem.png",
      MarginX: 0,
      MarginY: 0,
      SpriteWidth: 16,
      SpriteHeight: 22
    ),
    new Sprite(
      Name: "levelend",
      Path: "gfx/bone.png",
      MarginX: 0,
      MarginY: 0,
      SpriteWidth: 32,
      SpriteHeight: 32
    ),
  };

  public static void LoadAllAssets() {
    foreach (var asset in _Assets) {
      Load(asset);
    }
  }
}
