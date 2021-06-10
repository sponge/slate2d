/// <reference path="./decs.d.ts" />
import * as Draw from 'draw';
import * as SLT from 'slate2d';
import * as Assets from 'assets';

import Camera from './camera.js';
import LDTK from './ldtk.js';
import Buttons from './buttons.js';
import Player from './player.js';
import { randomRange } from './util.js';
import Entity from './entity.js';
import Platform from './platform.js';

interface GameState {
  t: number;
  entities: Entity[];
  mapName: string;
}

interface Background {
  id: number;
  w: number;
  h: number;
  x: number;
  y: number;
}

class Main {
  res = { w: 384, h: 216 };

  canvas: number = Assets.load({
    name: 'canvas',
    type: 'canvas',
    width: this.res.w,
    height: this.res.h
  });

  dog: number = Assets.load({
    name: 'dog',
    type: 'image',
    path: 'gfx/dog.png'
  });

  dogSpr: number = Assets.load({
    name: 'dogspr',
    type: 'sprite',
    path: 'gfx/dog.png',
    spriteWidth: 22,
    spriteHeight: 16,
    marginX: 0,
    marginY: 0,
  });

  state: GameState = {
    t: 0,
    entities: [],
    mapName: '',
  };

  map: LDTK;

  backgrounds: Background[] = [...Array(3).keys()].map(i => {
    const name = `gfx/grassland_bg${i}.png`;
    const id = Assets.load({ type: 'image', name, path: name });
    const { w, h } = Assets.imageSize(id);
    return { id, w, h, x: 0, y: this.res.h - h };
  });

  clouds: Background[] = [...Array(3).keys()].map(i => {
    const name = `gfx/grassland_cloud${i}.png`;
    const id = Assets.load({ type: 'image', name, path: name });
    const { w, h } = Assets.imageSize(id);
    return { id, w, h, x: randomRange(50, 150), y: randomRange(5, 90) };
  });

  camera = new Camera(this.res.w, this.res.h);
  entSpawnMap: { [key: string]: typeof Entity } = {
    'Player': Player,
    'Platform': Platform,
  };
  accumulator = 0;

  save() {
    return JSON.stringify(this.state);
  }

  constructor(initialState: string) {
    Buttons.register();

    if (initialState) {
      this.state = JSON.parse(initialState);
      this.state.entities = this.state.entities.map(ent => Object.assign(new this.entSpawnMap[ent.type], ent));
    } else {
      this.state.mapName = 'maps/0000-Level_0.ldtkl';
    }

    const src = JSON.parse(SLT.readFile(this.state.mapName));
    this.map = new LDTK(src);

    if (!initialState) {
      const entLayer = this.map.layersByName.Entities;
      this.state.entities = entLayer.entities.map(ent => Object.assign(new this.entSpawnMap[ent.type], ent));
    }

    this.camera.constrain(0, 0, this.map.widthPx, this.map.heightPx);
  };

  update(dt: number) {
    this.accumulator += dt;

    while (this.accumulator > 1 / 61) {
      this.state.t += 1 / 60;
      this.accumulator -= 1 / 60;
      this.state.entities.forEach(ent => ent.update(dt));
      const player = this.state.entities[0];
      this.camera.window(player.pos[0], player.pos[1], 20, 20);
    }

    SLT.showObj('main class', this);
  };

  draw() {
    Draw.clear(0, 0, 0, 255);

    Draw.useCanvas(this.canvas);
    Draw.clear(41, 173, 255, 255);

    const { res } = this;
    const t = this.state.t;

    // parallax bgs
    const camY = 1 - (this.camera.y / (this.map.heightPx ?? 0 - res.h));
    const camYoffset = camY * 20
    this.backgrounds.forEach((bg, i) => {
      const speed = (i + 1) * 0.25;
      const x = Math.floor(((bg.x - this.camera.x) * speed) % bg.w);
      const y = Math.floor(bg.y + camYoffset);
      Draw.image(bg.id, x, y, 0, 0, 1, 0, 0, 0);
      Draw.image(bg.id, x + bg.w, y, 0, 0, 1, 0, 0, 0);
    });

    // clouds which scroll, no parallax
    this.clouds.forEach((bg: Background, i: number) => {
      const speed = (i + 1) * 6;
      const x = res.w + (bg.x - t * speed) % (res.w + bg.w);
      Draw.image(bg.id, x, bg.y, 0, 0, 1, 0, 0, 0);
    });

    // running dog
    const x = Math.floor((t * 50) % (res.w + 22) - 22);
    const y = Math.floor(Math.sin(x / 50) * 5 + 167);
    Draw.setColor(255, 255, 255, 255);
    Draw.sprite(this.dogSpr, Math.floor(t * 12) % 6, x, y + camYoffset, 1, 0, 1, 1);
    Draw.setColor(255, 255, 255, 128);

    this.camera.drawStart();

    // tilemap and entities
    Draw.setColor(255, 255, 255, 255);
    this.map.draw('BGDecoration');
    this.map.draw('BGTiles');
    this.map.draw('BGWorld');
    this.state.entities.forEach((ent: Entity) => ent.draw());
    this.map.draw('Collision');

    this.camera.drawEnd();

    // draw the canvas into the center of the window
    const screen = SLT.resolution();
    const scale = Math.floor(screen.h / res.h);
    Draw.resetCanvas();
    Draw.setColor(255, 255, 255, 255);
    Draw.image(this.canvas, (screen.w - (res.w * scale)) / 2, (screen.h - (res.h * scale)) / 2, res.w, res.h, scale, 0, 0, 0);

    Draw.submit();
  };
}

export default Main;