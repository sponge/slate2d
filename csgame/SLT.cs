global using AssetHandle = System.UInt32;
global using SoundPlayHandle = System.UInt32;
using System;
using System.Reflection.PortableExecutable;
using System.Runtime.InteropServices;
using static System.Net.Mime.MediaTypeNames;

namespace Slate2D
{
    public struct Position
    {
        public int x, y;
    }

    public struct Dimensions
    {
        public int w, h;
    }

    public partial class SLT
    {
        const string LibName = "slate2d";

        [LibraryImport(LibName, EntryPoint = "SLT_Init", StringMarshalling = StringMarshalling.Utf8)]
        static partial void _Init(int argc, [MarshalAs(UnmanagedType.LPArray)] string[] argv);

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

        [LibraryImport(LibName, EntryPoint = "SLT_SetWindowTitle", StringMarshalling = StringMarshalling.Utf8)]
        public static partial void SetWindowTitle(string title);

        [LibraryImport(LibName, EntryPoint = "SLT_GetResolution")]
        public static partial Dimensions GetResolution();

    }

    public partial class SLTCon
    {
        const string LibName = "slate2d";

        [LibraryImport(LibName, EntryPoint = "SLT_Print", StringMarshalling = StringMarshalling.Utf8)]
        static partial void _Print(string fmt, string arg1);

        public static void Print(string str)
        {
            _Print("%s", str);
        }

        [LibraryImport(LibName, EntryPoint = "SLT_SendConsoleCommand", StringMarshalling = StringMarshalling.Utf8)]
        public static partial void SendConsoleCommand(string text);

        // SLT_API void SLT_Con_SetErrorHandler(void(*errHandler)(int level, const char *msg));

        // SLT_API void SLT_Con_SetDefaultCommandHandler(bool(*cmdHandler)());

        [LibraryImport(LibName, EntryPoint = "SLT_Error", StringMarshalling = StringMarshalling.Utf8)]
        public static partial void Error(int level, string error);

        // SLT_API const conVar_t* SLT_Con_GetVarDefault(const char* var_name, const char* var_value, int flags);

        // SLT_API const conVar_t* SLT_Con_GetVar(const char* name);

        // SLT_API const conVar_t* SLT_Con_SetVar(const char* var_name, const char* value);

        [LibraryImport(LibName, EntryPoint = "SLT_Con_GetArgCount")]
        public static partial int GetArgCount();

        [LibraryImport(LibName, EntryPoint = "SLT_Con_GetArg", StringMarshalling = StringMarshalling.Utf8)]
        public static partial string GetArg(int arg);

        [LibraryImport(LibName, EntryPoint = "SLT_Con_GetArgs", StringMarshalling = StringMarshalling.Utf8)]
        public static partial string GetArgs(int start);

        // SLT_API void SLT_Con_AddCommand(const char* name, conCmd_t cmd);
    }

    public partial class FS
    {
        const string LibName = "slate2d";

        // SLT_API int SLT_FS_ReadFile(const char* path, void** buffer);

        // SLT_API int SLT_FS_WriteFile(const char* filename, const void* data, int len);

        [LibraryImport(LibName, EntryPoint = "SLT_FS_RealDir", StringMarshalling = StringMarshalling.Utf8)]
        public static partial string RealDir(string path);

        [LibraryImport(LibName, EntryPoint = "SLT_FS_Exists", StringMarshalling = StringMarshalling.Utf8)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static partial bool Exists(string file);

        // SLT_API char** SLT_FS_List(const char* path);

        // SLT_API void SLT_FS_FreeList(void* listVar);
    }

    public struct AnalogAxes
    {
        public float leftX, leftY;
        public float rightX, rightY;
        public float triggerLeft, triggerRight;
    }

    public enum Buttons
    {
        Up,
        Down,
        Left,
        Right,
        A,
        B,
        X,
        Y,
        L,
        R,
        Start,
        Select
    }

    public partial class Input
    {
        const string LibName = "slate2d";

        // static partial void SLT_In_AllocateButtons(const char** buttonNames, int buttonCount);

        // static partial const buttonState_t* SLT_In_GetButton(int buttonNum);

        [LibraryImport(LibName, EntryPoint = "SLT_In_ButtonPressed")]
        [return: MarshalAs(UnmanagedType.I1)]
        public static partial bool ButtonPressed(int buttonNum, uint delay = 0, int repeat = 0);

        [LibraryImport(LibName, EntryPoint = "SLT_In_MousePosition")]
        public static partial Position MousePosition();

        [LibraryImport(LibName, EntryPoint = "SLT_In_ControllerAnalog")]
        public static partial AnalogAxes ControllerAnalog(int controllerNum);
    }

    public partial class Assets
    {
        const string LibName = "slate2d";

        public record AssetConfig
        {
            public record Image(string name, string path, bool linearFilter = false) : AssetConfig();
            public record Sprite(string name, string path, int spriteWidth, int spriteHeight, int marginX, int marginY) : AssetConfig();
            public record Speech(string name, string text) : AssetConfig();
            public record Sound(string name, string path) : AssetConfig();
            public record Mod(string name, string path) : AssetConfig();
            public record Font(string name, string path) : AssetConfig();
            public record BitmapFont(string name, string path, string glyphs, int glyphWidth, int charSpacing, int spaceWidth, int lineHeight) : AssetConfig();
            public record Canvas(string name, int width, int height) : AssetConfig();
            public record Shader(string name, bool isFile, string vs, string fs) : AssetConfig();

            AssetConfig() { }
        }

        [LibraryImport(LibName, EntryPoint = "SLT_Asset_LoadImage", StringMarshalling = StringMarshalling.Utf8)]
        private static partial AssetHandle _LoadImage(string name, string path, [MarshalAs(UnmanagedType.U1)] bool linearFilter);

        [LibraryImport(LibName, EntryPoint = "SLT_Asset_LoadSprite", StringMarshalling = StringMarshalling.Utf8)]
        private static partial AssetHandle _LoadSprite(string name, string path, int spriteWidth, int spriteHeight, int marginX, int marginY);

        [LibraryImport(LibName, EntryPoint = "SLT_Asset_LoadSpeech", StringMarshalling = StringMarshalling.Utf8)]
        private static partial AssetHandle _LoadSpeech(string name, string text);

        [LibraryImport(LibName, EntryPoint = "SLT_Asset_LoadSound", StringMarshalling = StringMarshalling.Utf8)]
        private static partial AssetHandle _LoadSound(string name, string path);

        [LibraryImport(LibName, EntryPoint = "SLT_Asset_LoadMod", StringMarshalling = StringMarshalling.Utf8)]
        private static partial AssetHandle _LoadMod(string name, string path);

        [LibraryImport(LibName, EntryPoint = "SLT_Asset_LoadFont", StringMarshalling = StringMarshalling.Utf8)]
        private static partial AssetHandle _LoadFont(string name, string path);

        [LibraryImport(LibName, EntryPoint = "SLT_Asset_LoadBitmapFont", StringMarshalling = StringMarshalling.Utf8)]
        private static partial AssetHandle _LoadBitmapFont(string name, string path, string glyphs, int glyphWidth, int charSpacing, int spaceWidth, int lineHeight);

        [LibraryImport(LibName, EntryPoint = "SLT_Asset_LoadCanvas", StringMarshalling = StringMarshalling.Utf8)]
        private static partial AssetHandle _LoadCanvas(string name, int width, int height);

        [LibraryImport(LibName, EntryPoint = "SLT_Asset_LoadShader", StringMarshalling = StringMarshalling.Utf8)]
        private static partial AssetHandle _LoadShader(string name, [MarshalAs(UnmanagedType.U1)] bool isFile, string vs, string fs);

        public static AssetHandle Load(AssetConfig asset)
        {
            return asset switch
            {
                AssetConfig.Image i => _LoadImage(i.name, i.path, i.linearFilter),
                AssetConfig.Sprite s => _LoadSprite(s.name, s.path, s.spriteWidth, s.spriteHeight, s.marginX, s.marginY),
                AssetConfig.Speech s => _LoadSpeech(s.name, s.text),
                AssetConfig.Sound s => _LoadSound(s.name, s.path),
                AssetConfig.Mod m => _LoadMod(m.name, m.path),
                AssetConfig.Font f => _LoadFont(f.name, f.path),
                AssetConfig.BitmapFont f => _LoadBitmapFont(f.name, f.path, f.glyphs, f.glyphWidth, f.charSpacing, f.spaceWidth, f.lineHeight),
                AssetConfig.Canvas c => _LoadCanvas(c.name, c.width, c.height),
                AssetConfig.Shader s => _LoadShader(s.name, s.isFile, s.vs, s.fs),
                _ => throw new NotImplementedException(),
            };
        }

        [LibraryImport(LibName, EntryPoint = "SLT_Asset_Find", StringMarshalling = StringMarshalling.Utf8)]
        public static partial AssetHandle Find(string name);

        [LibraryImport(LibName, EntryPoint = "SLT_Asset_ClearAll")]
        public static partial void ClearAll();

        [LibraryImport(LibName, EntryPoint = "SLT_Asset_LoadINI", StringMarshalling = StringMarshalling.Utf8)]
        public static partial void LoadINI(string path);

        [LibraryImport(LibName, EntryPoint = "SLT_Asset_TextSize", StringMarshalling = StringMarshalling.Utf8)]
        public static partial Dimensions TextSize(float w, string str, int count);

        [LibraryImport(LibName, EntryPoint = "SLT_Asset_BreakString", StringMarshalling = StringMarshalling.Utf8)]
        public static partial string BreakString(int width, string str);

        [LibraryImport(LibName, EntryPoint = "SLT_Get_ImgSize")]
        public static partial Dimensions ImageSize(AssetHandle id);

        [LibraryImport(LibName, EntryPoint = "SLT_Snd_Play")]
        public static partial SoundPlayHandle SndPlay(AssetHandle asset, float volume, float pan, [MarshalAs(UnmanagedType.U1)] bool loop);

        [LibraryImport(LibName, EntryPoint = "SLT_Snd_Stop")]
        public static partial void SndStop(SoundPlayHandle handle);

        [LibraryImport(LibName, EntryPoint = "SLT_Snd_PauseResume")]
        public static partial void SndPauseResume(uint handle, [MarshalAs(UnmanagedType.U1)] bool pause);
    }

    public partial class DC
    {
        const string LibName = "slate2d";

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

        [LibraryImport(LibName, EntryPoint = "DC_DrawText", StringMarshalling = StringMarshalling.Utf8)]
        public static partial void Text(float x, float y, float h, string text, int len);

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

        [LibraryImport(LibName, EntryPoint = "DC_DrawTilemap")]
        public static partial void Tilemap(AssetHandle sprId, int x, int y, int w, int h, int[] tiles);

        [LibraryImport(LibName, EntryPoint = "DC_Submit")]
        public static partial void Submit();

        [LibraryImport(LibName, EntryPoint = "DC_Clear")]
        public static partial void Clear(byte r, byte g, byte b, byte a);
    }
}
