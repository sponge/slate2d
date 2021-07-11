import Entity from "../entity.js";
import * as Draw from 'draw';
import CollisionType from "../collisiontype.js";

class Platform extends Entity {
  moving: boolean;
  dim: number;
  speed: number;
  start: number;
  end: number;
  oneShot: boolean;

  constructor(args: any) {
    super(args);
    this.moving = args.properties?.Moving ?? true;
    this.name = args.properties?.Name ?? '';
    this.dim = args.properties?.Direction == 'Horizontal' ? 0 : 1;
    this.speed = args.properties?.Speed ?? 1;
    this.oneShot = args.properties?.OneShot ?? false;

    const a = this.pos[this.dim];
    const b = a + (args.properties?.Distance ?? 100);
    this.speed *= a > b ? -1 : 1;
    this.start = Math.min(a, b);
    this.end = Math.max(a, b);
  }

  update(ticks: number, dt: number) {
    if (!this.moving) return;

    this.moveSolid(this.dim == 0 ? this.speed : 0, this.dim == 1 ? this.speed : 0);

    if (this.pos[this.dim] <= this.start || this.pos[this.dim] >= this.end) {
      this.speed *= -1;
      if (this.oneShot) {
        this.moving = false;
        return;
      }
    }
  }

  draw() {
    Draw.setColor(255, 255, 0, 255);
    Draw.rect(this.pos[0], this.pos[1], this.size[0], this.size[1], false);
  }

  trigger(other: Entity) {
    this.moving = !this.moving;
  }
}

export { Platform };