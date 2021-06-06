interface Coordinates {
  x: number;
  y: number;
}

interface Size {
  w: number;
  h: number;
}

interface ImageAsset {
  type: 'image';
  name: string;
  path: string;
  linearFilter?: boolean;
}

interface SpriteAsset {
  type: 'sprite';
  name: string;
  path: string;
  spriteWidth: number;
  spriteHeight: number;
  marginX: number;
  marginY: number;
}

interface SpeechAsset {
  type: 'speech';
  name: string;
  text: string;
}

interface SoundAsset {
  type: 'sound';
  name: string;
  path: string;
}

interface ModAsset {
  type: 'mod';
  name: string;
  path: string;
}

interface FontAsset {
  type: 'font';
  name: string;
  path: string;
}

interface BitmapFontAsset {
  type: 'bitmapfont';
  name: string;
  path: string;
  glyphs: string;
  glyphWidth: number;
  charSpacing: number;
  spaceWidth: number;
  lineHeight: number;
}

interface CanvasAsset {
  type: 'canvas';
  name: string;
  width: number;
  height: number;
}

interface ShaderAsset {
  type: 'shader';
  name: string;
  isFIle: boolean;
  vs: string;
  fs: string;
}

type Asset = ImageAsset | SpriteAsset | SpeechAsset | SoundAsset |
  ModAsset | FontAsset | BitmapFontAsset | CanvasAsset | ShaderAsset;

declare module 'draw' {
  export function setColor(r:number, g:number, b:number, a:number): void
  export function resetTransform(): void
  export function scale(x:number, y:number): void
  export function rotate(angle:number): void
  export function translate(x:number, y:number): void
  export function setScissor(x:number, y:number, w:number, h:number): void
  export function resetScissor(): void
  export function useCanvas(canvasId:number): void
  export function resetCanvas(): void
  export function useShader(shaderId:number): void
  export function resetShader(): void
  export function rect(x:number, y:number, w:number, h:number, outline:number): void
  export function setTextStyle(fontId:number, size:number, lineHeight:number, align:number): void
  export function text(x:number, y:number, h:number, text:string, len:number): void
  export function image(imgId:number, x:number, y:number, w:number, h:number, scale:number, flipBits:number, ox:number, oy:number): void
  export function sprite(spriteId:number, id:number, x:number, y:number, scale:number, flipBits:number, w:number, h:number): void
  export function line(x1:number, y1:number, x2:number, y2:number): void
  export function circle(x:number, y:number, radius:number, outline:number): void
  export function tri(x1:number, y1:number, x2:number, y2:number, x3:number, y3:number, outline:boolean): void
  export function tilemap(sprite:number, x:number, y:number, w:number, h:number, tiles:number[]): void
  export function submit(): void
  export function clear(r:number, g:number, b:number, a:number): void
}

declare module 'slate2d' {
  export function printWin(title:string, key:string, value:any): void
  export function showObj(title:string, obj:object): void
  export function error(level:number, error:string): void
  export function console(text:string): void
  export function sndPlay(asset:number, volume:number, pan:number, loop:number): number
  export function sndStop(handle:number): void
  export function sndPauseResume(handle:number, paused:number): void
  export function registerButtons(buttons:string[]): void
  export function buttonPressed(buttonNum:number, delay?:number, repeat?:number): boolean
  export function setWindowTitle(title:string): void
  export function mouse(): Coordinates
  export function resolution(): Size
  export function readFile(path:string): string
}

declare module 'assets' {
  export function find(name:string): number
  export function load(settings:Asset): number
  export function clearAll(): void
  export function loadINI(path:string): void
  export function textWidth(assetHandle:number, str:string, scale:number): number
  export function breakString(width:number, inStr:string): string
  export function imageSize(assetHandle:number): Size
}