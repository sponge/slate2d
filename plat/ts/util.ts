const clamp = (num: number, min: number, max: number) => Math.min(Math.max(num, min), max);

function randomRange(min: number, max: number) {
  min = Math.ceil(min);
  max = Math.floor(max);
  return Math.floor(Math.random() * (max - min) + min);
}

const rectIntersect = (x1: number, y1: number, w1: number, h1: number, x2: number, y2: number, w2: number, h2: number) =>
  x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2;

const pointInRect = (px: number, py: number, x: number, y: number, w: number, h: number) =>
  (px >= x) && (px < (x + w)) && (py >= y) && (py < (y + h));

const lerp = (a: number, b: number, t: number) => (1 - t) * a + t * b;

export { clamp, randomRange, rectIntersect, pointInRect, lerp };