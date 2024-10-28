using System;
using System.Runtime.InteropServices;


namespace UkemochiEngine
{
    namespace CoreModule
    {
        public class Component : Object
        {
            [DllImport("__Internal")]
            private static extern IntPtr GetTransform(int entityID);

            public GameObject gameobject;
            public string tag;
        }
    }
}
