using Slate2D;
using System.Text.RegularExpressions;
using static Slate2D.Assets.AssetConfig;

public record GameState
{
    public float T = 0;
    public uint Ticks = 0;
    public bool Paused = false;
    public uint WallTicks = 0;
    public List<Entity> Entities = new List<Entity>();
    public string MapName = "";
    public uint NextMap = 0;
    public uint CurrentCoins = 0;
    public uint MaxCoins = 0;
    public bool LevelComplete = false;
    public uint LevelCompleteTicks = 0;
    public bool CheckpointActive = false;
    public (int X, int Y) CheckpointPos;
}

public record Background
{
    public AssetHandle Id;
    public (int X, int Y) Pos;
    public (int W, int H) Size;
    public bool Random;
}

public class Game : IScene
{
    public (int W, int H) Res;
    public LDTK Map;
    public Camera Camera;
    float Accumulator = 0;
    Player Player;
    public GameState GameState = new GameState();
    AssetHandle Canvas;
    AssetHandle DogSpr;
    AssetHandle HealthSpr;
    AssetHandle CoinSpr;
    AssetHandle BlueFont;
    List<Background> Backgrounds = new List<Background>();
    List<Background> Clouds = new List<Background>();

    string MapName = "";
    int NextMap = 0;

    public Game(string mapName)
    {
        var windowRes = SLT.GetResolution();
        int scaleFactor = windowRes.w / 384;
        Res = (windowRes.w / scaleFactor, windowRes.h / scaleFactor);
        Camera = new Camera(Res.W, Res.H);
        Canvas = Assets.Load(new Canvas("canvas", Res.W, Res.H));

        // FIXME: loadAllAssets();
        DogSpr = Assets.Find("dogspr");
        HealthSpr = Assets.Find("health");
        CoinSpr = Assets.Find("coin");
        BlueFont = Assets.Find("blueFont");

        MapName = mapName ?? "maps/map1.ldtkl";

        var match = Regex.Match(MapName, @"map(\d+)", RegexOptions.Multiline);
        NextMap = int.Parse(match.Groups[1].Value ?? "1") + 1;

        string mapStr = FS.ReadTextFile(MapName);
        Map = new LDTK(mapStr);
        // FIXME: freefile?

        var entLayer = Map.LayersByName["Entities"];
        foreach (var ent in entLayer.Entities)
        {
            Type t;
            if (Spawnable.EntityMaps.TryGetValue(ent.Type, out t))
            {
                var newEnt = Activator.CreateInstance(t, ent);
                if (newEnt != null) GameState.Entities.Add((Entity)newEnt);
            }
        }

        var bgProp = this.Map.Background switch
        {
            "grassland" => (Key: "grassland", NumBgs: 3, NumClouds: 3, Random: true),
            "snowland" => (Key: "snowland", NumBgs: 4, NumClouds: 3, Random: false),
            _ => (Key: "grassland", NumBgs: 3, NumClouds: 3, Random: true),
        };

        Backgrounds = Enumerable.Range(0, bgProp.NumBgs).Select(i =>
        {
            var name = $"gfx/{bgProp.Key}_bg{i}.png";
            var id = Assets.Load(new Image(name, name));
            var size = Assets.ImageSize(id);

            return new Background
            {
                Id = id,
                Size = (size.w, size.h),
                Pos = (0, Res.H - size.h),
                Random = bgProp.Random
            };
        }).ToList();

        Clouds = Enumerable.Range(0, bgProp.NumClouds).Select(i =>
        {
            var name = $"gfx/{bgProp.Key}_cloud{i}.png";
            var id = Assets.Load(new Image(name, name));
            var size = Assets.ImageSize(id);
            var rnd = new Random();

            return new Background
            {
                Id = id,
                Size = (size.w, size.h),
                Pos = (bgProp.Random ? rnd.Next(-300, 0) : 0, bgProp.Random ? rnd.Next(5, 120) : Res.H - size.h),
                Random = bgProp.Random
            };
        }).ToList();

        // setup player
        Player = (Player)GameState.Entities.First(ent => ent.Type == "Player");

        // hack to draw the player at the end since i don't yet support draw order
        GameState.Entities.Remove(Player);
        GameState.Entities.Add(Player);

        GameState.MaxCoins = GameState.CurrentCoins + (uint)GameState.Entities.Count(ent => ent.Type == "Coin");

        // setup camera
        Camera.Constrain(0, 0, Map.PxSize.w, Map.PxSize.h);
        Camera.Window(Player.Pos.X, Player.Pos.Y, 20, 20);
    }

    public void Update(float dt)
    {
        Accumulator += dt;

        // if we're running at ~58ish fps, pretend its a full frame
        while (Accumulator > 0.0164)
        {
            // FIXME: reset accumulated prints
            // clearPrintWin();
            // setRetain(true);

            // always step at the same speed and subtract a little extra in case we're at ~62ish fps
            Accumulator = MathF.Max(0, Accumulator - 0.0175f);
            if (!GameState.Paused)
            {
                GameState.T += 1 / 60;
                GameState.Ticks += 1;
            }

            GameState.WallTicks += 1;

            if (GameState.LevelComplete && GameState.Ticks > GameState.LevelCompleteTicks)
            {
                Main.SwitchLevel(GameState.NextMap);
                return;
            }

            //run preupdate on all entities before updating
            foreach (var ent in this.GameState.Entities) {
                if (ent.Destroyed) continue;
                if (GameState.Paused && !ent.RunWhilePaused) continue;
                ent.PreUpdate(ent.RunWhilePaused ? GameState.WallTicks : GameState.Ticks, dt);
            }

            foreach (var ent in this.GameState.Entities)
            {
                if (ent.Destroyed) continue;
                if (GameState.Paused && !ent.RunWhilePaused) continue;
                ent.Update(ent.RunWhilePaused ? GameState.WallTicks : GameState.Ticks, dt);
            }

            // update camera to player
            Camera.Window(Player.Pos.X, Player.Pos.Y, 20, 20);

            // kill all entities that are disabled
            for (int i = GameState.Entities.Count - 1; i >= 0; i--)
            {
                if (GameState.Entities[i].Destroyed)
                {
                    GameState.Entities.RemoveAt(i);
                }
            }
        }

        // SLT.printWin('frame', 'frame', true);
        // FIXME:setRetain(false);
    }

    public void Draw()
    {
        DC.Clear(30, 30, 30, 255);

        DC.SetColor(255, 255, 255, 255);
        Map.Draw("BGTiles");
        Map.Draw("BGWorld");
        Map.Draw("BGDecoration");
        Map.Draw("Collision");
        DC.SetColor(255, 255, 255, 255);
        DC.Submit();
    }
}