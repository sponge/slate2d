const clamp = (num, min, max) => Math.min(Math.max(num, min), max);
function randomRange(min, max) {
    min = Math.ceil(min);
    max = Math.floor(max);
    return Math.floor(Math.random() * (max - min) + min);
}
const entIntersect = (a, b) => a.pos[0] < b.pos[0] + b.size[0] && a.pos[0] + a.size[0] > b.pos[0] && a.pos[1] < b.pos[1] + b.size[1] && a.pos[1] + a.size[1] > b.pos[1];
const rectIntersect = (apos, asize, bpos, bsize) => apos[0] < bpos[0] + bsize[0] && apos[0] + asize[0] > bpos[0] && apos[1] < bpos[1] + bsize[1] && apos[1] + asize[1] > bpos[1];
const pointInRect = (px, py, x, y, w, h) => (px >= x) && (px < (x + w)) && (py >= y) && (py < (y + h));
const lerp = (a, b, t) => (1 - t) * a + t * b;
export { clamp, randomRange, entIntersect, rectIntersect, pointInRect, lerp };
