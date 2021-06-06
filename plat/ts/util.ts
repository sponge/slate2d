/// <reference path="./decs.d.ts" />

const clamp = (num:number, min:number, max:number) => Math.min(Math.max(num, min), max);

function randomRange(min:number, max:number) {
  min = Math.ceil(min);
  max = Math.floor(max);
  return Math.floor(Math.random() * (max - min) + min);
}

export { clamp, randomRange };