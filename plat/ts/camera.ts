/// <reference path="./decs.d.ts" />
import * as Draw from 'draw';
import { clamp } from './util.js';

interface CameraConstraint {
  x: number;
  y: number;
  w: number;
  h: number;
};

class Camera {
  // camera coords
  x = 0;
  y = 0;
  // size of viewport
  w = 0;
  h = 0;
  // camera constraint
  con:CameraConstraint | undefined;
  
  constructor(w:number, h:number) {
    this.w = w;
    this.h = h;
  }

  // don't let the camera render beyond this box
  constrain(x:number, y:number, w:number, h:number) {
    this.con = {x, y, w, h};
    this.move(this.x, this.y);
  }

  // move the camera to this position
  move (x:number, y:number) {
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
  window(x:number, y:number, windowWidth:number, windowHeight:number) {
    const centerX = this.x + this.w / 2;
    const centerY = this.y + this.h / 2;

    let deltaX = 0;
    if (Math.abs(x - centerX) > windowWidth) {
      deltaX = x - centerX + (x > centerX ? -1 : 1) * windowWidth;
    }

    let deltaY = 0;
    if (Math.abs(y - centerY) > windowHeight) {
      deltaY = y - centerY + (y > centerY ? -1 : 1) * windowHeight;
    }

    this.move(this.x + deltaX, this.y + deltaY);
  }

  // centers the camera at a point
  center(x:number, y:number) {
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

export default Camera;