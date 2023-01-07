using System.Reflection.PortableExecutable;
using System.Runtime.InteropServices;

namespace Slate2D
{
    public partial class SLT
    {
        private const string LibName = "slate2d.dll";

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
    }

    public partial class DC
    {
        private const string LibName = "slate2d.dll";

        [LibraryImport(LibName, EntryPoint = "DC_Submit")]
        public static partial void Submit();

        [LibraryImport(LibName, EntryPoint = "DC_Clear")]
        public static partial void Clear(byte r, byte g, byte b, byte a);

        [LibraryImport(LibName, EntryPoint = "DC_DrawRect")]
        public static partial void DrawRect(float x, float y, float w, float h, [MarshalAs(UnmanagedType.U1)] bool outline);
    }
}
