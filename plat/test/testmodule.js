import { testmodule3, testmodule4 } from "./testmodule2.js";

function testmodule(i) {
  return i + 1;
}

function testmodule2(i) {
  return i + 2;
}

export { testmodule, testmodule2, testmodule3, testmodule4 };