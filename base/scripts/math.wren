class Math {
   static sign(a) { a < 0 ? -1 : a > 0 ? 1 : 0 }
   static max(a, b) { a > b ? a : b }
   static min(a, b) { a < b ? a : b }
   static clamp(min, val, max) { val > max ? max : val < min ? min : val }
   static rectIntersect(x1, y1, w1, h1, x2, y2, w2, h2) { x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2 }
   static pointInRect(px, py, x, y, w, h) { (px >= x) && (px < (x + w)) && (py >= y) && (py < (y + h)) }
   static lerp(a, b, t) { (1 - t) * a + t * b }
}