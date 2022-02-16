import Dir from "./dir.js";
const clamp = (num, min, max) => Math.min(Math.max(num, min), max);
function randomRange(min, max) {
    min = Math.ceil(min);
    max = Math.floor(max);
    return Math.floor(Math.random() * (max - min) + min);
}
const entIntersect = (a, b) => rectIntersect(a.pos, a.size, b.pos, b.size);
const rectIntersect = (apos, asize, bpos, bsize) => apos[0] < bpos[0] + bsize[0] && apos[0] + asize[0] > bpos[0] && apos[1] < bpos[1] + bsize[1] && apos[1] + asize[1] > bpos[1];
const pointInRect = (px, py, x, y, w, h) => (px >= x) && (px < (x + w)) && (py >= y) && (py < (y + h));
const lerp = (a, b, t) => (1 - t) * a + t * b;
const invLerp = (a, b, v) => (v - a) / (b - a);
function getOppositeDir(dir) {
    switch (dir) {
        case Dir.Down: return Dir.Up;
        case Dir.Up: return Dir.Down;
        case Dir.Left: return Dir.Right;
        case Dir.Right: return Dir.Left;
    }
    return Dir.None;
}
export { clamp, randomRange, entIntersect, rectIntersect, pointInRect, lerp, invLerp, getOppositeDir };
//# sourceMappingURL=util.js.map