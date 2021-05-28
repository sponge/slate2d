import { testmodule, testmodule2, testmodule3, testmodule4 } from "./test/testmodule.js";
import * as Draw from "draw";
import * as SLT from "slate2d";
import * as Assets from "assets";
import { loadTilemap } from "./tiled.js";

class Main {
  canvas = undefined;
  dog = undefined;
  dogSpr = undefined;
  state = {t: 0};
  test = [1,2,3,4,5,1,2,3,4,5,1,2,3,4,5,1,2,3,4,5];
  test2 = "hello world";
  test3 = new Promise(() => {}, () => {});
  tiles = undefined;
  backgrounds = [];
  clouds = [];

  save() {
    return JSON.stringify(this.state);
  }

  start(initialState) {
    this.canvas = Assets.load({
      name: 'canvas',
      type: 'canvas',
      width: 384,
      height: 216
    });

    SLT.registerButtons(['up', 'down', 'left', 'right']);
  
    if (initialState) {
      this.state = JSON.parse(initialState);
    }
  
    this.dog = Assets.load({
      name: 'dog',
      type: 'image',
      path: 'gfx/dog.png'
    });
  
    this.dogSpr = Assets.load({
      name: 'dogspr',
      type: 'sprite',
      path: 'gfx/dog.png',
      spriteWidth: 22,
      spriteHeight: 16,
      marginX: 0,
      marginY: 0,
    });

    this.tiles = loadTilemap('maps/8x8.json');

    this.backgrounds = [...Array(3).keys()].map(i => {
      const name = `gfx/grassland_bg${i}.png`;
      const id = Assets.load({type: 'image', name, path: name});
      const {w, h} = Assets.imageSize(id);
      return { id, w, h };
    });

    const cloudX = [50, 100, 150];
    const cloudY = [15, 45, 30];
    this.clouds = [...Array(3).keys()].map(i => {
      const name = `gfx/grassland_cloud${i}.png`;
      const id = Assets.load({type: 'image', name, path: name});
      const {w, h} = Assets.imageSize(id);
      return { id, w, h, x: cloudX[i], y: cloudY[i] };
    });

  };

  draw() {
    Draw.clear(0, 0, 0, 255);

    Draw.useCanvas(this.canvas);
    Draw.clear(41, 173, 255, 255);
  
    const mouse = SLT.mouse();
    const res = {w: 384, h: 216};
  
    let t = this.state.t;
  
    this.backgrounds.forEach((bg, i) => {
      const speed = (i+1) * 32;
      const x = (0 - t * speed) % bg.w;
      Draw.image(bg.id, x, res.h - bg.h, 0, 0, 1, 0, 0, 0);
      Draw.image(bg.id, x + bg.w, res.h - bg.h, 0, 0, 1, 0, 0, 0);
    });

    this.clouds.forEach((bg, i) => {
      const speed = (i+1) * 6;
      const x = res.w + (bg.x - t * speed) % (res.w + bg.w);
      Draw.image(bg.id, x, bg.y, 0, 0, 1, 0, 0, 0);
    });
  
    const x = Math.floor((t * 50) % (res.w + 22) - 22);
    const y = Math.floor(Math.sin(x / 50) * 5 + 167);
    const sz = Math.cos(t) * 9 + 32
    Draw.setColor(255, 255, 255, 255);
    Draw.sprite(this.dogSpr, Math.floor(t * 12) % 6, x, y, 1, 0, 1, 1);
    Draw.setColor(255, 255, 255, 128);
  
    //Draw.tilemap(this.tiles.tilesetHandle, 0, 0, this.tiles.width, this.tiles.height, this.tiles.data);

    Draw.tri(10, 20, 400, 400, 400, 100, true);
    Draw.setColor(128, 0, 0, 255);
    Draw.line(0, 0, res.w, res.h);
    Draw.line(0, 0, 200, 200);
    Draw.setColor(255, 255, 255, 128);
    Draw.rect(mouse.x, mouse.y, 16, 16);

    SLT.printWin("test", "key", "val");
    SLT.printWin("test", "x", x);
    SLT.printWin("test", "y", y);

    // draw the canvas into the game
    const screen = SLT.resolution();
    const scale = Math.floor(screen.h / res.h);
    Draw.resetCanvas();
    Draw.setColor(255, 255, 255, 255);
    Draw.image(this.canvas, (screen.w - (res.w * scale)) / 2, (screen.h - (res.h * scale)) / 2, 384, 216, scale, 0, 0, 0);

    Draw.submit();
  };
  
  update(dt) {
    this.state.t += dt;
    testmodule(1);
    testmodule4(4);
    SLT.showObj("main class", this);
  
    // if (t > 3) {
    //   throw new Error("test exception");
    // }
  };
}

export default Main;

/*
let dog, dogSpr;
let state = {t: 0};

function save() {
  return JSON.stringify(state);
}

function start(initialState) {
  console.log("start");
  console.log("platform is " + SLT.platform);
  SLT.registerButtons(['up', 'down', 'left', 'right']);

  if (initialState) {
    state = JSON.parse(initialState);
  }

  dog = Assets.load({
    name: 'dog',
    type: 'image',
    path: 'gfx/dog.png'
  });

  dogSpr = Assets.load({
    name: 'dogspr',
    type: 'sprite',
    path: 'gfx/dog.png',
    spriteWidth: 16,
    spriteHeight: 16,
    marginX: 0,
    marginY: 0,
  })

  tiles = loadTilemap('maps/8x8.json');
};

function draw() {
  Draw.clear(0, 0, 0, 255);

  const mouse = SLT.mouse();
  const res = SLT.resolution();

  let t = state.t;

  const x = Math.floor((t * 100) % res.w);
  const y = Math.floor(Math.sin(x / 50) * 100 + 200);
  const sz = Math.cos(t) * 32 + 32
  Draw.rect(x, y, sz, sz, true);

  Draw.tileMap(tiles.tilesetHandle, 0, 0, tiles.width, tiles.height, tiles.data);

  Draw.image(dog, 0, 0, 32, 32, 1, 0, 0, 0);
  Draw.tri(10, 20, 400, 400, 400, 100, true);
  Draw.setColor(128, 0, 0, 255);
  Draw.line(0, 0, res.w, res.h);
  Draw.setColor(255, 255, 255, 128);
  Draw.rect(mouse.x, mouse.y, 16, 16);

  Draw.submit();
  SLT.printWin("test", "key", "val");
  SLT.printWin("test", "x", x);
  SLT.printWin("test", "y", y);
  SLT.printWin("test", "tiles", JSON.stringify(tiles));
};

function update(dt) {
  state.t += dt;
  testmodule(1);
  testmodule4(4);

  // if (t > 3) {
  //   throw new Error("test exception");
  // }
};

export default { draw, update, start, save };
*/