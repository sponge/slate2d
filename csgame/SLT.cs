using System.Reflection.PortableExecutable;
using System.Runtime.InteropServices;

namespace Slate2D
{
    internal partial class SLT
    {
        private const string LibName = "slate2d.dll";

        [LibraryImport(LibName, EntryPoint = "SLT_Init", StringMarshalling = StringMarshalling.Utf8)]
        private static partial void _Init(int argc, [MarshalAs(UnmanagedType.LPArray)] string[] argv);
        public static void Init(string[] args)
        {
            string[] sltargs = args.Prepend(Environment.ProcessPath ?? ".").ToArray();
            _Init(sltargs.Length, sltargs);
        }
    }
}
