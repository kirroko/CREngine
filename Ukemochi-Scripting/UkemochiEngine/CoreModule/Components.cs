using System;

namespace UkemochiEngine
{
    namespace CoreModule
    {
        public struct Transform
        {
            public Vector2 position { get; set; }

            public Vector2 rotation { get; set; }

            public Vector2 scale { get; set; }

            public Transform(Vector2 position, Vector2 rotation, Vector2 scale)
            {
                this.position = position;
                this.rotation = rotation;
                this.scale = scale;
            }
        }
    }
}
