import { testmodule } from "testmodule.js";

globalThis.start = function() {
  console.log("start");
};

globalThis.draw = function() {
  console.log("hello draw");
};

globalThis.update = function(dt) {
  console.log(dt);
  testmodule(1);
};