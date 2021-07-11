import Dir from "./dir.js";
import Entity from "./entity.js";

const clamp = (num: number, min: number, max: number) => Math.min(Math.max(num, min), max);

function randomRange(min: number, max: number) {
  min = Math.ceil(min);
  max = Math.floor(max);
  return Math.floor(Math.random() * (max - min) + min);
}

const entIntersect = (a: Entity, b: Entity) =>
  rectIntersect(a.pos, a.size, b.pos, b.size);

const rectIntersect = (apos: [number, number], asize: [number, number], bpos: [number, number], bsize: [number, number]) =>
  apos[0] < bpos[0] + bsize[0] && apos[0] + asize[0] > bpos[0] && apos[1] < bpos[1] + bsize[1] && apos[1] + asize[1] > bpos[1];

const pointInRect = (px: number, py: number, x: number, y: number, w: number, h: number) =>
  (px >= x) && (px < (x + w)) && (py >= y) && (py < (y + h));

const lerp = (a: number, b: number, t: number) => (1 - t) * a + t * b;

function getOppositeDir(dir: Dir) {
  switch (dir) {
    case Dir.Down: return Dir.Up;
    case Dir.Up: return Dir.Down;
    case Dir.Left: return Dir.Right;
    case Dir.Right: return Dir.Left;
  }

  return Dir.None;
}

export { clamp, randomRange, entIntersect, rectIntersect, pointInRect, lerp, getOppositeDir };