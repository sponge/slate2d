const clamp = (num, min, max) => Math.min(Math.max(num, min), max);
function randomRange(min, max) {
    min = Math.ceil(min);
    max = Math.floor(max);
    return Math.floor(Math.random() * (max - min) + min);
}
const rectIntersect = (x1, y1, w1, h1, x2, y2, w2, h2) => x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2;
const pointInRect = (px, py, x, y, w, h) => (px >= x) && (px < (x + w)) && (py >= y) && (py < (y + h));
const lerp = (a, b, t) => (1 - t) * a + t * b;
export { clamp, randomRange, rectIntersect, pointInRect, lerp };
