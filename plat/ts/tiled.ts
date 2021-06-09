import { readFile } from 'slate2d';
import * as Assets from "assets";

const FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
const FLIPPED_VERTICALLY_FLAG = 0x40000000;
const FLIPPED_DIAGONALLY_FLAG = 0x20000000;

export function loadTilemap(path: string) {
  const src = readFile(path);
  const tilemap = JSON.parse(src);
  const tileset = tilemap.tilesets[0];

  const tilesetHandle = Assets.load({
    name: 'tileset',
    type: 'sprite',
    path: `maps/${tileset.image.replace(/\\/g, '/')}`,
    spriteWidth: tileset.tilewidth,
    spriteHeight: tileset.tileheight,
    marginX: 0,
    marginY: 0,
  });

  const data = new Array(tilemap.layers[1].data.length);
  for (let i = 0; i < tilemap.layers[1].data.length; i++) {
    let d = tilemap.layers[1].data[i];
    d &= ~(FLIPPED_HORIZONTALLY_FLAG | FLIPPED_VERTICALLY_FLAG | FLIPPED_DIAGONALLY_FLAG);
    data[i] = d - 1;
  }

  return {
    tilesetHandle,
    data,
    width: tilemap.width,
    height: tilemap.height,
  };
}