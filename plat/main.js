import * as Draw from 'draw';
import * as SLT from 'slate2d';
import * as Assets from 'assets';
import { loadTilemap } from './tiled.js';
import Camera from './js/camera.js';

class Entity {
  type = 'default';
  x = 0;
  y = 0;
  sprite = 0;

  update(dt) {}
  draw() {}
}

class Player extends Entity {
  t = 0;
  constructor() {
    super()
    this.type = 'player';
    this.sprite = Assets.find('dogspr');
  }

  update(dt) {
    this.t += dt;
    if (SLT.buttonPressed(0)) this.y -= 1;
    if (SLT.buttonPressed(1)) this.y += 1;
    if (SLT.buttonPressed(2)) this.x -= 1;
    if (SLT.buttonPressed(3)) this.x += 1;
  }

  draw() {
    Draw.setColor(255, 255, 255, 255);
    Draw.sprite(this.sprite, Math.floor(this.t * 12) % 6, this.x, this.y, 1, 0, 1, 1);
  }
}

class Main {
  canvas = undefined;
  dog = undefined;
  dogSpr = undefined;
  state = {
    t: 0,
    entities: []
  };
  tiles = undefined;
  backgrounds = [];
  clouds = [];
  camera = undefined;

  entMap = {
    'player': Player
  };

  save() {
    return JSON.stringify(this.state);
  }

  start(initialState) {
    this.canvas = Assets.load({
      name: 'canvas',
      type: 'canvas',
      width: 384,
      height: 216
    });

    SLT.registerButtons(['up', 'down', 'left', 'right']);
  
    this.dog = Assets.load({
      name: 'dog',
      type: 'image',
      path: 'gfx/dog.png'
    });
  
    this.dogSpr = Assets.load({
      name: 'dogspr',
      type: 'sprite',
      path: 'gfx/dog.png',
      spriteWidth: 22,
      spriteHeight: 16,
      marginX: 0,
      marginY: 0,
    });

    this.tiles = loadTilemap('maps/8x8.json');

    this.backgrounds = [...Array(3).keys()].map(i => {
      const name = `gfx/grassland_bg${i}.png`;
      const id = Assets.load({type: 'image', name, path: name});
      const {w, h} = Assets.imageSize(id);
      return { id, w, h };
    });

    const cloudX = [50, 100, 150];
    const cloudY = [15, 45, 30];
    this.clouds = [...Array(3).keys()].map(i => {
      const name = `gfx/grassland_cloud${i}.png`;
      const id = Assets.load({type: 'image', name, path: name});
      const {w, h} = Assets.imageSize(id);
      return { id, w, h, x: cloudX[i], y: cloudY[i] };
    });

    if (initialState) {
      this.state = JSON.parse(initialState);
      this.state.entities = this.state.entities.map(ent => Object.assign(new this.entMap[ent.type], ent));
      console.log(this.state.entities);
    } else {
      const player = new Player();
      player.x = 100;
      player.y = 100;
      this.state.entities.push(player);
    }

    this.camera = new Camera(384, 216);
    this.camera.constrain(0, 0, 384 * 2, this.camera.h);
  };

  update(dt) {
    this.state.t += dt;
    SLT.showObj('main class', this);

    this.state.entities.forEach(ent => ent.update(dt));
    const player = this.state.entities[0];
    this.camera.window(player.x, player.y, 20);
  };

  draw() {
    Draw.clear(0, 0, 0, 255);

    Draw.useCanvas(this.canvas);
    Draw.clear(41, 173, 255, 255);
  
    const mouse = SLT.mouse();
    const res = {w: 384, h: 216};
  
    let t = this.state.t;
  
    this.backgrounds.forEach((bg, i) => {
      const speed = (i+1) * 1;
      const x = (0 - this.camera.x * speed) % bg.w;
      Draw.image(bg.id, x, res.h - bg.h, 0, 0, 1, 0, 0, 0);
      Draw.image(bg.id, x + bg.w, res.h - bg.h, 0, 0, 1, 0, 0, 0);
    });

    this.clouds.forEach((bg, i) => {
      const speed = (i+1) * 6;
      const x = res.w + (bg.x - t * speed) % (res.w + bg.w);
      Draw.image(bg.id, x, bg.y, 0, 0, 1, 0, 0, 0);
    });
  
    const x = Math.floor((t * 50) % (res.w + 22) - 22);
    const y = Math.floor(Math.sin(x / 50) * 5 + 167);
    const sz = Math.cos(t) * 9 + 32
    Draw.setColor(255, 255, 255, 255);
    Draw.sprite(this.dogSpr, Math.floor(t * 12) % 6, x, y, 1, 0, 1, 1);
    Draw.setColor(255, 255, 255, 128);
  
    this.camera.drawStart();
    //Draw.tilemap(this.tiles.tilesetHandle, 0, 0, this.tiles.width, this.tiles.height, this.tiles.data);

    Draw.tri(10, 20, 400, 400, 400, 100, true);
    Draw.setColor(128, 0, 0, 255);
    Draw.line(0, 0, res.w, res.h);
    Draw.line(0, 0, 200, 200);
    Draw.setColor(255, 255, 255, 128);
    Draw.rect(mouse.x, mouse.y, 16, 16);

    this.state.entities.forEach(ent => ent.draw());

    this.camera.drawEnd();

    SLT.printWin('test', 'key', 'val');
    SLT.printWin('test', 'x', x);
    SLT.printWin('test', 'y', y);

    // draw the canvas into the game
    const screen = SLT.resolution();
    const scale = Math.floor(screen.h / res.h);
    Draw.resetCanvas();
    Draw.setColor(255, 255, 255, 255);
    Draw.image(this.canvas, (screen.w - (res.w * scale)) / 2, (screen.h - (res.h * scale)) / 2, 384, 216, scale, 0, 0, 0);

    Draw.submit();
  };
}

export default Main;