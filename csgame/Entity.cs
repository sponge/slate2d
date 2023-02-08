using Slate2D;

enum Layer
{
    Back,
    Background,
    Normal,
    Foreground
}

public class Entity
{
    protected string Type = "Default";
    public string Name = "";
    public bool Destroyed = false;
    public (int X, int Y) Pos = (0, 0);
    public (int W, int H) Size = (0, 0);
    public (int X, int Y) Vel = (0, 0);
    public (int X, int Y) DrawOfs = (0, 0);
    Layer Layer = Layer.Normal;

    (float X, float Y) Remainder = (0, 0);
    AssetHandle Sprite = 0;
    int Frame = 0;
    byte FlipBits = 0;
    // default reaction if canCollide is not overridden
    public CollisionType Collidable = CollisionType.Disabled;
    // whether this entity collides with the world, or moveSolid entities
    bool WorldCollide = true;
    public bool RunWhilePaused = false;

    Entity? CollideEnt;
    Tile CollideTile = Tile.Empty;
    bool AnyInSlope = false;

    public Entity()
    {
        // FIXME: construct entity with properties
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
        var layer = Main.World.Map.LayersByName["Collision"];
        var tx = Pos.X / layer.TileSize;
        var ty = Math.Clamp(Pos.Y / layer.TileSize, 0, layer.Size.h);

        return layer.Tiles[ty * layer.Size.w + tx];
    }

    // returns true/false if there is a collision at the specified coordinates.
    // this only queries the world, but it will update this.collideEnt
    bool CollideAt(int x, int y, Dir dir)
    {
        var corners = new (int X, int Y)[]
        {
            (x, y),
            (x + Size.W - 1, y),
            (x, y + Size.H - 1),
            (x + Size.W - 1, y + Size.H - 1),
        };

        var layer = Main.World.Map.LayersByName["Collision"];
        var opposite = Util.GetOppositeDir(dir);

        // iterate through all entities looking for a collision
        CollideEnt = null;
        foreach (var other in Main.World.GameState.Entities)
        {
            if (other == this) continue;

            var intersects = Util.RectIntersect(corners[0], Size, other.Pos, other.Size);

            if (intersects)
            {
                if (other.CanCollide(this, opposite) == CollisionType.Disabled) continue;
                if (CanCollide(other, dir) == CollisionType.Disabled) continue;

                // both sides need to be solid
                if (CanCollide(other, dir) == CollisionType.Enabled && other.CanCollide(this, opposite) == CollisionType.Enabled)
                {
                    CollideEnt = other;
                    return true;
                }
                // need to check both sides of platform collision otherwise you might fall through
                else if (CanCollide(other, dir) == CollisionType.Platform && dir == Dir.Up && other.Max.Y >= y)
                {
                    CollideEnt = other;
                    return true;
                }
                // original platform check
                else if (other.CanCollide(this, opposite) == CollisionType.Platform && dir == Dir.Down && corners[2].Y == other.Pos.Y)
                {
                    CollideEnt = other;
                    return true;
                }
            }
        }

        // check bottom middle point if its in a slope
        var tx = BottomMiddle.X / layer.TileSize;
        var ty = Math.Clamp(BottomMiddle.Y / layer.TileSize, 0, layer.Size.h);
        Tile tid = (Tile)layer.Tiles[ty * layer.Size.w + tx];

        AnyInSlope = false;

        if (WorldCollide)
        {
            // check if we're in the solid part of the slope (always 45 degrees)
            if (tid == Tile.SlopeL || tid == Tile.SlopeR)
            {
                var localX = BottomMiddle.X % layer.TileSize;
                var localY = BottomMiddle.Y % layer.TileSize;
                var minY = tid == Tile.SlopeR ? localX : layer.TileSize - localX;
                CollideTile = localY >= minY ? tid : Tile.Empty;
                AnyInSlope = true;
                return localY > minY;
            }

            // check against tilemap
            // iterate through corners. note this will currently break if entities are > tileSize
            foreach (var corner in corners)
            {
                tx = corner.X / layer.TileSize;
                ty = Math.Clamp(corner.Y / layer.TileSize, 0, layer.Size.h);
                tid = ty >= layer.Size.h ? Tile.Empty : (Tile)layer.Tiles[ty * layer.Size.w + tx];

                //if there's a tile in the intgrid...
                if (tx < 0 || tx > layer.Size.w || tid != Tile.Empty)
                {
                    if (tid == Tile.Dirtback)
                    {
                        continue;
                    }

                    // if it's a ground sloped tile, only bottom middle pixel should collide with it
                    if (tid == Tile.SlopeL || tid == Tile.SlopeR)
                    {
                        AnyInSlope = true;
                        continue;
                    }

                    // if it's a platform, check if dir is down, and only block if bottom of entity
                    // intersects with the first pixel of the platform block
                    if (tid == Tile.Platform)
                    {
                        if (dir == Dir.Down && corner.Y == corners[2].Y && corner.Y % layer.TileSize == 0)
                        {
                            CollideTile = tid;
                            return true;
                        }
                        continue;
                    }

                    CollideTile = tid;
                    return true;
                }
            }
        }

        CollideTile = Tile.Empty;
        return false;
    }

    private bool Move(int dim, float amt)
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

        var dir = dim == 0 ? (sign > 0 ? Dir.Right : Dir.Left) : (sign > 0 ? Dir.Down : Dir.Up);
        var opposite = Util.GetOppositeDir(dir);

        var fullMove = true;
        while (move != 0)
        {
            var check = pos + sign;
            var collision = dim == 0 ? CollideAt(check, Pos.Y, dir) : CollideAt(Pos.X, check, dir);
            if (!collision)
            {
                pos += sign;
                move -= sign;
            }
            else
            {
                // step up 1 pixel to check for slope
                if (dim == 0)
                {
                    if (!CollideAt(check, Pos.Y - 1, Dir.Up))
                    {
                        Pos.X += sign;
                        Pos.Y -= 1;
                        move -= sign;
                        continue;
                    }
                }

                if (CollideEnt != null)
                {
                    CollideEnt.Collide(this, opposite);
                }
                fullMove = false;
                break;
            }
        }

        foreach (var other in FindTriggers()) other.Collide(this, opposite);

        // kinda lame hack, call collide with a world entity so we don't need
        // duplicate code between world response and ent response
        if (!fullMove) Collide(CollideEnt ?? WorldEnt.Value, dir);

        if (Pos.Y > Main.World.Map.PxSize.h + 16)
        {
            Die();
        }

        return fullMove;
    }

    public bool MoveX(float amt) { return Move(0, amt); }
    public bool MoveY(float amt) { return Move(1, amt); }

    public IEnumerable<Entity> GetRidingEntities()
    {
        foreach (var other in Main.World.GameState.Entities)
        {
            if (other.WorldCollide &&
                !Util.EntIntersect(this, other) &&
                Util.RectIntersect(Pos, Size, (other.Pos.X, other.Pos.Y + 1), other.Size))
            {
                yield return other;
            }
        }

    }

    // move a solid object that is not constrained by world collision (movers)
    public void MoveSolid(int x, int y)
    {
        Remainder.X += x;
        Remainder.Y += y;

        // disable collision temporarily
        var currCollidable = Collidable;
        Collidable = CollisionType.Disabled;

        // not ideal but needs to be done before the move
        // riding is true if the other entity isn't intersecting them but one pixel down vertically does
        var riding = GetRidingEntities().ToList(); // FIXME: gc?

        for (int dim = 0; dim < 2; dim++)
        {
            ref var pos = ref (dim == 0 ? ref Pos.X : ref Pos.Y);
            ref var remainder = ref (dim == 0 ? ref Remainder.X : ref Remainder.Y);
            ref var size = ref (dim == 0 ? ref Size.W : ref Size.H);

            var move = MathF.Floor(remainder);
            remainder -= move;

            pos += (int)move;
            if (move == 0)
            {
                continue;
            }

            foreach (var other in Main.World.GameState.Entities)
            {
                if (other == this) continue;

                // if collision is enabled and the other entity intersects with the post move position, try and push them out of the way
                var intersects = currCollidable == CollisionType.Enabled && other.WorldCollide && Util.EntIntersect(this, other);
                if (intersects)
                {
                    ref var oPos = ref (dim == 0 ? ref other.Pos.X : ref other.Pos.Y);
                    ref var oSize = ref (dim == 0 ? ref other.Size.W : ref other.Size.H);

                    // find minimum amount of movement to resolve intersection.
                    var amt = MathF.Sign(move) > 0 ? (pos + size) - oPos : pos - (oPos + oSize);
                    if (!other.Move(dim, amt))
                    {
                        other.Die();
                    }
                }
                else if (riding.Contains(other))
                {
                    other.Move(dim, move);
                }
            }
        }

        Collidable = currCollidable;
    }

    public IEnumerable<Entity> FindTriggers()
    {
        foreach (var other in Main.World.GameState.Entities)
        {
            if (this == other) continue;
            if (other.Destroyed) continue;
            if (other.CanCollide(this, Dir.None) != CollisionType.Trigger) continue;
            if (!Util.EntIntersect(this, other)) continue;
            yield return other;
        }
    }

    public CollisionType StandardEnemyCanCollide(Entity other, Dir dir)
    {
        if (other is Player && dir == Dir.Up) return CollisionType.Platform;
        else if (other is Player) return CollisionType.Trigger;
        else return CollisionType.Enabled;
    }

    // FIXME: other.hurt shouldn't be in here maybe? also the die param is kinda smelly)
    public bool HandleEnemyStomp(Entity other, Dir dir, bool die = true)
    {
        if (other is Player && dir == Dir.Up && other.Max.Y <= Min.Y) {
            if (die) Die();
            ((Player)other).StompEnemy();
            return true;
        }
        else if (other is Player) {
            other.Hurt(1);
            return false;
        }

        return false;
    }

    public uint Ticks { get => this.RunWhilePaused ? Main.World.GameState.WallTicks : Main.World.GameState.Ticks; }
}

class WorldEnt : Entity
{
    public WorldEnt()
    {
        this.Type = "World";
        Collidable = CollisionType.Disabled;
    }

    public override void Hurt(int dmg) { }
    public override void Die() { }
    public override void Draw() { }

    public static WorldEnt Value = new WorldEnt();
}

//[System.AttributeUsage(System.AttributeTargets.Class)]
class Spawnable //: System.Attribute
{
    public static Dictionary<string, Type> EntityMaps = new Dictionary<string, Type>
    {
        { "Coin", typeof(Coin) },
        { "Player", typeof(Player) },

    };

    //public Spawnable(string name)
    //{
    //    EntityMaps[name] = typeof(Entity);
    //}
}