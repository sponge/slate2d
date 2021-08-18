import * as Draw from 'draw';
import * as SLT from 'slate2d';
import * as Assets from 'assets';

import Camera from './camera.js';
import { LDTK } from './ldtk.js';
import Buttons from './buttons.js';
import { Player } from './entities/player.js';
import { randomRange } from './util.js';
import Entity from './entity.js';
import { drawPrintWin, clearPrintWin, setRetain, dbgval } from './printwin.js';
import * as EntMap from './entmap.js';
import loadAllAssets from './assetlist.js';
import { SpinParticle } from './entities/spinparticle.js';
import { PuffParticle } from './entities/puffparticle.js';
import Main from './main.js';

const EntityMappings = EntMap as { [key: string]: any };

interface GameState {
  t: number
  ticks: number
  entities: Entity[]
  mapName: string
  nextMap: number
  currCoins: number
  maxCoins: number
  levelComplete: boolean
  levelCompleteTicks: number
}

interface Background {
  id: number
  w: number
  h: number
  x: number
  y: number
  random: boolean
}

const scaleFactor = Math.floor(SLT.resolution().w / 384);
const res = SLT.resolution();

class Game {
  res = { w: Math.floor(res.w / scaleFactor), h: Math.floor(res.h / scaleFactor) };
  map: LDTK;
  camera = new Camera(this.res.w, this.res.h);
  accumulator = 0;
  player: Player;

  state: GameState = {
    t: 0,
    ticks: 0,
    entities: [],
    mapName: '',
    nextMap: 0,
    currCoins: 0,
    maxCoins: 0,
    levelComplete: false,
    levelCompleteTicks: 0,
  };

  canvas: number = Assets.load({
    name: 'canvas',
    type: 'canvas',
    width: this.res.w,
    height: this.res.h
  });

  dogSpr: number;
  heartSpr: number;
  pMeterSpr: number;
  coinSpr: number;
  blueFont: number;
  backgrounds: Background[] = [];
  clouds: Background[] = [];

  save() {
    return JSON.stringify(this.state);
  }

  constructor(initialState: string, mapName?: string) {
    Buttons.register();

    loadAllAssets();
    this.dogSpr = Assets.find('dogspr');
    this.heartSpr = Assets.find('heart');
    this.pMeterSpr = Assets.find('pmeter');
    this.coinSpr = Assets.find('coin');
    this.blueFont = Assets.find('blueFont');

    // load the saved state, otherwise set the default map
    if (initialState) {
      this.state = JSON.parse(initialState);
      this.state.entities = this.state.entities.map(ent => Object.assign(new EntityMappings[ent.type]({}), ent));
    } else {
      this.state.mapName = mapName ?? 'maps/0000-Level_0.ldtkl';
    }

    this.state.nextMap = parseInt(this.state.mapName.match(/\d\d\d\d/)?.[0] ?? '0000', 10) + 1;

    // parse the map
    const src = JSON.parse(SLT.readFile(this.state.mapName));
    this.map = new LDTK(src);

    // if a new game, load the entities from the map
    if (!initialState) {
      const entLayer = this.map.layersByName.Entities;
      this.state.entities = entLayer.entities.map(ent => new EntityMappings[ent.type](ent));
    }

    const bgProps: Record<string, { key: string, numBgs: number, numClouds: number, random: boolean }> = {
      'grassland': { key: 'grassland', numBgs: 3, numClouds: 3, random: true },
      'snowland': { key: 'snowland', numBgs: 4, numClouds: 1, random: false },
    }
    const bgProp = bgProps[this.map.background] ?? bgProps['snowland'];

    this.backgrounds = [...Array(bgProp.numBgs).keys()].map(i => {
      const name = `gfx/${bgProp.key}_bg${i}.png`;
      const id = Assets.load({ type: 'image', name, path: name });
      const { w, h } = Assets.imageSize(id);
      return { id, w, h, x: 0, y: this.res.h - h, random: bgProp.random };
    });

    this.clouds = [...Array(bgProp.numClouds).keys()].map(i => {
      const name = `gfx/${bgProp.key}_cloud${i}.png`;
      const id = Assets.load({ type: 'image', name, path: name });
      const { w, h } = Assets.imageSize(id);
      return { id, w, h, x: bgProp.random ? randomRange(-300, 0) : 0, y: bgProp.random ? randomRange(5, 120) : this.res.h - h, random: bgProp.random };
    });

    // setup player
    this.player = this.state.entities.find(ent => ent instanceof Player) as Player;
    this.state.maxCoins = this.state.currCoins + this.state.entities.filter(ent => ent.type == 'Coin').length;

    // setup camera
    this.camera.constrain(0, 0, this.map.widthPx, this.map.heightPx);
    this.camera.window(this.player.pos[0], this.player.pos[1], 20, 20);
  };

  update(dt: number) {
    this.accumulator += dt;

    // if we're running at ~58ish fps, pretend its a full frame
    while (this.accumulator > 0.0164) {
      // reset accumulated prints
      clearPrintWin();
      setRetain(true);

      // always step at the same speed and subtract a little extra in case we're at ~62ish fps
      this.state.t += 1 / 60;
      this.accumulator = Math.max(0, this.accumulator - 0.0175);
      this.state.ticks += 1;

      if (this.state.levelComplete && this.state.ticks > this.state.levelCompleteTicks) {
        Main.switchLevel(this.state.nextMap);
        return;
      }

      //run preupdate on all entities before updating
      this.state.entities.forEach(ent => ent.destroyed || ent.preupdate(this.state.ticks, dt));
      this.state.entities.forEach(ent => ent.destroyed || ent.update(this.state.ticks, dt));

      // update camera to player
      this.camera.window(this.player.pos[0], this.player.pos[1], 20, 20);

      // kill all entities that are disabled
      for (let i = this.state.entities.length - 1; i >= 0; i--) {
        if (this.state.entities[i].destroyed) {
          this.state.entities.splice(i, 1);
        }
      }

      //SLT.printWin('frame', 'frame', true);
      setRetain(false);
    }

    drawPrintWin();
    SLT.showObj('main class', this);
  };

  draw() {
    // clear screen
    Draw.clear(0, 0, 0, 255);
    Draw.useCanvas(this.canvas);

    // draw map background, needs to change based on environment
    Draw.clear(this.map.bgColor[0], this.map.bgColor[1], this.map.bgColor[2], 255);

    const { res } = this;
    const t = this.state.t;

    // clouds which scroll, no parallax
    this.clouds.forEach((bg: Background, i: number) => {
      const speed = (i + 1) * 6;
      let x = res.w + (bg.x - t * speed) % (res.w + bg.w);
      if (bg.random) {
        Draw.image(bg.id, x, bg.y, 0, 0, 1, 0, 0, 0);
      }
      else {
        while (x > 0) {
          x -= bg.w;
        }
        while (x < res.w) {
          Draw.image(bg.id, x, bg.y, 0, 0, 1, 0, 0, 0);
          x += bg.w;
        }
      }
    });

    // parallax bgs
    const camY = 1 - this.camera.y / (this.map.heightPx - res.h) || 0;
    const camYoffset = camY * 50;
    this.backgrounds.forEach((bg, i) => {
      const speed = (i + 1) * 0.25;
      const x = Math.floor(((bg.x - this.camera.x) * speed) % bg.w);
      const y = Math.floor(bg.y + camYoffset);
      let bgx = x;
      while (bgx > 0) {
        bgx -= bg.w;
      }
      while (bgx < res.w) {
        Draw.image(bg.id, bgx, y, 0, 0, 1, 0, 0, 0);
        bgx += bg.w;
      }
    });

    // running dog
    if (this.map.background == 'grassland') {
      const x = Math.floor((t * 50) % (res.w + 22) - 22);
      const y = Math.floor(Math.sin(x / 50) * 5 + (this.res.h * 0.8));
      Draw.setColor(255, 255, 255, 255);
      Draw.sprite(this.dogSpr, Math.floor(t * 12) % 6, x, y + camYoffset, 1, 0, 1, 1);
    }

    // dim the background slightly
    Draw.setColor(99, 155, 255, 60);
    Draw.rect(0, 0, res.w, res.h, false);
    Draw.setColor(255, 255, 255, 255);

    // start drawing from camera viewpoint
    this.camera.drawStart();

    // tilemap and entities
    Draw.setColor(255, 255, 255, 255);
    this.map.draw('BGTiles');
    this.map.draw('BGWorld');
    this.map.draw('BGDecoration');
    this.map.draw('Collision');
    for (let i = this.state.entities.length - 1; i >= 0; i--) {
      this.state.entities[i].draw();
    }
    Draw.setColor(255, 255, 255, 255);

    this.camera.drawEnd();

    // player hud

    // coin display 
    Draw.setTextStyle(this.blueFont, 1, 1, 1);
    Draw.sprite(this.coinSpr, 0, 165, 8, 1, 0, 1, 1);
    Draw.text(185, 11, 300, `${this.state.currCoins}/${this.state.maxCoins}`, 0);

    // hearts
    for (let i = 0; i < this.player.maxHealth; i++) {
      Draw.sprite(this.heartSpr, i + 1 <= this.player.health ? 0 : 1, 14 + i * 20, 7, 1, 0, 1, 1);
    }

    // p-meter
    const pct = this.player.getPMeterRatio();
    for (let i = 0; i < 5; i++) {
      let num = pct == 1.0 ? 2 : i < Math.floor(pct * 6) ? 1 : 0;
      Draw.sprite(this.pMeterSpr, num, 85 + i * 14, 8, 1, 0, 1, 1);
    }

    // draw the canvas into the center of the window
    const screen = SLT.resolution();
    const scale = Math.floor(screen.h / res.h);
    Draw.resetCanvas();
    Draw.setColor(255, 255, 255, 255);
    Draw.image(this.canvas, (screen.w - (res.w * scale)) / 2, (screen.h - (res.h * scale)) / 2, res.w, res.h, scale, 0, 0, 0);
  };

  // spawn an entity into the map as active
  spawnEntity(type: string) {
    const ent = new EntityMappings[type]({});
    this.state.entities.push(ent);
    return ent;
  }

  // spawns a lil puff of smoke
  spawnPuffParticle(x: number, y: number) {
    const puffEnt = new PuffParticle({});
    puffEnt.pos[0] = x;
    puffEnt.pos[1] = y;
    puffEnt.start = this.state.ticks;
    this.state.entities.push(puffEnt);
  }

  // spawn a placeholder enemy that flings out of the screen
  spawnDeathParticle(ent: Entity, frame: number) {
    this.spawnPuffParticle(ent.pos[0], ent.pos[1]);

    const deathEnt = new SpinParticle({});
    deathEnt.pos = [...ent.pos];
    deathEnt.sprite = ent.sprite;
    deathEnt.frame = frame;
    deathEnt.size = ent.size;
    deathEnt.drawOfs = ent.drawOfs;
    deathEnt.start = this.state.ticks;
    deathEnt.vel[0] *= Math.sign(ent.center(0) - this.player.center(0));
    this.state.entities.push(deathEnt);

    return deathEnt;
  }

  completeLevel() {
    this.state.levelComplete = true;
    this.state.levelCompleteTicks = this.state.ticks + 120;
  }
}

export default Game;