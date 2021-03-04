import { testmodule } from "testmodule.js";
import * as Draw from "draw";

globalThis.start = function() {
  console.log("start");
};

let t = 0;
globalThis.draw = function() {
  let x = (t * 100) % 800;
  let y = Math.sin(x / 50) * 100 + 200;
  let sz = Math.cos(t) * 32 + 32
  Draw.rect(x, y, sz, sz, true);
  Draw.tri(10, 20, 400, 400, 400, 100, true);
  Draw.submit();
};

globalThis.update = function(dt) {
  t += dt;
  testmodule(1);
};