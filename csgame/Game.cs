using Slate2D;
using System.Text.RegularExpressions;
using static Slate2D.Assets.AssetConfig;

public record GameState
{
    public float T = 0;
    public uint Ticks = 0;
    public bool Paused = false;
    public uint WallTicks = 0;
    public List<Entity> Entities = new();
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
    List<Background> Backgrounds = new();
    List<Background> Clouds = new();

    string MapName = "";
    int NextMap = 0;

    public Game(string mapName)
    {
        Spawnable.ConfigureSpawnables();

        var windowRes = SLT.GetResolution();
        int scaleFactor = windowRes.w / 384;
        Res = (windowRes.w / scaleFactor, windowRes.h / scaleFactor);
        Camera = new Camera(Res.W, Res.H);
        Canvas = Assets.Load(new Canvas("canvas", Res.W, Res.H));

        AssetList.LoadAllAssets();
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
            if (Spawnable.EntityMaps.TryGetValue(ent.Type, out var t))
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
            PW.ClearPrintWin();
            PW.Retained = true;

            // always step at the same speed and subtract a little extra in case we're at ~62ish fps
            Accumulator = MathF.Max(0, Accumulator - 0.0175f);
            if (!GameState.Paused)
            {
                GameState.T += 1 / 60.0f;
                GameState.Ticks += 1;
            }

            GameState.WallTicks += 1;

            if (GameState.LevelComplete && GameState.Ticks > GameState.LevelCompleteTicks)
            {
                Main.SwitchLevel(GameState.NextMap);
                return;
            }

            //run preupdate on all entities before updating
            for (int i = 0; i < Main.World.GameState.Entities.Count; i++)
            {
                var ent = Main.World.GameState.Entities[i];
                if (ent.Destroyed) continue;
                if (GameState.Paused && !ent.RunWhilePaused) continue;
                ent.PreUpdate(ent.RunWhilePaused ? GameState.WallTicks : GameState.Ticks, dt);
            }

            for (int i = 0; i < Main.World.GameState.Entities.Count; i++)
            {
                var ent = Main.World.GameState.Entities[i];
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
            PW.Retained = false;
        }

        PW.DrawPrintWin();
    }

    public void Draw()
    {
        // clear screen
        DC.Clear(0, 0, 0, 255);
        DC.UseCanvas(Canvas);

        // draw map background, needs to change based on environment
        DC.Clear(Map.BGColor.r, Map.BGColor.g, Map.BGColor.b, 255);

        var res = this.Res;
        var t = GameState.T;

        // clouds which scroll, no parallax
        for (int i = 0; i < Clouds.Count; i++)
        {
            var bg = Clouds[i];

            var speed = (i + 1) * 6;
            var x = res.W + (bg.Pos.X - t * speed) % (res.W + bg.Size.W);
            if (bg.Random)
            {
                DC.Image(bg.Id, x, bg.Pos.Y, 0, 0, 1, 0, 0, 0);
            }
            else
            {
                while (x > 0)
                {
                    x -= bg.Size.W;
                }
                while (x < res.W)
                {
                    DC.Image(bg.Id, x, bg.Pos.Y, 0, 0, 1, 0, 0, 0);
                    x += bg.Size.W;
                }
            }
        }

        // parallax bgs
        float camY = 1 - Camera.y / (float)(Map.PxSize.h - res.H);
        var camYoffset = camY * 50;
        for (int i = 0; i < Backgrounds.Count; i++)
        {
            var bg = Backgrounds[i];
            
            var speed = (i + 1) * 0.25f;
            var x = (int)((bg.Pos.X - Camera.x) * speed) % bg.Size.W;
            var y = (int)(bg.Pos.Y + camYoffset);
            var bgx = x;
            while (bgx > 0)
            {
                bgx -= bg.Size.W;
            }
            while (bgx < res.W)
            {
                DC.Image(bg.Id, bgx, y, 0, 0, 1, 0, 0, 0);
                bgx += bg.Size.W;
            }
        }

        // running dog
        if (Map.Background == "grassland")
        {
            var x = (int)(t * 50f) % (res.W + 22) - 22;
            var y = (int)(Math.Sin(x / 50f) * 5 + (res.H * 0.8f));
            DC.SetColor(255, 255, 255, 255);
            DC.Sprite(DogSpr, (uint)(t * 12) % 6, x, y + camYoffset, 1, 0, 1, 1);
        }

        // dim the background slightly
        DC.SetColor(99, 155, 255, 60);
        DC.Rect(0, 0, res.W, res.H, false);
        DC.SetColor(255, 255, 255, 255);

        // start drawing from camera viewpoint
        Camera.DrawStart();

        // tilemap and entities
        DC.SetColor(255, 255, 255, 255);
        foreach (var ent in GameState.Entities) if (ent.Layer == Layer.Back) ent.Draw();
        Map.Draw("BGTiles");
        Map.Draw("BGWorld");
        Map.Draw("BGDecoration");
        foreach (var ent in GameState.Entities) if (ent.Layer == Layer.Background) ent.Draw();
        Map.Draw("Collision");
        foreach (var ent in GameState.Entities) if (ent.Layer == Layer.Normal) ent.Draw();
        foreach (var ent in GameState.Entities) if (ent.Layer == Layer.Foreground) ent.Draw();
        DC.SetColor(255, 255, 255, 255);

        Camera.DrawEnd();

        // player hud

        // coin display 
        DC.SetTextStyle(BlueFont, 1, 1, 1);
        DC.Sprite(CoinSpr, 0, 85, 8, 1, 0, 1, 1);
        if (GameState.CurrentCoins == GameState.MaxCoins)
        {
            DC.SetColor(255, 200, 0, 255);
        }
        DC.Text(105, 11, 300, $"{GameState.CurrentCoins}/{GameState.MaxCoins}", 0);
        DC.SetColor(255, 255, 255, 255);

        // health
        for (int i = 0; i < Player.MaxHealth; i++)
        {
            DC.Sprite(HealthSpr, i + 1 <= Player.Health ? 0u : 1, 14 + i * 20, 7, 1, 0, 1, 1);
        }

        // draw the canvas into the center of the window
        var screen = SLT.GetResolution();
        var scale = screen.h / res.H;
        DC.ResetCanvas();
        DC.SetColor(255, 255, 255, 255);
        DC.Image(Canvas, (screen.w - (res.W * scale)) / 2, (screen.h - (res.H * scale)) / 2, res.W, res.H, scale, 0, 0, 0);

        DC.Submit();
    }

    // spawn an entity into the map as active
    public Entity? spawnEntity(string type)
    {
        if (Spawnable.EntityMaps.TryGetValue(type, out var t))
        {
            var newEnt = Activator.CreateInstance(t);
            if (newEnt != null) GameState.Entities.Add((Entity)newEnt);
                GameState.Entities.Add((Entity)newEnt);
            return (Entity)newEnt;

        }

        return null;
    }

    // spawns a lil puff of smoke
    public void SpawnPuffParticle(int x, int y)
    {
        var puffEnt = new PuffParticle();
        puffEnt.Pos = (x, y);
        puffEnt.Start = GameState.Ticks;
        GameState.Entities.Add(puffEnt);
    }
 
    // spawn a placeholder enemy that flings out of the screen
    public SpinParticle SpawnDeathParticle(Entity ent, uint frame)
    {
        SpawnPuffParticle(ent.Pos.X, ent.Pos.Y);

        var deathEnt = new SpinParticle();
        deathEnt.Pos = ent.Pos;
        deathEnt.Sprite = ent.Sprite;
        deathEnt.Frame = frame;
        deathEnt.Size = ent.Size;
        deathEnt.DrawOfs = ent.DrawOfs;
        deathEnt.Start = GameState.Ticks;
        deathEnt.Vel.X *= Math.Sign(ent.Center.X - Player.Center.X);
        GameState.Entities.Add(deathEnt);

        return deathEnt;
    }

    public void CompleteLevel()
    {
        GameState.LevelComplete = true;
        GameState.LevelCompleteTicks = GameState.Ticks + 120;
    }

    public void FailLevel()
    {
        Main.SwitchLevel(GameState.NextMap - 1, GameState.CheckpointActive ? GameState.CheckpointPos : null);
    }
}