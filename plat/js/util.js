// @ts-check
/// <reference path="../decs.d.ts" />

const clamp = (num, min, max) => Math.min(Math.max(num, min), max);

function randomRange(min, max) {
  min = Math.ceil(min);
  max = Math.floor(max);
  return Math.floor(Math.random() * (max - min) + min);
}

export { clamp, randomRange };