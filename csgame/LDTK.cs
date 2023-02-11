using Slate2D;
using System.Globalization;
using System.Text.Json.Nodes;
using static Slate2D.Assets.AssetConfig;

public record LDTKProperty
{
    public float Num;
    public string Str = "";
    public bool Bool;
    public (float x, float y) Point;
}

public record class LDTKLayer
{
    public string Name = "";
    public (int w, int h) Size = (0, 0);
    public int TileSize = 0;
    public (int x, int y) Offset = (0, 0);
    public int[][] DrawTiles;
    public AssetHandle TilesetHnd;
    public uint[] Tiles;
    public LDTKEntity[] Entities;
}

public record class LDTKEntity
{
    public string Type = "";
    public (int w, int b) Size = (0, 0);
    public (int x, int y) Pos = (0, 0);
    public Dictionary<string, LDTKProperty> Properties = new();
}

public class LDTK
{
    public (int w, int h) PxSize = (0, 0);
    public string Background;
    public (byte r, byte g, byte b) BGColor = (0, 0, 0);
    public Dictionary<string, LDTKProperty> Properties = new();
    public List<LDTKLayer> Layers = new();
    public Dictionary<string, LDTKLayer> LayersByName = new();

    private Dictionary<string, LDTKProperty> ParseProperties(JsonNode properties)
    {
        var dict = new Dictionary<string, LDTKProperty>();

        foreach (var node in properties.AsArray())
        {
#pragma warning disable CS8602 // Dereference of a possibly null reference.
            string key = node["__identifier"].GetValue<string>();
            JsonNode val = node["__value"];

            if (val == null) continue;

            dict[key] = new LDTKProperty
            {
                Num = val.GetType() == typeof(float) ? val.GetValue<float>() : 0,
                Str = val.GetType() == typeof(string) ? val.GetValue<string>() : "",
                Bool = val.GetType() == typeof(bool) ? val.GetValue<bool>() : false,
            };

            if (node["__type"].GetValue<string>() == "Point")
            {
                dict[key].Point = (val["cx"].GetValue<float>(), val["cy"].GetValue<float>());
            }
#pragma warning restore CS8602 // Dereference of a possibly null reference.
        }

        return dict;
    }

    public LDTK(string jsonStr)
    {
        JsonNode obj = JsonNode.Parse(jsonStr);

        if (obj == null)
        {
            throw new InvalidDataException();
        }

#pragma warning disable CS8602 // Dereference of a possibly null reference.
        PxSize = (obj["pxWid"].GetValue<int>(), obj["pxHei"].GetValue<int>());

        int rgb = Int32.Parse(obj["__bgColor"].GetValue<string>().Replace("#", ""), NumberStyles.HexNumber);
        BGColor = ((byte)((rgb >> 16) & 255), (byte)((rgb >> 8) & 255), (byte)(rgb & 255));

        // turn into a reasonable k/v object
        Properties = ParseProperties(obj["fieldInstances"]);
        Background = Properties.ContainsKey("background") ? Properties["background"].Str : "grassland";

        // parse each layer
        foreach (var layer in obj["layerInstances"].AsArray())
        {
            bool isEntityLayer = layer["__type"].GetValue<string>() == "Entities";
            var lobj = new LDTKLayer
            {
                Name = layer["__identifier"].GetValue<string>(),
                Size = (layer["__cWid"].GetValue<int>(), layer["__cHei"].GetValue<int>()),
                TileSize = layer["__gridSize"].GetValue<int>(),
                Offset = (layer["__pxTotalOffsetX"].GetValue<int>(), layer["__pxTotalOffsetY"].GetValue<int>()),
                Entities = isEntityLayer ? new LDTKEntity[layer["entityInstances"].AsArray().Count] : new LDTKEntity[0],
            };

            if (layer["__type"].GetValue<string>() == "Entities")
            {
                var arr = layer["entityInstances"].AsArray();
                lobj.Entities = new LDTKEntity[arr.Count];
                int i = 0;
                foreach (var ent in arr.AsArray())
                {
                    var pivot = ent["__pivot"].AsArray();
                    var px = ent["px"].AsArray();
                    var width = ent["width"].GetValue<int>();
                    var height = ent["height"].GetValue<int>();

                    var type = ent["__identifier"].GetValue<string>();
                    var size = (width, height);
                    var pos = ((int)(px[0].GetValue<int>() - width * pivot[0].GetValue<float>()),
                            (int)(px[1].GetValue<int>() - height * pivot[1].GetValue<float>()));
                    var props = ParseProperties(ent["fieldInstances"]);

                    lobj.Entities[i++] = new LDTKEntity
                    {
                        Type = type,
                        Size = size,
                        Pos = pos,
                        Properties = props,
                    };
                    continue;
                }
            }
            else
            {
                lobj.TilesetHnd = Assets.Load(new Sprite(
                    Name: layer["__tilesetRelPath"].GetValue<string>(),
                    Path: layer["__tilesetRelPath"].GetValue<string>(),
                    SpriteWidth: lobj.TileSize,
                    SpriteHeight: lobj.TileSize,
                    MarginX: 0,
                    MarginY: 0
                ));

                if (layer["__type"].GetValue<string>() == "IntGrid")
                {
                    var arr = layer["intGridCsv"].AsArray();
                    lobj.Tiles = new uint[arr.Count];
                    int i = 0;
                    foreach (var tile in arr)
                    {
                        lobj.Tiles[i++] = tile.GetValue<uint>();
                    }
                }

                // ldtk can stack tiles in the same layer
                // handle this by making a new array when there's a stack
                int sz = layer["__cWid"].GetValue<int>() * layer["__cHei"].GetValue<int>();
                var drawTiles = new List<int[]>
                {
                    Enumerable.Repeat(-1, sz).ToArray()
                };

                JsonArray tiles = layer["__type"].GetValue<string>() == "Tiles" ? layer["gridTiles"].AsArray() : layer["autoLayerTiles"].AsArray();

                foreach (var t in tiles)
                {
                    var px = t["px"].AsArray();
                    var tileidx = (px[1].GetValue<int>() / lobj.TileSize) * lobj.Size.w + px[0].GetValue<int>() / lobj.TileSize;
                    // look for an open space on existing layers
                    for (int i = 0; i < drawTiles.Count; i++)
                    {
                        if (drawTiles[i][tileidx] == -1)
                        {
                            drawTiles[i][tileidx] = t["t"].GetValue<int>();
                            break;
                        }

                        // we're out of space, add a new layer
                        if (i + 1 == drawTiles.Count)
                        {
                            drawTiles.Add(Enumerable.Repeat(-1, sz).ToArray());
                            drawTiles[i][tileidx] = t["t"].GetValue<int>();
                            break;
                        }
                    }
                }

                lobj.DrawTiles = drawTiles.ToArray();

            }

            Layers.Add(lobj);
            LayersByName[lobj.Name] = lobj;
        }

        Layers.Reverse();
#pragma warning restore CS8602 // Dereference of a possibly null reference.
    }

    public void Draw(string layerName)
    {
        var l = LayersByName[layerName];
        foreach (var tmap in l.DrawTiles)
        {
            DC.Tilemap(l.TilesetHnd, l.Offset.x, l.Offset.y, l.Size.w, l.Size.h, tmap);
        }
    }
}