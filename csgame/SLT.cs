global using AssetHandle = System.UInt32;
using System.Reflection.PortableExecutable;
using System.Runtime.InteropServices;
using static System.Net.Mime.MediaTypeNames;

namespace Slate2D
{
    public record Asset
    {
        public record Image(string name, string path, bool linearFilter = false) : Asset();
        public record Sprite(string name, string path, int spriteWidth, int spriteHeight, int marginX, int marginY) : Asset();
        public record Speech(string name, string text) : Asset();
        public record Sound(string name, string path) : Asset();
        public record Mod(string name, string path) : Asset();
        public record Font(string name, string path) : Asset();
        public record BitmapFont(string name, string path, string glyphs, int glyphWidth, int charSpacing, int spaceWidth, int lineHeight) : Asset();
        public record Canvas(string name, int width, int height) : Asset();
        public record Shader(string name, bool isFile, string vs, string fs) : Asset();

        private Asset() { }
    }

    public struct Position
    {
        public int x;
        public int y;
    }

    public struct Dimensions
    {
        public int w;
        public int h;
    }

    public partial class SLT
    {
        private const string LibName = "slate2d";

        [LibraryImport(LibName, EntryPoint = "SLT_Init", StringMarshalling = StringMarshalling.Utf8)]
        private static partial void _Init(int argc, [MarshalAs(UnmanagedType.LPArray)] string[] argv);

        public static void Init(string[] args)
        {
            string[] sltargs = args.Prepend(Environment.ProcessPath ?? ".").ToArray();
            _Init(sltargs.Length, sltargs);
        }

        [LibraryImport(LibName, EntryPoint = "SLT_Shutdown")]
        public static partial void Shutdown();

        [LibraryImport(LibName, EntryPoint = "SLT_StartFrame")]
        public static partial double StartFrame();

        [LibraryImport(LibName, EntryPoint = "SLT_EndFrame")]
        public static partial void EndFrame();

        [LibraryImport(LibName, EntryPoint = "SLT_UpdateLastFrameTime")]
        public static partial void UpdateLastFrameTime();

        [LibraryImport(LibName, EntryPoint = "SLT_GetResolution")]
        public static partial Dimensions GetResolution();
    }

    public partial class Assets
    {
        private const string LibName = "slate2d.dll";

        [LibraryImport(LibName, EntryPoint = "SLT_Asset_LoadImage")]
        private static partial AssetHandle _LoadImage([MarshalAs(UnmanagedType.LPWStr)] string name, [MarshalAs(UnmanagedType.LPWStr)] string path, [MarshalAs(UnmanagedType.U1)] bool linearFilter);

        [LibraryImport(LibName, EntryPoint = "SLT_Asset_LoadSprite")]
        private static partial AssetHandle _LoadSprite([MarshalAs(UnmanagedType.LPWStr)] string name, [MarshalAs(UnmanagedType.LPWStr)] string path, int spriteWidth, int spriteHeight, int marginX, int marginY);

        [LibraryImport(LibName, EntryPoint = "SLT_Asset_LoadSpeech")]
        private static partial AssetHandle _LoadSpeech([MarshalAs(UnmanagedType.LPWStr)] string name, [MarshalAs(UnmanagedType.LPWStr)] string text);

        [LibraryImport(LibName, EntryPoint = "SLT_Asset_LoadSound")]
        private static partial AssetHandle _LoadSound([MarshalAs(UnmanagedType.LPWStr)] string name, [MarshalAs(UnmanagedType.LPWStr)] string path);

        [LibraryImport(LibName, EntryPoint = "SLT_Asset_LoadMod")]
        private static partial AssetHandle _LoadMod([MarshalAs(UnmanagedType.LPWStr)] string name, [MarshalAs(UnmanagedType.LPWStr)] string path);

        [LibraryImport(LibName, EntryPoint = "SLT_Asset_LoadFont")]
        private static partial AssetHandle _LoadFont([MarshalAs(UnmanagedType.LPWStr)] string name, [MarshalAs(UnmanagedType.LPWStr)] string path);

        [LibraryImport(LibName, EntryPoint = "SLT_Asset_LoadBitmapFont")]
        private static partial AssetHandle _LoadBitmapFont([MarshalAs(UnmanagedType.LPWStr)] string name, [MarshalAs(UnmanagedType.LPWStr)] string path, [MarshalAs(UnmanagedType.LPWStr)] string glyphs, int glyphWidth, int charSpacing, int spaceWidth, int lineHeight);

        [LibraryImport(LibName, EntryPoint = "SLT_Asset_LoadCanvas")]
        private static partial AssetHandle _LoadCanvas([MarshalAs(UnmanagedType.LPWStr)] string name, int width, int height);

        [LibraryImport(LibName, EntryPoint = "SLT_Asset_LoadShader")]
        private static partial AssetHandle _LoadShader([MarshalAs(UnmanagedType.LPWStr)] string name, [MarshalAs(UnmanagedType.U1)] bool isFile, [MarshalAs(UnmanagedType.LPWStr)] string vs, [MarshalAs(UnmanagedType.LPWStr)] string fs);

        public static AssetHandle Load(Asset asset)
        {
            return asset switch
            {
                Asset.Image i => _LoadImage(i.name, i.path, i.linearFilter),
                Asset.Sprite s => _LoadSprite(s.name, s.path, s.spriteWidth, s.spriteHeight, s.marginX, s.marginY),
                Asset.Speech s => _LoadSpeech(s.name, s.text),
                Asset.Sound s => _LoadSound(s.name, s.path),
                Asset.Mod m => _LoadMod(m.name, m.path),
                Asset.Font f => _LoadFont(f.name, f.path),
                Asset.BitmapFont f => _LoadBitmapFont(f.name, f.path, f.glyphs, f.glyphWidth, f.charSpacing, f.spaceWidth, f.lineHeight),
                Asset.Canvas c => _LoadCanvas(c.name, c.width, c.height),
                Asset.Shader s => _LoadShader(s.name, s.isFile, s.vs, s.fs),
                _ => throw new NotImplementedException(),
            };
        }

        [LibraryImport(LibName, EntryPoint = "SLT_Asset_Find")]
        public static partial AssetHandle Find([MarshalAs(UnmanagedType.LPWStr)] string name);

        [LibraryImport(LibName, EntryPoint = "SLT_Asset_ClearAll")]
        public static partial void ClearAll();

        [LibraryImport(LibName, EntryPoint = "SLT_Asset_LoadINI")]
        public static partial void LoadINI([MarshalAs(UnmanagedType.LPWStr)] string path);

        [LibraryImport(LibName, EntryPoint = "SLT_Asset_TextSize")]
        public static partial Dimensions TextSize(float w, [MarshalAs(UnmanagedType.LPWStr)] string str, int count);

        //[LibraryImport(LibName, EntryPoint = "SLT_Asset_BreakString")]
        //public static partial [MarshalAs(UnmanagedType.LPWStr)] string SLT_Asset_BreakString(int width, [MarshalAs(UnmanagedType.LPWStr)] string str);

        //[LibraryImport(LibName, EntryPoint = "SLT_Get_Img")]
        //public static partial const Image* SLT_Get_Img(AssetHandle id);
    }

    public partial class DC
    {
        private const string LibName = "slate2d";

        [LibraryImport(LibName, EntryPoint = "DC_SetColor")]
        public static partial void SetColor(byte r, byte g, byte b, byte a);

        [LibraryImport(LibName, EntryPoint = "DC_ResetTransform")]
        public static partial void ResetTransform();

        [LibraryImport(LibName, EntryPoint = "DC_Scale")]
        public static partial void Scale(float x, float y);

        [LibraryImport(LibName, EntryPoint = "DC_Rotate")]
        public static partial void Rotate(float angle);

        [LibraryImport(LibName, EntryPoint = "DC_Translate")]
        public static partial void Translate(float x, float y);

        [LibraryImport(LibName, EntryPoint = "DC_SetScissor")]
        public static partial void SetScissor(int x, int y, int w, int h);

        [LibraryImport(LibName, EntryPoint = "DC_ResetScissor")]
        public static partial void ResetScissor();

        [LibraryImport(LibName, EntryPoint = "DC_UseCanvas")]
        public static partial void UseCanvas(AssetHandle canvasId);

        [LibraryImport(LibName, EntryPoint = "DC_ResetCanvas")]
        public static partial void ResetCanvas();

        [LibraryImport(LibName, EntryPoint = "DC_UseShader")]
        public static partial void UseShader(AssetHandle shaderId);

        [LibraryImport(LibName, EntryPoint = "DC_ResetShader")]
        public static partial void ResetShader();

        [LibraryImport(LibName, EntryPoint = "DC_DrawRect")]
        public static partial void Rect(float x, float y, float w, float h, [MarshalAs(UnmanagedType.U1)] bool outline);

        [LibraryImport(LibName, EntryPoint = "DC_SetTextStyle")]
        public static partial void SetTextStyle(AssetHandle fontId, float size, float lineHeight, int align);

        [LibraryImport(LibName, EntryPoint = "DC_DrawText")]
        public static partial void Text(float x, float y, float h, [MarshalAs(UnmanagedType.LPWStr)] string text, int len);

        [LibraryImport(LibName, EntryPoint = "DC_DrawImage")]
        public static partial void Image(uint imgId, float x, float y, float w, float h, float scale, byte flipBits, float ox, float oy);

        [LibraryImport(LibName, EntryPoint = "DC_DrawSprite")]
        public static partial void Sprite(uint spriteId, int id, float x, float y, float scale, byte flipBits, int w, int h);

        [LibraryImport(LibName, EntryPoint = "DC_DrawLine")]
        public static partial void Line(float x1, float y1, float x2, float y2);

        [LibraryImport(LibName, EntryPoint = "DC_DrawCircle")]
        public static partial void Circle(float x, float y, float radius, [MarshalAs(UnmanagedType.U1)] bool outline);

        [LibraryImport(LibName, EntryPoint = "DC_DrawTri")]
        public static partial void Tri(float x1, float y1, float x2, float y2, float x3, float y3, [MarshalAs(UnmanagedType.U1)] bool outline);

        //[LibraryImport(LibName, EntryPoint = "DC_DrawTilemap")]
        //public static partial void DrawTilemap(uint sprId, int x, int y, int w, int h, int* tiles);

        [LibraryImport(LibName, EntryPoint = "DC_Submit")]
        public static partial void Submit();

        [LibraryImport(LibName, EntryPoint = "DC_Clear")]
        public static partial void Clear(byte r, byte g, byte b, byte a);
    }
}
