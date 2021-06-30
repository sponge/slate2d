import * as Draw from 'draw';
import * as Assets from 'assets';
function parseProperties(obj, layer) {
    return obj.reduce((acc, val) => {
        if (val.__type == 'Point') {
            acc[val.__identifier] = [val.__value.cx, val.__value.cy];
        }
        else {
            acc[val.__identifier] = val.__value;
        }
        return acc;
    }, {});
}
class LDTK {
    widthPx = 0;
    heightPx = 0;
    background = '';
    bgColor = [0, 0, 0];
    properties = {};
    layers = [];
    layersByName = {};
    constructor(o) {
        this.widthPx = o.pxWid;
        this.heightPx = o.pxHei;
        this.bgColor = o.__bgColor.match(/\w\w/g).map((x) => parseInt(x, 16));
        // turn into a reasonable k/v object
        this.properties = parseProperties(o.fieldInstances);
        // parse each layer
        o.layerInstances.forEach((layer) => {
            const lobj = {
                name: layer.__identifier,
                width: layer.__cWid,
                height: layer.__cHei,
                tileSize: layer.__gridSize,
                offsetX: layer.__pxTotalOffsetX,
                offsetY: layer.__pxTotalOffsetY,
                drawTiles: [],
                tilesetHnd: -1,
                tiles: [],
                entities: []
            };
            if (layer.__type == 'Entities') {
                lobj.entities = layer.entityInstances.map((ent) => {
                    return {
                        type: ent.__identifier,
                        size: [ent.width, ent.height],
                        pos: [ent.px[0] - ent.width * ent.__pivot[0], ent.px[1] - ent.height * ent.__pivot[1]],
                        properties: parseProperties(ent.fieldInstances, layer),
                    };
                });
            }
            else {
                lobj.tilesetHnd = Assets.load({
                    type: 'sprite',
                    name: layer.__tilesetRelPath,
                    path: layer.__tilesetRelPath,
                    spriteWidth: lobj.tileSize,
                    spriteHeight: lobj.tileSize,
                    marginX: 0,
                    marginY: 0,
                });
                if (layer.__type == 'IntGrid') {
                    lobj.tiles = layer.intGridCsv;
                }
                // ldtk can stack tiles in the same layer
                // handle this by making a new array when there's a stack
                const sz = layer.__cWid * layer.__cHei;
                lobj.drawTiles.push(new Array(sz).fill(-1));
                const tiles = layer.__type == 'Tiles' ? layer.gridTiles : layer.autoLayerTiles;
                tiles.forEach((t) => {
                    const tileidx = (t.px[1] / lobj.tileSize) * lobj.width + t.px[0] / lobj.tileSize;
                    // look for an open space on existing layers
                    for (let i = 0; i < lobj.drawTiles.length; i++) {
                        if (lobj.drawTiles[i][tileidx] == -1) {
                            lobj.drawTiles[i][tileidx] = t.t;
                            break;
                        }
                        // we're out of space, add a new layer
                        if (i + 1 == lobj.drawTiles.length) {
                            lobj.drawTiles.push(new Array(sz).fill(-1));
                            lobj.drawTiles[i + 1][tileidx] = t.t;
                            break;
                        }
                    }
                });
                // we want 0 to be on the bottom
                layer.autoLayerTiles.reverse();
            }
            this.layers.push(lobj);
            this.layersByName[lobj.name] = lobj;
        });
        // index 0 should be the bottom-most layer for drawing purposes
        this.layers.reverse();
    }
    draw(layerName) {
        const l = this.layersByName[layerName];
        for (let tmap of l.drawTiles) {
            Draw.tilemap(l.tilesetHnd, l.offsetX, l.offsetY, l.width, l.height, tmap);
        }
    }
}
export { LDTK };
