/// <reference path="./decs.d.ts" />
import * as Draw from 'draw';
import * as SLT from 'slate2d';
import * as Assets from 'assets';
import Camera from './camera.js';
import { LDTK } from './ldtk.js';
import Buttons from './buttons.js';
import Player from './entities/player.js';
import { randomRange } from './util.js';
import Platform from './entities/platform.js';
import { drawPrintWin, clearPrintWin } from './printwin.js';
import Spring from './entities/spring.js';
import Phys from './phys.js';
import Switch from './entities/switch.js';
import Coin from './entities/coin.js';
const scaleFactor = Math.floor(SLT.resolution().w / 384);
const res = SLT.resolution();
class Main {
    res = { w: res.w / scaleFactor, h: res.h / scaleFactor };
    map;
    camera = new Camera(this.res.w, this.res.h);
    accumulator = 0;
    player;
    state = {
        t: 0,
        ticks: 0,
        entities: [],
        mapName: '',
        currCoins: 0,
        maxCoins: 0
    };
    entSpawnMap = {
        'Player': Player,
        'Platform': Platform,
        'Spring': Spring,
        'Switch': Switch,
        'Coin': Coin,
    };
    canvas = Assets.load({
        name: 'canvas',
        type: 'canvas',
        width: this.res.w,
        height: this.res.h
    });
    dogSpr = Assets.load({
        name: 'dogspr',
        type: 'sprite',
        path: 'gfx/dog.png',
        spriteWidth: 22,
        spriteHeight: 16,
        marginX: 0,
        marginY: 0,
    });
    pMeterSpr = Assets.load({
        name: 'pmeter',
        type: 'sprite',
        path: 'gfx/pmeter.png',
        spriteWidth: 10,
        spriteHeight: 14,
        marginX: 0,
        marginY: 0,
    });
    coinSpr = Assets.load({
        type: 'sprite',
        name: 'coin',
        path: 'gfx/coin.png',
        marginX: 0,
        marginY: 0,
        spriteWidth: 14,
        spriteHeight: 14,
    });
    blueFont = Assets.load({
        name: 'blueFont',
        type: 'bitmapfont',
        path: 'gfx/panicbomber_blue.png',
        glyphs: ' !"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~',
        glyphWidth: 8,
        charSpacing: 0,
        spaceWidth: 8,
        lineHeight: 8,
    });
    backgrounds = [...Array(3).keys()].map(i => {
        const name = `gfx/grassland_bg${i}.png`;
        const id = Assets.load({ type: 'image', name, path: name });
        const { w, h } = Assets.imageSize(id);
        return { id, w, h, x: 0, y: this.res.h - h };
    });
    clouds = [...Array(3).keys()].map(i => {
        const name = `gfx/grassland_cloud${i}.png`;
        const id = Assets.load({ type: 'image', name, path: name });
        const { w, h } = Assets.imageSize(id);
        return { id, w, h, x: randomRange(50, 150), y: randomRange(5, 90) };
    });
    save() {
        return JSON.stringify(this.state);
    }
    constructor(initialState) {
        Buttons.register();
        Assets.load({
            type: 'sprite',
            name: 'spring',
            path: 'gfx/spring.png',
            marginX: 0,
            marginY: 0,
            spriteWidth: 16,
            spriteHeight: 16,
        });
        if (initialState) {
            this.state = JSON.parse(initialState);
            this.state.entities = this.state.entities.map(ent => Object.assign(new this.entSpawnMap[ent.type]({}), ent));
        }
        else {
            this.state.mapName = 'maps/0000-Level_0.ldtkl';
        }
        const src = JSON.parse(SLT.readFile(this.state.mapName));
        this.map = new LDTK(src);
        if (!initialState) {
            const entLayer = this.map.layersByName.Entities;
            this.state.entities = entLayer.entities.map(ent => new this.entSpawnMap[ent.type](ent));
        }
        this.player = this.state.entities.find(ent => ent instanceof Player);
        this.state.maxCoins = this.state.entities.filter(ent => ent instanceof Coin).length;
        this.camera.constrain(0, 0, this.map.widthPx, this.map.heightPx);
        this.camera.window(this.player.pos[0], this.player.pos[1], 20, 20);
    }
    ;
    update(dt) {
        this.accumulator += dt;
        while (this.accumulator > 0.0164) {
            clearPrintWin();
            this.state.t += 1 / 60;
            this.accumulator -= 0.0175;
            this.accumulator = Math.max(0, this.accumulator);
            this.state.ticks += 1;
            this.state.entities.forEach(ent => ent.destroyed || ent.preupdate(this.state.ticks, dt));
            this.state.entities.forEach(ent => ent.destroyed || ent.update(this.state.ticks, dt));
            this.camera.window(this.player.pos[0], this.player.pos[1], 20, 20);
            // kill all entities that are disabled
            for (let i = this.state.entities.length - 1; i >= 0; i--) {
                if (this.state.entities[i].destroyed) {
                    this.state.entities.splice(i, 1);
                }
            }
            //SLT.printWin('frame', 'frame', true);
        }
        drawPrintWin();
        SLT.showObj('main class', this);
    }
    ;
    draw() {
        Draw.clear(0, 0, 0, 255);
        Draw.useCanvas(this.canvas);
        Draw.clear(41, 173, 255, 255);
        const { res } = this;
        const t = this.state.t;
        // parallax bgs
        const camY = 1 - this.camera.y / (this.map.heightPx - res.h);
        const camYoffset = camY * 50;
        this.backgrounds.forEach((bg, i) => {
            const speed = (i + 1) * 0.25;
            const x = Math.floor(((bg.x - this.camera.x) * speed) % bg.w);
            const y = Math.floor(bg.y + camYoffset);
            Draw.image(bg.id, x, y, 0, 0, 1, 0, 0, 0);
            Draw.image(bg.id, x + bg.w, y, 0, 0, 1, 0, 0, 0);
        });
        // clouds which scroll, no parallax
        this.clouds.forEach((bg, i) => {
            const speed = (i + 1) * 6;
            const x = res.w + (bg.x - t * speed) % (res.w + bg.w);
            Draw.image(bg.id, x, bg.y, 0, 0, 1, 0, 0, 0);
        });
        // running dog
        const x = Math.floor((t * 50) % (res.w + 22) - 22);
        const y = Math.floor(Math.sin(x / 50) * 5 + (this.res.h * 0.8));
        Draw.setColor(255, 255, 255, 255);
        Draw.sprite(this.dogSpr, Math.floor(t * 12) % 6, x, y + camYoffset, 1, 0, 1, 1);
        this.camera.drawStart();
        // tilemap and entities
        Draw.setColor(255, 255, 255, 255);
        this.map.draw('BGDecoration');
        this.map.draw('BGTiles');
        this.map.draw('BGWorld');
        for (let i = this.state.entities.length - 1; i >= 0; i--) {
            this.state.entities[i].draw();
        }
        Draw.setColor(255, 255, 255, 255);
        this.map.draw('Collision');
        this.camera.drawEnd();
        // player hud
        // coin display 
        Draw.setTextStyle(this.blueFont, 1, 1, 1);
        Draw.sprite(this.coinSpr, 0, 100, 8, 1, 0, 1, 1);
        Draw.text(118, 11, 300, `${this.state.currCoins}/${this.state.maxCoins}`, 0);
        // p-meter
        const pct = Math.floor(this.player.pMeter / Phys.pMeterCapacity * 6);
        for (let i = 0; i < 5; i++) {
            let num = this.player.pMeter == Phys.pMeterCapacity ? 2 : i < pct ? 1 : 0;
            Draw.sprite(this.pMeterSpr, num, 14 + i * 14, 8, 1, 0, 1, 1);
        }
        // draw the canvas into the center of the window
        const screen = SLT.resolution();
        const scale = Math.floor(screen.h / res.h);
        Draw.resetCanvas();
        Draw.setColor(255, 255, 255, 255);
        Draw.image(this.canvas, (screen.w - (res.w * scale)) / 2, (screen.h - (res.h * scale)) / 2, res.w, res.h, scale, 0, 0, 0);
        Draw.submit();
    }
    ;
}
export default Main;
