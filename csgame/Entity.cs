using Slate2D;
using System.Diagnostics.CodeAnalysis;
using System.Reflection;

public enum Layer {
  Back,
  Background,
  Normal,
  Foreground
}

public class Entity {
  public readonly string Id;
  public string Type = "Default";
  public string Name = "";
  public bool Awake = false;
  public bool Destroyed = false;
  public (int X, int Y) Pos = (0, 0);
  public (int W, int H) Size = (0, 0);
  public (float X, float Y) Vel = (0, 0);
  public (int X, int Y) DrawOfs = (0, 0);
  public Layer Layer = Layer.Normal;

  public (float X, float Y) Remainder = (0, 0);
  public AssetHandle Sprite = 0;
  public uint Frame = 0;
  public byte FlipBits = 0;
  // default reaction if canCollide is not overridden
  public CollisionType Collidable = CollisionType.Enabled;
  // whether this entity collides with the world, or moveSolid entities
  public bool WorldCollide = true;
  public bool RunWhilePaused = false;

  public Entity? CollideEnt;
  public Tile CollideTile = Tile.Empty;
  public bool AnyInSlope = false;

  public Entity() {
    Id = Guid.NewGuid().ToString();
  }

  public Entity(LDTKEntity ent) {
    Id = ent.Iid;
    Size = ent.Size;
    Pos = ent.Pos;
    Type = ent.Type;

    //var targetType = GetType();
    //foreach (var property in ent.Properties)
    //{
    //    var targProp = targetType.GetProperty(property.Key);
    //    if (targProp != null)
    //    {
    //        continue;
    //        //targProp.SetValue(this, propertyGetValue(source));
    //    }
    //}

    var colTypeStr = ent.Properties.GetValueOrDefault("CollisionType", null)?.Str ?? "Enabled";
    colTypeStr = colTypeStr.Length > 0 ? colTypeStr : "Enabled";
    Collidable = (CollisionType)Enum.Parse(typeof(CollisionType), colTypeStr);
  }

  public override string ToString() {
    return $"[Entity {Type}]";
  }

  public virtual void PreUpdate(uint ticks, float dt) {

  }

  public virtual void Update(uint ticks, float dt) {

  }

  public virtual void Draw() {
    DC.SetColor(255, 255, 255, 255);
    DC.Sprite(Sprite, Frame, Pos.X + DrawOfs.X, Pos.Y + DrawOfs.Y, 1, FlipBits, 1, 1);
  }

  public void DrawBbox(byte r, byte g, byte b, byte a) {
    DC.SetColor(r, g, b, a);
    DC.Rect(Pos.X, Pos.Y, Size.W, Size.H, false);
  }

  // callback to determine what type of collision based on the entity
  public virtual CollisionType CanCollide(Entity other, Dir dir) { return Collidable; }
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
  protected Tile TileAt(int x, int y) {
    var layer = Main.World.Map.LayersByName["Collision"];
    var tx = x / layer.TileSize;
    var ty = Math.Clamp(y / layer.TileSize, 0, layer.Size.H - 1);

    if (x < 0 || tx > layer.Size.W) return Tile.Solid;

    return (Tile)layer.Tiles[ty * layer.Size.W + tx];
  }

  // returns true/false if there is a collision at the specified coordinates.
  // this only queries the world, but it will update this.collideEnt
  public bool CollideAt(int x, int y, Dir dir) {
    var corners = new (int X, int Y)[]
    {
            (x, y),
            (x + Size.W - 1, y),
            (x, y + Size.H - 1),
            (x + Size.W - 1, y + Size.H - 1),
    };

    // different bottommiddle than class property
    (int X, int Y) bottomMiddle = (x + Size.W / 2, corners[2].Y);

    var layer = Main.World.Map.LayersByName["Collision"];
    var opposite = Util.GetOppositeDir(dir);

    CollideEnt = null;
    CollideTile = Tile.Empty;

    // iterate through all entities looking for a collision
    foreach (var other in Main.World.GameState.Entities) {
      if (other == this) continue;

      var intersects = Util.RectIntersect(corners[0], Size, other.Pos, other.Size);

      if (intersects) {
        if (other.CanCollide(this, opposite) == CollisionType.Disabled) continue;
        if (CanCollide(other, dir) == CollisionType.Disabled) continue;

        // both sides need to be solid
        if (CanCollide(other, dir) == CollisionType.Enabled && other.CanCollide(this, opposite) == CollisionType.Enabled) {
          CollideEnt = other;
          return true;
        }
        // need to check both sides of platform collision otherwise you might fall through
        else if (CanCollide(other, dir) == CollisionType.Platform && dir == Dir.Up && other.Max.Y >= y) {
          CollideEnt = other;
          return true;
        }
        // original platform check
        else if (other.CanCollide(this, opposite) == CollisionType.Platform && dir == Dir.Down && corners[2].Y == other.Pos.Y) {
          CollideEnt = other;
          return true;
        }
      }
    }

    // check bottom middle point if its in a slope
    var tx = bottomMiddle.X / layer.TileSize;
    var ty = Math.Clamp(bottomMiddle.Y / layer.TileSize, 0, layer.Size.H - 1);
    Tile tid = (Tile)layer.Tiles[ty * layer.Size.W + tx];

    AnyInSlope = false;

    if (!WorldCollide) {
      return false;
    }

    // check if we're in the solid part of the slope (always 45 degrees)
    if (tid == Tile.SlopeL || tid == Tile.SlopeR) {
      var localX = bottomMiddle.X % layer.TileSize;
      var localY = bottomMiddle.Y % layer.TileSize;
      var minY = tid == Tile.SlopeR ? localX : layer.TileSize - localX;
      CollideTile = localY >= minY ? tid : Tile.Empty;
      AnyInSlope = true;
      return localY >= minY;
    }

    // check against tilemap
    // iterate through corners. note this will currently break if entities are > tileSize
    foreach (var corner in corners) {
      tx = corner.X / layer.TileSize;
      ty = Math.Clamp(corner.Y / layer.TileSize, 0, layer.Size.H);
      tid = ty >= layer.Size.H ? Tile.Empty : (Tile)layer.Tiles[ty * layer.Size.W + tx];

      if (corner.X < 0 || tx >= layer.Size.W) {
        CollideTile = Tile.Solid;
        return true;
      }

      //if there's a tile in the intgrid...
      if (tx < 0 || tx > layer.Size.W || tid != Tile.Empty) {
        if (tid == Tile.Dirtback) {
          continue;
        }

        // if it's a ground sloped tile, only bottom middle pixel should collide with it
        if (tid == Tile.SlopeL || tid == Tile.SlopeR) {
          AnyInSlope = true;
          continue;
        }

        // if it's a platform, check if dir is down, and only block if bottom of entity
        // intersects with the first pixel of the platform block
        if (tid == Tile.Platform) {
          if (dir == Dir.Down && corner.Y == corners[2].Y && corner.Y % layer.TileSize == 0) {
            CollideTile = tid;
            return true;
          }
          continue;
        }

        CollideTile = tid;
        return true;
      }
    }

    return false;
  }

  private bool Move(int dim, float amt, bool skipRemainder = false) {
    ref var pos = ref (dim == 0 ? ref Pos.X : ref Pos.Y);
    ref var remainder = ref (dim == 0 ? ref Remainder.X : ref Remainder.Y);

    float move = amt;
    if (!skipRemainder) {
      remainder += amt;
      move = remainder > 0 ? MathF.Floor(remainder) : MathF.Ceiling(remainder);
    }

    if (move == 0) {
      foreach (var other in FindTriggers()) other.Collide(this, Dir.None);
      return true;
    }

    if (!skipRemainder) remainder -= move;
    var sign = Math.Sign(move);

    var dir = dim == 0 ? (sign > 0 ? Dir.Right : Dir.Left) : (sign > 0 ? Dir.Down : Dir.Up);
    var opposite = Util.GetOppositeDir(dir);

    var fullMove = true;
    while (move != 0) {
      var check = pos + sign;
      var collision = dim == 0 ? CollideAt(check, Pos.Y, dir) : CollideAt(Pos.X, check, dir);
      if (!collision) {
        pos += sign;
        move -= sign;
      }
      else {
        // if moving left/right, step up 1 pixel to check for moving up a slope
        if (dim == 0) {
          if (!CollideAt(check, Pos.Y - 1, Dir.Up)) {
            Pos.X += sign;
            Pos.Y -= 1;
            move -= sign;
            continue;
          }
        }

        if (CollideEnt != null) {
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

    if (Pos.Y > Main.World.Map.PxSize.h + 16) {
      Die();
    }

    return fullMove;
  }

  public bool MoveX(float amt) { return Move(0, amt); }
  public bool MoveY(float amt) { return Move(1, amt); }

  public IEnumerable<Entity> GetRidingEntities() {
    foreach (var other in Main.World.GameState.Entities) {
      if (other.WorldCollide &&
          !Util.EntIntersect(this, other) &&
          Util.RectIntersect(Pos, Size, (other.Pos.X, other.Pos.Y + 1), other.Size)) {
        yield return other;
      }
    }

  }

  // move a solid object that is not constrained by world collision (movers)
  public void MoveSolid(float x, float y) {
    Remainder.X += x;
    Remainder.Y += y;

    // disable collision temporarily
    var currCollidable = Collidable;
    Collidable = CollisionType.Disabled;

    // not ideal but needs to be done before the move
    // riding is true if the other entity isn't intersecting them but one pixel down vertically does
    var riding = GetRidingEntities().ToList(); // FIXME: gc?

    for (int dim = 0; dim < 2; dim++) {
      ref var pos = ref (dim == 0 ? ref Pos.X : ref Pos.Y);
      ref var remainder = ref (dim == 0 ? ref Remainder.X : ref Remainder.Y);
      ref var size = ref (dim == 0 ? ref Size.W : ref Size.H);

      var move = MathF.Floor(remainder);
      remainder -= move;

      pos += (int)move;
      if (move == 0) {
        continue;
      }

      for (int i = 0; i < Main.World.GameState.Entities.Count; i++) {
        var other = Main.World.GameState.Entities[i];
        if (other == this) continue;

        // if collision is enabled and the other entity intersects with the post move position, try and push them out of the way
        var intersects = currCollidable == CollisionType.Enabled && other.WorldCollide && Util.EntIntersect(this, other);
        if (intersects) {
          ref var oPos = ref (dim == 0 ? ref other.Pos.X : ref other.Pos.Y);
          ref var oSize = ref (dim == 0 ? ref other.Size.W : ref other.Size.H);

          // find minimum amount of movement to resolve intersection.
          var amt = MathF.Sign(move) > 0 ? (pos + size) - oPos : pos - (oPos + oSize);
          if (!other.Move(dim, amt, true)) {
            other.Die();
          }
        }
        else if (riding.Contains(other)) {
          other.Move(dim, move, true);
        }
      }
    }

    Collidable = currCollidable;
  }

  public IEnumerable<Entity> FindTriggers() {
    // needs to be a for loop since may be modified during iteration
    // (puffparticles and such)
    for (int i = 0; i < Main.World.GameState.Entities.Count; i++) {
      var other = Main.World.GameState.Entities[i];
      if (this == other) continue;
      if (other.Destroyed) continue;
      if (other.CanCollide(this, Dir.None) != CollisionType.Trigger) continue;
      if (!Util.EntIntersect(this, other)) continue;
      yield return other;
    }
  }

  public CollisionType StandardEnemyCanCollide(Entity other, Dir dir) {
    if (other is Player && dir == Dir.Up) return CollisionType.Platform;
    else if (other is Player) return CollisionType.Trigger;
    else return CollisionType.Enabled;
  }

  // FIXME: other.hurt shouldn't be in here maybe? also the die param is kinda smelly)
  public bool HandlePlayerStomp(Entity other, Dir dir, bool die = true) {
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

  public uint Animate<TEnum>(TEnum[] animation, uint startTime, uint endTime) where TEnum : Enum {
    var v = Util.InvLerp<float>(startTime, endTime, Ticks);
    var idx = v == 1f ? animation.Length - 1 : (int)(v * animation.Length);
    return Convert.ToUInt32(animation[idx]);
  }

  public uint Ticks { get => this.RunWhilePaused ? Main.World.GameState.WallTicks : Main.World.GameState.Ticks; }
}

class WorldEnt : Entity {
  public WorldEnt(LDTKEntity ent) : base(ent) {
    this.Type = "World";
    Collidable = CollisionType.Disabled;
  }

  public override void Hurt(int dmg) { }
  public override void Die() { }
  public override void Draw() { }

  public static WorldEnt Value = new WorldEnt(new LDTKEntity());
}

[System.AttributeUsage(System.AttributeTargets.Class)]
class Spawnable : System.Attribute {
  string Name;
  public Spawnable(string name = null) {
    Name = name;
  }

  public static Dictionary<string, Type> EntityMaps = new() { };

  [RequiresUnreferencedCode("Calls System.Reflection.Assembly.GetTypes()")]
  public static void ConfigureSpawnables() {
    EntityMaps.Clear();

    Assembly assembly = Assembly.GetAssembly(typeof(Entity));
    if (assembly == null) return;

    Type[] types = assembly.GetTypes();
    foreach (Type type in types) {
      var attributes = type.GetCustomAttributes(typeof(Spawnable), false);
      if (attributes?.Length > 0) {
        Spawnable attrib = (Spawnable)attributes[0];
        EntityMaps[attrib.Name ?? type.Name] = type;
      }

    }
  }
}
