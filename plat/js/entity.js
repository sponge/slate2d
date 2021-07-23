import * as Draw from 'draw';
import { clamp, entIntersect, getOppositeDir, rectIntersect } from './util.js';
import Dir from './dir.js';
import Tiles from './tiles.js';
import CollisionType from './collisiontype.js';
import World from './world.js';
const slopes = [Tiles.SlopeL, Tiles.SlopeR];
class Entity {
    type = 'default';
    name = '';
    destroyed = false;
    pos = [0, 0];
    size = [0, 0];
    vel = [0, 0];
    drawOfs = [0, 0];
    remainder = [0, 0];
    sprite = 0;
    frame = 0;
    flipBits = 0;
    // default reaction if canCollide is not overridden
    collidable;
    // whether this entity collides with the world, or moveSolid entities
    worldCollide = true;
    collideEnt;
    collideTile = Tiles.Empty;
    anyInSlope = false;
    constructor(args) {
        Object.assign(this, args);
        const key = args.properties?.CollisionType;
        this.collidable = CollisionType[key] ?? CollisionType.Enabled;
    }
    toString() {
        return `[Entity ${this.type}]`;
    }
    preupdate(_ticks, _dt) { }
    update(_ticks, _dt) { }
    draw() {
        Draw.setColor(255, 255, 255, 255);
        Draw.sprite(this.sprite, this.frame, this.pos[0] + this.drawOfs[0], this.pos[1] + this.drawOfs[1], 1, this.flipBits, 1, 1);
    }
    drawBbox(r, g, b, a) {
        Draw.setColor(r, g, b, a);
        Draw.rect(this.pos[0], this.pos[1], this.size[0], this.size[1], false);
    }
    // callback to determine what type of collision based on the entity
    canCollide(other, dir) { return this.collidable; }
    // callback when someone else touches this entity
    collide(other, dir) { }
    // callback when an entity is activated
    activate(other) { }
    center(dim) { return this.pos[dim] + this.size[dim] / 2; }
    min(dim) { return this.pos[dim]; }
    max(dim) { return this.pos[dim] + this.size[dim]; }
    canHurt(other) { return other.type == 'Player'; }
    hurt(amt) { this.die(); }
    die() { this.destroyed = true; }
    // returns true/false if there is a collision at the specified coordinates.
    // this only queries the world, but it will update this.collideEnt
    collideAt(x, y, dir) {
        const corners = [
            [x, y],
            [x + this.size[0] - 1, y],
            [x, y + this.size[1] - 1],
            [x + this.size[0] - 1, y + this.size[1] - 1]
        ];
        const bottomMiddle = [x + this.size[0] / 2, corners[2][1]];
        const layer = World().map.layersByName.Collision;
        const entities = World().state.entities;
        const opposite = getOppositeDir(dir);
        // iterate through all entities looking for a collision
        this.collideEnt = undefined;
        for (let other of entities) {
            if (other == this)
                continue;
            const intersects = rectIntersect(corners[0], this.size, other.pos, other.size);
            if (intersects) {
                if (other.canCollide(this, opposite) == CollisionType.Disabled)
                    continue;
                if (this.canCollide(other, dir) == CollisionType.Enabled && other.canCollide(this, opposite) == CollisionType.Enabled) {
                    this.collideEnt = other;
                    return true;
                }
                else if (other.canCollide(this, opposite) == CollisionType.Platform && dir == Dir.Down && corners[2][1] == other.pos[1]) {
                    this.collideEnt = other;
                    return true;
                }
            }
        }
        // check bottom middle point if its in a slope
        const tx = Math.floor(bottomMiddle[0] / layer.tileSize);
        const ty = clamp(Math.floor(bottomMiddle[1] / layer.tileSize), 0, layer.height);
        const tid = layer.tiles[ty * layer.width + tx];
        this.anyInSlope = false;
        if (this.worldCollide) {
            // check if we're in the solid part of the slope (always 45 degrees)
            if (slopes.includes(tid)) {
                const localX = bottomMiddle[0] % layer.tileSize;
                const localY = bottomMiddle[1] % layer.tileSize;
                const minY = tid == Tiles.SlopeR ? localX : layer.tileSize - localX;
                this.collideTile = localY >= minY ? tid : Tiles.Empty;
                this.anyInSlope = true;
                return localY >= minY;
            }
            // check against tilemap
            // iterate through corners. note this will currently break if entities are > tileSize
            for (let corner of corners) {
                const tx = Math.floor(corner[0] / layer.tileSize);
                const ty = clamp(Math.floor(corner[1] / layer.tileSize), 0, layer.height);
                const tid = layer.tiles[ty * layer.width + tx];
                //if there's a tile in the intgrid...
                if (tx < 0 || tx >= layer.width || tid !== Tiles.Empty) {
                    if (tid == Tiles.Dirtback) {
                        continue;
                    }
                    // if it's a ground sloped tile, only bottom middle pixel should collide with it
                    if (slopes.includes(tid)) {
                        this.anyInSlope = true;
                        continue;
                    }
                    // if it's a platform, check if dir is down, and only block if bottom of entity
                    // intersects with the first pixel of the platform block
                    if (tid == Tiles.Platform) {
                        if (dir == Dir.Down && corner[1] == corners[2][1] && corner[1] % layer.tileSize == 0) {
                            this.collideTile = tid;
                            return true;
                        }
                        continue;
                    }
                    this.collideTile = tid;
                    return true;
                }
            }
        }
        this.collideTile = Tiles.Empty;
        return false;
    }
    __move(dim, amt) {
        this.remainder[dim] += amt;
        let move = Math.floor(this.remainder[dim]);
        if (move == 0) {
            for (let other of this.findTriggers())
                other.collide(this, Dir.None);
            return true;
        }
        this.remainder[dim] -= move;
        const sign = Math.sign(move);
        const dir = dim == 0 ? (sign > 0 ? Dir.Right : Dir.Left) : (sign > 0 ? Dir.Down : Dir.Up);
        let opposite = getOppositeDir(dir);
        let fullMove = true;
        while (move != 0) {
            const check = this.pos[dim] + sign;
            const collision = dim == 0 ? this.collideAt(check, this.pos[1], dir) : this.collideAt(this.pos[0], check, dir);
            if (!collision) {
                this.pos[dim] += sign;
                move -= sign;
            }
            else {
                // step up 1 pixel to check for slope
                if (dim == 0) {
                    if (!this.collideAt(check, this.pos[1] - 1, Dir.Up)) {
                        this.pos[0] += sign;
                        this.pos[1] -= 1;
                        move -= sign;
                        continue;
                    }
                }
                if (this.collideEnt) {
                    this.collideEnt.collide(this, opposite);
                }
                fullMove = false;
                break;
            }
        }
        for (let other of this.findTriggers())
            other.collide(this, opposite);
        // kinda lame hack, call collide with a world entity so we don't need
        // duplicate code between world response and ent response
        if (!fullMove)
            this.collide(this.collideEnt ?? worldEnt, dir);
        return fullMove;
    }
    moveX(amt) {
        return this.__move(0, amt);
    }
    moveY(amt) {
        return this.__move(1, amt);
    }
    getRidingEntities() {
        return World().state.entities.filter(other => other.worldCollide &&
            !entIntersect(this, other) &&
            rectIntersect(this.pos, this.size, [other.pos[0], other.pos[1] + 1], other.size));
    }
    moveSolid(x, y) {
        this.remainder[0] += x;
        this.remainder[1] += y;
        const entities = World().state.entities;
        // disable collision temporarily
        const currCollidable = this.collidable;
        this.collidable = CollisionType.Disabled;
        // not ideal but needs to be done before the move. need to figure out if passing in new arrays causes GC.
        // riding is true if the other entity isn't intersecting them but one pixel down vertically does
        const riding = this.getRidingEntities();
        for (let dim = 0; dim < 2; dim++) {
            const move = Math.floor(this.remainder[dim]);
            this.remainder[dim] -= move;
            this.pos[dim] += move;
            if (move == 0) {
                continue;
            }
            for (let other of entities) {
                if (other == this)
                    continue;
                // if collision is enabled and the other entity intersects with the post move position, try and push them out of the way
                const intersects = currCollidable == CollisionType.Enabled && other.worldCollide && entIntersect(this, other);
                if (intersects) {
                    // find minimum amount of movement to resolve intersection.
                    const amt = Math.sign(move) > 0 ? (this.pos[dim] + this.size[dim]) - other.pos[dim] : this.pos[dim] - (other.pos[dim] + other.size[dim]);
                    if (!other.__move(dim, amt)) {
                        other.die();
                    }
                }
                else if (riding.includes(other)) {
                    other.__move(dim, move);
                }
            }
        }
        this.collidable = currCollidable;
    }
    *findTriggers() {
        for (let other of World().state.entities) {
            if (this == other)
                continue;
            if (other.destroyed)
                continue;
            if (other.canCollide(this, Dir.None) != CollisionType.Trigger)
                continue;
            if (!entIntersect(this, other))
                continue;
            yield other;
        }
    }
}
class WorldEnt extends Entity {
    type = "World";
    collidable = CollisionType.Disabled;
    hurt(damage) { }
    die() { }
    draw() { }
}
const worldEnt = new WorldEnt({});
export default Entity;
