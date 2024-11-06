/* Start Header
 *****************************************************************/
/*!
\file	EngineInterop.cs
\par	Ukemochi
\author WONG JUN YU, Kean, junyukean.wong, 2301234
\par	junyukean.wong\@digipen.edu
\par	Course: CSD2400/CSD2401
\date	29/10/24
\brief	This files is responsible for handling the interop between C# and C++ code, specifically for the engine

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
 *******************************************************************/

using System;
using System.Runtime.CompilerServices;

namespace UkemochiEngine.CoreModule
{
    public static class EngineInterop
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void AddComponent(ulong id, object component, string typeName);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void SetTransformPosition(ulong id, float x, float y);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void SetTransformRotation(ulong id, float x, float y);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void SetTransformScale(ulong id, float x, float y);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern object GetMonoObject(ulong id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern bool GetKey(int key);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern bool GetKeyDown(int key);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void LogMessage(string message);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void SetRigidbodyVelocity(ulong getInstanceId, float velocityX, float velocityY);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void SetRigidbodyAcceleration(ulong getInstanceId, float accelerationX,
            float accelerationY);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void SetRigidbodyForce(ulong getInstanceId, float forceX, float forceY);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void SetRigidbodyMass(ulong getInstanceId, float mass);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void SetRigidbodyInverseMass(ulong getInstanceId, float inverseMass);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void SetRigidbodyDrag(ulong getInstanceId, float drag);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void SetRigidbodyAngle(ulong getInstanceId, float angle);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void SetRigidbodyAngularVelocity(ulong getInstanceId, float angularVelocity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void SetRigidbodyTorque(ulong getInstanceId, float torque);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void SetRigidbodyInertiaMass(ulong getInstanceId, float inertiaMass);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void SetRigidbodyInverseInertiaMass(ulong getInstanceId, float inverseInertiaMass);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void SetRigidbodyAngularDrag(ulong getInstanceId, float angularDrag);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void SetRigidbodyUseGravity(ulong getInstanceId, bool useGravity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void SetRigidbodyIsKinematic(ulong getInstanceId, bool isKinematic);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void SetSpriteRenderFlipX(ulong getInstanceId, bool isFacingRight);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void SetSpriteRenderFlipY(ulong getInstanceId, bool flipY);
    }
}