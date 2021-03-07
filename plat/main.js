import { testmodule } from "testmodule.js";
import * as Draw from "draw";
import * as SLT from "slate2d";

globalThis.start = function() {
  console.log("start");
  console.log("platform is " + SLT.platform);
  SLT.registerButtons(['up', 'down', 'left', 'right']);
};

let t = 0;
globalThis.draw = function() {
  const mouse = SLT.mouse();
  const res = SLT.resolution();

  const x = (t * 100) % res.w;
  const y = Math.sin(x / 50) * 100 + 200;
  const sz = Math.cos(t) * 32 + 32
  Draw.rect(x, y, sz, sz, true);

  Draw.tri(10, 20, 400, 400, 400, 100, true);
  Draw.setColor(128, 0, 0, 255);
  Draw.line(0, 0, res.w, res.h);
  Draw.setColor(255, 255, 255, 128);
  Draw.rect(mouse.x, mouse.y, 16, 16);
  Draw.submit();
};

globalThis.update = function(dt) {
  t += dt;
  testmodule(1);
};