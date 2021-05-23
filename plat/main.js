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
  
  constructor() {
    console.log("constructed!");
  }

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
  };

  draw() {
    Draw.clear(0, 0, 0, 255);

    Draw.useCanvas(this.canvas);
    Draw.clear(0, 0, 0, 255);
  
    const mouse = SLT.mouse();
    const res = {w: 384, h: 216};
  
    let t = this.state.t;
  
    const x = Math.floor((t * 50) % res.w);
    const y = Math.floor(Math.sin(x / 50) * 100 + 80);
    const sz = Math.cos(t) * 9 + 32
    Draw.rect(x, y, sz, sz, true);

    Draw.setColor(255, 255, 255, 255);
    Draw.sprite(this.dogSpr, Math.floor(t * 8) % 6, x, y, 2, 0, 1, 1);
    Draw.setColor(255, 255, 255, 128);
  
    Draw.tilemap(this.tiles.tilesetHandle, 0, 0, this.tiles.width, this.tiles.height, this.tiles.data);

    Draw.image(this.dog, 0, 0, 32, 32, 1, 0, 0, 0);
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