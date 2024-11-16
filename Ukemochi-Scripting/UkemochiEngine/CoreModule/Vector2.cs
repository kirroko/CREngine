/* Start Header
 *****************************************************************/
/*!
\file	Vector2.cs
\par	Ukemochi
\author WONG JUN YU, Kean, junyukean.wong, 2301234
\par	junyukean.wong\@digipen.edu
\par	Course: CSD2400/CSD2401
\date	31/09/24
\brief	This files is responsible for the Vector2 struct that stores 2D coordinates
        for the engine

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
 *******************************************************************/

using System;

namespace Ukemochi
{
    public struct Vector2 : IEquatable<Vector2>
    {
        // ==================== Properties ====================
        // public float x;
        // public float y;
        public static Vector2 zero => new Vector2(0.0f);
        public static Vector2 one => new Vector2(1.0f);
        public static Vector2 right => new Vector2(1.0f, 0.0f);
        public static Vector2 left => new Vector2(-1.0f, 0.0f);
        public static Vector2 up => new Vector2(0.0f, 1.0f);
        public static Vector2 down => new Vector2(0.0f, -1.0f);
        
        
        public float x { get; set; }
        public float y { get; set; }
        
        public float magnitude => (float)Math.Sqrt(x * x + y * y);
        public float sqrMagnitude => x * x + y * y;
        public Vector2 normalized => this / magnitude;
        

        // ==================== METHODS/CONSTRUCTORS ====================
        public Vector2(float value)
        {
            x = value;
            y = value;
        }

        public Vector2(float x, float y)
        {
            this.x = x;
            this.y = y;
        }

        public override string ToString()
        {
            return $"({x}, {y})";
        }
        
        public bool Equals(Vector2 other)
        {
            return this == other;
        }

        public void Normalize()
        {
            float mag = magnitude;
            x /= mag;
            y /= mag;
        }

        public void Set(float newX, float newY)
        {
            x = newX;
            y = newY;
        }

        public override bool Equals(object obj)
        {
            return obj is Vector2 other && Equals(other);
        }
        
        public override int GetHashCode()
        {
            return x.GetHashCode() ^ y.GetHashCode() << 2;
        }

        // ==================== STATIC METHODS ====================
        public static float Dot(Vector2 a, Vector2 b)
        {
            return a.x * b.x + a.y * b.y;
        }
        
        public static float Distance(Vector2 a, Vector2 b)
        {
            return (a - b).magnitude;
        }
        
        public static Vector2 Lerp(Vector2 a, Vector2 b, float t)
        {
            return a + (b - a) * t;
        }
        
        public static Vector2 MoveTowards(Vector2 current, Vector2 target, float maxDistanceDelta)
        {
            Vector2 a = target - current;
            float magnitude = a.magnitude;
            if (magnitude <= maxDistanceDelta || magnitude == 0.0f)
            {
                return target;
            }
            return current + a / magnitude * maxDistanceDelta;
        }
        
        public static Vector2 Scale(Vector2 a, Vector2 b)
        {
            return new Vector2(a.x * b.x, a.y * b.y);
        }
        
        public static Vector2 Reflect(Vector2 inDirection, Vector2 inNormal)
        {
            return inDirection - 2.0f * Dot(inNormal, inDirection) * inNormal;
        }
        
        public static Vector2 Perpendicular(Vector2 inDirection)
        {
            return new Vector2(-inDirection.y, inDirection.x);
        }
        
        public static Vector2 ClampMagnitude(Vector2 vector, float maxLength)
        {
            if (vector.sqrMagnitude > maxLength * maxLength)
            {
                return vector.normalized * maxLength;
            }
            return vector;
        }
        
        public static float Angle(Vector2 from, Vector2 to)
        {
            return (float)Math.Acos(Clamp(Dot(from.normalized, to.normalized), -1.0f, 1.0f)) * 57.29578f;
        }
        
        public static float SignedAngle(Vector2 from, Vector2 to)
        {
            float num = Angle(from, to);
            float num2 = from.x * to.y - from.y * to.x;
            return num * Math.Sign(num2);
        }
        
        public static Vector2 Min(Vector2 lhs, Vector2 rhs)
        {
            return new Vector2(Math.Min(lhs.x, rhs.x), Math.Min(lhs.y, rhs.y));
        }
        
        public static Vector2 Max(Vector2 lhs, Vector2 rhs)
        {
            return new Vector2(Math.Max(lhs.x, rhs.x), Math.Max(lhs.y, rhs.y));
        }

        private static float Clamp(float value, float min, float max)
        {
            // If value is less or equal to max, check if value is less than min, if so return min, else return value, else return max
            return value <= max ? value < min ? min : value : max;
        }

        // ==================== OPERATORS ====================
        public static Vector2 operator +(Vector2 a, Vector2 b)
        {
            return new Vector2(a.x + b.x, a.y + b.y);
        }

        public static Vector2 operator -(Vector2 a, Vector2 b)
        {
            return new Vector2(a.x - b.x, a.y - b.y);
        }

        public static Vector2 operator *(Vector2 a, float b)
        {
            return new Vector2(a.x * b, a.y * b);
        }

        public static Vector2 operator *(float a, Vector2 b)
        {
            return new Vector2(b.x * a, b.y * a);
        }

        public static Vector2 operator /(Vector2 a, float b)
        {
            return new Vector2(a.x / b, a.y / b);
        }

        public static bool operator ==(Vector2 lhs, Vector2 rhs)
        {
            return (lhs - rhs).magnitude < 1e-5f;
        }
        
        public static bool operator !=(Vector2 lhs, Vector2 rhs)
        {
            return !(lhs == rhs);
        }
    }
}