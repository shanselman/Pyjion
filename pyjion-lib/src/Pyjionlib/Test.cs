using System;
using System.Runtime.InteropServices;

namespace Pyjion
{
    public static class Test
    {
        [StructLayout(LayoutKind.Sequential)]
        public struct LibArgs
        {
            public IntPtr Message;
            public int Number;
        }

        public static int Hello(IntPtr arg, int argLength)
        {
            if (argLength < System.Runtime.InteropServices.Marshal.SizeOf(typeof(LibArgs)))
            {
                return System.Runtime.InteropServices.Marshal.SizeOf(typeof(LibArgs));
            }

            LibArgs libArgs = Marshal.PtrToStructure<LibArgs>(arg);
            Console.WriteLine($"Hello from .NET, I am {nameof(Test)}");
            string message = RuntimeInformation.IsOSPlatform(OSPlatform.Windows)
                ? Marshal.PtrToStringUni(libArgs.Message)
                : Marshal.PtrToStringUTF8(libArgs.Message);
            Console.WriteLine($"You said : '{message}'");
            return 0;
        }
    }
}
