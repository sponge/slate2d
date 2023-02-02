using Slate2D;
using System.Reflection.Emit;
using static System.Runtime.InteropServices.JavaScript.JSType;

enum Layer
{
    Back,
    Background,
    Normal,
    Foreground
}

class Entity
{
    protected string Type = "Default";
    public string Name = "";
    bool Destroyed = false;
    (int X, int Y) Pos = (0, 0);
    (int W, int H) Size = (0, 0);
    (int X, int Y) Vel = (0, 0);
    (int X, int Y) DrawOfs = (0, 0);
    Layer Layer = Layer.Normal;

    (float X, float Y) Remainder = (0, 0);
    AssetHandle Sprite = 0;
    int Frame = 0;
    byte FlipBits = 0;
    // default reaction if canCollide is not overridden
    public CollisionType Collidable = CollisionType.Disabled;
    // whether this entity collides with the world, or moveSolid entities
    bool WorldCollide = true;
    bool RunWhilePaused = false;

    Entity? CollideEnt;
    Tiles CollideType = Tiles.Empty;
    bool AnyInSlope = false;

    public Entity()
    {
        //Object.assign(this, args);
        //const key: keyof typeof CollisionType = args.properties?.CollisionType;
        //this.collidable = CollisionType[key] ?? CollisionType.Enabled;
    }

    public override string ToString()
    {
        return $"[Entity {Type}]";
    }

    public virtual void PreUpdate(uint ticks, float dt)
    {

    }

    public virtual void Update(uint ticks, float dt)
    {

    }

    public virtual void Draw()
    {
        DC.SetColor(255, 255, 255, 255);
        DC.Sprite(Sprite, Frame, Pos.X + DrawOfs.X, Pos.Y + DrawOfs.Y, 1, FlipBits, 1, 1);
    }

    public void DrawBbox(byte r, byte g, byte b, byte a)
    {
        DC.SetColor(r, g, b, a);
        DC.Rect(Pos.X, Pos.Y, Size.W, Size.H, false);
    }

    // callback to determine what type of collision based on the entity
    public CollisionType CanCollide(Entity other, Dir dir) { return Collidable; }
    // callback when someone else touches this entity
    public virtual void Collide(Entity other, Dir dir) { }
    // callback when an entity is activated
    public virtual void Activate(Entity other) { }

    public (int X, int Y) Center { get => (Pos.X + Size.W / 2, Pos.Y + Size.H / 2); }
    public (int X, int Y) Min { get => (Pos.X, Pos.Y); }
    public (int X, int Y) Max { get => (Pos.X + Size.W, Pos.Y + Size.H); }
    public (int X, int Y) BottomMiddle { get => (Pos.X + Size.W / 2, Pos.Y + Size.H - 1); }

    public virtual void Hurt(int number) { Die(); }
    public virtual void Die() { Destroyed = true; }

    // returns the tile id at the given coordinates
    uint TileAt(int x, int y)
    {
        throw new NotImplementedException();
        /*
        const layer = World().map.layersByName.Collision;
        const tx = Math.floor(x / layer.tileSize);
        const ty = clamp(Math.floor(y / layer.tileSize), 0, layer.height);

        return layer.tiles[ty * layer.width + tx];
        */
    }

    // returns true/false if there is a collision at the specified coordinates.
    // this only queries the world, but it will update this.collideEnt\\
    bool CollideAt(int x, int y, Dir dir)
    {
        throw new NotImplementedException();
        /*
        const corners: [number, number][] = [
            [x, y],
            [x + this.size[0] - 1, y],
            [x, y + this.size[1] - 1],
            [x + this.size[0] - 1, y + this.size[1] - 1]
        ];

        const bottomMiddle = [x + this.size[0] / 2, corners[2][1]];

        const layer = World().map.layersByName.Collision;
        const opposite = getOppositeDir(dir);

        // iterate through all entities looking for a collision
        this.collideEnt = undefined;
        for (let other of World().state.entities) {
            if (other == this) continue;

            const intersects = rectIntersect(corners[0], this.size, other.pos, other.size);

            if (intersects) {
            if (other.canCollide(this, opposite) == CollisionType.Disabled) continue;
            if (this.canCollide(other, dir) == CollisionType.Disabled) continue;

            // both sides need to be solid
            if (this.canCollide(other, dir) == CollisionType.Enabled && other.canCollide(this, opposite) == CollisionType.Enabled) {
                this.collideEnt = other;
                return true;
            }
            // need to check both sides of platform collision otherwise you might fall through
            else if (this.canCollide(other, dir) == CollisionType.Platform && dir == Dir.Up && other.max(1) >= y) {
                this.collideEnt = other;
                return true;
            }
            // original platform check
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
            const tid = ty >= layer.height ? 0 : layer.tiles[ty * layer.width + tx];
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
        */
    }

    private bool Move(int dim, int amt)
    {
        ref var pos = ref (dim == 0 ? ref Pos.X : ref Pos.Y);
        ref var remainder = ref (dim == 0 ? ref Remainder.X : ref Remainder.Y);

        remainder += amt;
        float move = MathF.Floor(remainder);

        if (move == 0)
        {
            foreach (var other in FindTriggers()) other.Collide(this, Dir.None);
            return true;
        }

        remainder -= move;
        var sign = Math.Sign(move);

        throw new NotImplementedException();

        /*
        const dir = dim == 0 ? (sign > 0 ? Dir.Right : Dir.Left) : (sign > 0 ? Dir.Down : Dir.Up);
        let opposite = getOppositeDir(dir);

        let fullMove = true;
        while (move != 0)
        {
            const check = this.pos[dim] + sign;
            const collision = dim == 0 ? this.collideAt(check, this.pos[1], dir) : this.collideAt(this.pos[0], check, dir);
            if (!collision)
            {
                this.pos[dim] += sign;
                move -= sign;
            }
            else
            {
                // step up 1 pixel to check for slope
                if (dim == 0)
                {
                    if (!this.collideAt(check, this.pos[1] - 1, Dir.Up))
                    {
                        this.pos[0] += sign;
                        this.pos[1] -= 1;
                        move -= sign;
                        continue;
                    }
                }

                if (this.collideEnt)
                {
                    this.collideEnt.collide(this, opposite);
                }
                fullMove = false;
                break;
            }
        }

        for (let other of this.findTriggers()) other.collide(this, opposite);

        // kinda lame hack, call collide with a world entity so we don't need
        // duplicate code between world response and ent response
        if (!fullMove) this.collide(this.collideEnt ?? worldEnt, dir);

        if (this.pos[1] > World().map.heightPx + 16)
        {
            this.die();
        }

        return fullMove;
        */
    }

    public bool MoveX(int amt) { return Move(0, amt); }
    public bool MoveY(int amt){ return Move(1, amt); }

    public IEnumerable<Entity> GetRidingEntities()
    {
        throw new NotImplementedException();
        // yield return
        /*
        return World().state.entities.filter(other =>
            other.worldCollide &&
            !entIntersect(this, other) &&
            rectIntersect(this.pos, this.size, [other.pos[0], other.pos[1] + 1], other.size)
        );
        */
    }

    // move a solid object that is not constrained by world collision (movers)
    public void MoveSolid(int x, int y)
    {
        throw new NotImplementedException();
        /*
        this.remainder[0] += x;
        this.remainder[1] += y;

        const entities = World().state.entities;

        // disable collision temporarily
        const currCollidable = this.collidable;
        this.collidable = CollisionType.Disabled;

        // not ideal but needs to be done before the move. need to figure out if passing in new arrays causes GC.
        // riding is true if the other entity isn't intersecting them but one pixel down vertically does
        const riding = this.getRidingEntities()

        for (let dim = 0; dim < 2; dim++)
        {
            const move = Math.floor(this.remainder[dim]);
            this.remainder[dim] -= move;

            this.pos[dim] += move;
            if (move == 0)
            {
                continue;
            }

            for (let other of entities)
            {
                if (other == this) continue;

                // if collision is enabled and the other entity intersects with the post move position, try and push them out of the way
                const intersects = currCollidable == CollisionType.Enabled && other.worldCollide && entIntersect(this, other);
                if (intersects)
                {
                    // find minimum amount of movement to resolve intersection.
                    const amt = Math.sign(move) > 0 ? (this.pos[dim] + this.size[dim]) - other.pos[dim] : this.pos[dim] - (other.pos[dim] + other.size[dim]);
                    if (!other.__move(dim, amt))
                    {
                        other.die();
                    }
                }
                else if (riding.includes(other))
                {
                    other.__move(dim, move);
                }
            }
        }

        this.collidable = currCollidable;
        */
    }

    public IEnumerable<Entity> FindTriggers()
    {
        throw new NotImplementedException();
        /*
        for (let other of World().state.entities)
        {
            if (this == other) continue;
            if (other.destroyed) continue;
            if (other.canCollide(this, Dir.None) != CollisionType.Trigger) continue;
            if (!entIntersect(this, other)) continue;
            yield other;
        }
        */
    }

    CollisionType StandardEnemyCanCollide(Entity other, Dir dir)
    {
        throw new NotImplementedException();
        //if (other instanceof Player && dir == Dir.Up) return CollisionType.Platform;
        //else if (other instanceof Player) return CollisionType.Trigger;
        //else return CollisionType.Enabled;
    }

    // FIXME: other.hurt shouldn't be in here maybe? also the die param is kinda smelly)
    bool HandleEnemyStomp(Entity other, Dir dir, bool die = true)
    {
        throw new NotImplementedException();
        /*
        if (other instanceof Player && dir == Dir.Up && other.max(1) <= this.min(1)) {
            if (die) this.die();
            other.stompEnemy();
            return true;
        }
        else if (other instanceof Player) {
            other.hurt(1);
            return false;
        }

        return false;
        */
    }

    public uint Ticks { get => 0; /*this.runWhilePaused ? World().state.wallTicks : World().state.ticks;*/ }
}


class WorldEnt : Entity {
    public WorldEnt()
    {
        this.Type = "World";
        Collidable = CollisionType.Disabled;
    }

    public override void Hurt(int dmg) { }
    public override void Die() { }
    public override void Draw() { }

    static WorldEnt Val = new WorldEnt();
}
