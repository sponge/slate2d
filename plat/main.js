/// <reference path="./decs.d.ts" />
import * as Draw from 'draw';
import * as SLT from 'slate2d';
import * as Assets from 'assets';

import Camera from './js/camera.js';
import LDTK from './js/ldtk.js';
import Buttons from './js/buttons.js';

const clamp = (num, min, max) => Math.min(Math.max(num, min), max);

class Entity {
  type = 'default';
  pos = [0,0];
  size = [0,0];
  vel = [0,0];
  drawOfs = [0,0];

  remainder = [0,0];
  sprite = 0;
  frame = 0;

  update(_dt) {}
  draw() {
    Draw.setColor(255, 255, 255, 255);
    Draw.sprite(this.sprite, this.frame, this.pos[0] + this.drawOfs[0], this.pos[1] + this.drawOfs[1], 1, 0, 1, 1);
  }
  
  collideAt(x, y) {
    // FIXME: need a reference to the world, but don't want to pass it in then state will have a cyclic reference
    // FIXME: GC?
    const corners = [
      [x, y],
      [x + this.size[0], y],
      [x, y + this.size[1]],
      [x + this.size[0], y + this.size[1]]
    ];

    // check against tilemap
    // iterate through corners. note this will currently break if entities are > tileSize
    const layer = globalThis.main.map.layersByName.Collision;
    for (let corner of corners) {
      const tx = Math.floor(corner[0] / layer.tileSize);
      const ty = clamp(Math.floor(corner[1] / layer.tileSize), 0, layer.height);
      if (tx < 0 || tx >= layer.width || layer.tiles[ty * layer.width + tx] !== 0) {
        // TODO: if sloped tile, check against y = mx + b
        // set some sort of collision response object on the entity to prevent alloc new objs?
        return true;
      }
    }
    
    return false;
  }

  __move(dim, amt) {
    this.remainder[dim] += amt;
    let move = Math.floor(this.remainder[dim]);

    if (move == 0) {
      return true;
    }

    this.remainder[dim] -= move;
    const sign = Math.sign(move);

    while (move != 0) {
      const check = this.pos[dim] + sign;
      const collision = dim == 0 ? this.collideAt(check, this.pos[1]) : this.collideAt(this.pos[0], check);
      if (!collision) {
        this.pos[dim] += sign;
        move -= sign;
      } else {
        return false;
      }
    }

    return true;
  }

  moveX(amt) {
    return this.__move(0, amt);
  }

  moveY(amt) {
    return this.__move(1, amt);
  }
}

class Player extends Entity {
  t = 0;

  disableControls = false;
  pMeter = 0;
  pMeterCapacity = 112;
  health = 3;
  shotsActive = 0;
  isPlayer = true;

  fallingFrames = 0;
  jumpHeld = false;
  jumpHeldFrames = false;
  invulnTime = 0;
  facing = 1;
  shotsActive = 0;
  nextShotTime = 0;

  // values from https://cdn.discordapp.com/attachments/191015116655951872/332350193540268033/smw_physics.png
  friction = 0.03125 * 2;
  accel = 0.046875 * 2;
  skidAccel = 0.15625 * 2;
  runSpeed = 1.125 * 2;
  maxSpeed = 1.5 * 2;
  pMeterCapacity = 112 * 2;
  heldGravity = 0.1875 / 2 * 2;
  gravity = 0.1875 * 2;
  earlyBounceFrames = 8;
  earlyJumpFrames = 6;
  lateJumpFrames = 6;
  terminalVelocity = 2 * 2;
  enemyJumpHeld = 3.4 * 2;
  enemyJump = 1.9 * 2;
  jumpHeights = [
    [  3, 2.875 * 2],
    [2.5, 2.78125 * 2],
    [  2, 2.71875 * 2],
    [1.5, 2.625 * 2],
    [  1, 2.5625 * 2],
    [0.5, 2.46875 * 2],
    [  0, 2.40625 * 2]
  ];

  //jumpHnd = null;
  //jumpSound = Asset.create(Asset.Sound, "player_jump", "sound/jump.wav")
  //shootSound = Asset.create(Asset.Sound, "player_shoot", "sound/shoot.wav")
  //hurtSound = Asset.create(Asset.Sound, "player_hurt", "sound/hurt.wav")
  //dieSound = Asset.create(Asset.Sound, "player_die", "sound/die.wav")

  constructor() {
    super()
    this.type = 'player';
    this.sprite = Assets.find('dogspr');
  }

  die(cause) {
    // super(cause)
    // _health = 0
    // Trap.sndStop(_jumpHnd)
    // Trap.sndPlay(_dieSound)
    // world.playerDied(this)
  }

  hurt(other, amount) {
    // if (world.ticks < _invulnTime) {
    //   return
    // }

    // if (world.levelWon) {
    //   return
    // }

    // _health = _health - amount
    // _invulnTime = world.ticks + 120

    // if (_health <= 0) {
    //   die(other)
    // } else {
    //   Trap.sndStop(_jumpHnd)
    //   Trap.sndPlay(_hurtSound)
    // }
  }

  update(dt) {
    this.t += dt;
    this.frame = Math.floor(this.t * 12) % 6

    const dir = this.disableControls ? 0 : SLT.buttonPressed(Buttons.Left) ? -1 : SLT.buttonPressed(Buttons.Right) ? 1 : 0;
    const jumpPress = this.disableControls ? false : SLT.buttonPressed(Buttons.Jump);
    const shootPress = this.disableControls ? false : SLT.buttonPressed(Buttons.Shoot);
  
    const grounded = this.collideAt(this.pos[0], this.pos[1] + 1);
    // TODO: spring code here

    // set direction for bullets and sprite drawing
    this.facing = dir != 0 ? dir : this.facing;

    // track frames since leaving platform for late jump presses
    this.fallingFrames = grounded ? 0 : this.fallingFrames + 1;

    // let players jump a few frames early but don't let them hold the button down
    this.jumpHeldFrames = jumpPress ? this.jumpHeldFrames + 1 : 0;
    if (!jumpPress && this.jumpHeld) {
      this.jumpHeld = false;
    }

    // if not pushing anything, slow down if on the ground
    if (dir == 0) {
      if (this.vel[0] != 0 && grounded) {
        this.vel[0] += this.friction * (this.vel[0] > 0 ? -1 : 1)
      }

      // null out small values so we dont keep bouncing around 0
      if (Math.abs(this.vel[0]) <= this.friction) {
        this.vel[0] = 0;
      }
    } else {
      // if holding a direction, figure out how fast we should try and go
      const speed = Math.sign(dir*this.vel[0]) == -1 ? this.skidAccel : this.accel;
      this.vel[0] = this.vel[0] + speed * dir;
    }

    // apply gravity if not on the ground. different gravity values depending on holding jump
    this.vel[1] = grounded ? 0 : this.vel[1] + (this.jumpHeld ? this.heldGravity : this.gravity);
 
    // if jump is held, and player has let go of it since last jump
    if (jumpPress && !this.jumpHeld) {
      // allow the jump if:
      // - they're on the ground, and haven't been holding for too long
      // - they're not on the ground, but have recently been on the ground
      if ((grounded && this.jumpHeldFrames < this.earlyJumpFrames) || (!grounded && this.fallingFrames < this.lateJumpFrames)) {
        for (const [speed, height] of this.jumpHeights) {
          if (Math.abs(this.vel[0]) >= speed) {
            this.vel[1] = -height;
            this.jumpHeld = true;
            // this.jumpHnd = SLT.sndPlay(this.jumpSound); // TODO: audio
            break
          }
        }
      }
    }

    // increment the p-meter if you're on the ground and going fast enough
    if (Math.abs(this.vel[0]) >= this.runSpeed && grounded) {
      this.pMeter += 2;
    // tick down the p-meter, but don't if you're at 100% and midair
    } else {
      if (grounded || this.pMeter != this.pMeterCapacity) {
        this.pMeter -= 1;
      }
    }
    this.pMeter = clamp(this.pMeter, 0, this.pMeterCapacity);

    // hard cap speed values
    if (this.pMeter == this.pMeterCapacity) {
      this.vel[0] = clamp(this.vel[0], -this.maxSpeed, this.maxSpeed);
    } else {
      this.vel[0] = clamp(this.vel[0], -this.runSpeed, this.runSpeed);
    }

    this.vel[1] = Math.min(this.vel[1], this.terminalVelocity);

    // move x first, then move y. don't do it at the same time, else buggy behavior
    // if (!groundEnt || groundEnt.has("spring") == false) { // TODO: spring check
    const chkx = this.moveX(this.vel[0]);
    // triggerTouch(chkx.delta // trigger check
    if (!chkx) {
      this.vel[0] = 0;
    }

    const chky = this.moveY(this.vel[1]);

    // TODO: all sorts of shit around jumping on top of enemies
  }

  draw() {
    Draw.setColor(255, 255, 255, 255);
    Draw.sprite(this.sprite, this.frame, this.pos[0] + this.drawOfs[0], this.pos[1] + this.drawOfs[1], 1, this.facing > 0 ? 0 : 1, 1, 1);
  }
}

class Main {
  res = {w: 384, h: 216}
  canvas = undefined;
  dog = undefined;
  dogSpr = undefined;
  state = {
    t: 0,
    entities: [],
    mapName: '',
  };
  map = undefined;
  backgrounds = [];
  clouds = [];
  camera = new Camera(this.res.w, this.res.h);
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
      width: this.res.w,
      height: this.res.h
    });

    Buttons.register();
  
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
    } else {
      const player = new Player();
      player.pos = [200,100];
      player.size = [14,14];
      player.drawOfs = [-3, -1];
      this.state.entities.push(player);
      this.state.mapName = 'maps/0000-Level_0.ldtkl';
    }

    const src = JSON.parse(SLT.readFile(this.state.mapName));
    this.map = new LDTK(src);

    this.camera.constrain(0, 0, this.map.widthPx, this.map.heightPx);
  };

  update(dt) {
    this.state.t += dt;
    SLT.showObj('main class', this);

    this.state.entities.forEach(ent => ent.update(dt));
    const player = this.state.entities[0];
    this.camera.window(player.pos[0], player.pos[1], 20, 20);
  };

  draw() {
    Draw.clear(0, 0, 0, 255);

    Draw.useCanvas(this.canvas);
    Draw.clear(41, 173, 255, 255);
  
    const { res } = this;
    const t = this.state.t;
  
    // parallax bgs
    const camY = 1 - (this.camera.y / (this.camera.con.h - res.h));
    const camYoffset = camY * 20
    this.backgrounds.forEach((bg, i) => {
      const speed = (i+1) * 0.25;
      const x = Math.floor(((0 - this.camera.x) * speed) % bg.w);
      const y = Math.floor(res.h - bg.h + camYoffset);
      Draw.image(bg.id, x, y, 0, 0, 1, 0, 0, 0);
      Draw.image(bg.id, x + bg.w, y, 0, 0, 1, 0, 0, 0);
    });

    // clouds which scroll, no parallax
    this.clouds.forEach((bg, i) => {
      const speed = (i+1) * 6;
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
    this.state.entities.forEach(ent => ent.draw());
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