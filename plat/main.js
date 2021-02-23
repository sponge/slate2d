import { testmodule } from "testmodule.js";

let i = 0;
globalThis.draw = function() {
  console.log("hello draw");
}

globalThis.update = function() {
  i += testmodule(i);
  console.log(testmodule);
  return i;
}