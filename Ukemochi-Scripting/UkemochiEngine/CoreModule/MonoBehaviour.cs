using System;
using System.Security;

namespace UkemochiEngine
{
    namespace CoreModule
    { 
        public class MonoBehaviour
        {
            public bool enabled { get; set; }
            public bool isActiveAndEnabled { get; }

            protected void Start() { }

            protected void Update() { }

            protected void OnDestroy() { }
        }
    }

}
