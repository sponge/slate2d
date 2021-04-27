import { testmodule } from "testmodule.js";
import * as Draw from "draw";
import * as SLT from "slate2d";
import * as Assets from "assets";
import { loadTilemap } from "./tiled.js";

let dog;
let tiles;

globalThis.start = function() {
  console.log("start");
  console.log("platform is " + SLT.platform);
  SLT.registerButtons(['up', 'down', 'left', 'right']);

  dog = Assets.load({
    name: 'dog',
    type: 'image',
    path: 'gfx/dog.png'
  });

  tiles = loadTilemap('maps/8x8.json');
};

let t = 0;
globalThis.draw = function() {
  const mouse = SLT.mouse();
  const res = SLT.resolution();

  const x = Math.floor((t * 100) % res.w);
  const y = Math.floor(Math.sin(x / 50) * 100 + 200);
  const sz = Math.cos(t) * 32 + 32
  Draw.rect(x, y, sz, sz, true);

  Draw.mapLayer(tiles.tilesetHandle, 0, 0, tiles.width, tiles.height, tiles.data);

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

globalThis.update = function(dt) {
  t += dt;
  testmodule(1);
};