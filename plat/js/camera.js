/// <reference path="../decs.d.ts" />
import * as Draw from 'draw';

const clamp = (num, min, max) => Math.min(Math.max(num, min), max);

class Camera {
  // camera coords
  x = 0;
  y = 0;
  // size of viewport
  w = 0;
  h = 0;
  // camera constraint
  con = undefined;
  
  constructor(w, h) {
    this.w = w;
    this.h = h;
  }

  // don't let the camera render beyond this box
  constrain(x, y, w, h) {
    this.con = {x, y, w, h};
    this.move(this.x, this.y);
  }

  // move the camera to this position
  move (x, y) {
    this.x = x;
    this.y = y;

    // clamp if out of bounds
    if (this.con) {
      this.x = clamp(this.x, this.con.x, this.con.x + this.con.w - this.w)
      this.y = clamp(this.y, this.con.y, this.con.y + this.con.h - this.h);
    }
  }

  // moves the camera based on a focus point and a window
  // camera won't move if you're within windowWidth px from the center
  window(x, y, windowWidth) {
    const center = this.x + this.w / 2;

    if (Math.abs(x - center) <= windowWidth) {
      return;
    }

    const delta = x - center + (x > center ? -1 : 1) * windowWidth;
    this.move(this.x + delta, y);
  }

  // centers the camera at a point
  center(x, y) {
    this.move(x - this.w / 2, y - this.h / 2);
  }

  // set transform to draw from this camera's POV
  // don't move the camera while inside this!
  drawStart() {
    Draw.translate(-this.x, -this.y);
  }

  // move transform back to stop drawing from this camera's POV
  drawEnd() {
    Draw.translate(this.x, this.y);
  }
}

export default Camera