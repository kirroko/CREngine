using System;
using System.Reflection;
using System.Runtime.CompilerServices;

namespace Ukemochi
{
    public class CSharpTest
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void LogMessage(string message);

        public float floatVar = 5.0f;

        public void Start()
        {
            Console.WriteLine("is it working?");
            Assembly asm = typeof(Ukemochi.CSharpTest).Assembly;
            Console.WriteLine("Assembly: " + asm.FullName);
            try
            {
                LogMessage("Hello World C#");
            }
            catch (Exception e)
            {
                Console.WriteLine($"Internal call failed: {e.Message}");
            }
        }

        public void PrintFloatVar()
        {
            Console.WriteLine("FloatVar = {0:f}", floatVar);
        }

        private void IncrementFloatVar(float val)
        {
            floatVar += val;
        }
    }
}