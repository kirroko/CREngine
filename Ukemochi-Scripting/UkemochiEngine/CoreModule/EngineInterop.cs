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

namespace Ukemochi
{
    internal static class EngineInterop
    {
        #region Objects/Components
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool HasComponent(ulong id, Type componentType);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetObjectByTag(string tag, out ulong id);
        
        #endregion

        #region Input
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetKey(int key);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetKeyDown(int key);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetMouseButton(int button);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetMouseButtonDown(int button);
        
        #endregion

        #region Logging
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void LogMessage(string message);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void LogWarning(string message);

        #endregion
        
        #region Transform

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetTransformPosition(ulong id, float x, float y);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetTransformPosition(ulong id, ref float x, ref float y);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetTransformRotation(ulong id, float x, float y); // TODO: WIP

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetTransformRotation(ulong id, ref float x, ref float y);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetTransformScale(ulong id, float x, float y);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetTransformScale(ulong id, ref float x, ref float y);

        #endregion

        #region Rigibody

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetRigidbodyVelocity(ulong getInstanceId, float velocityX, float velocityY);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetRigidbodyVelocity(ulong getInstanceId, ref float velocityX, ref float velocityY);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetRigidbodyAcceleration(ulong getInstanceId, float accelerationX,
            float accelerationY);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetRigidbodyAcceleration(ulong getInstanceId, ref float accelerationX,
            ref float accelerationY);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetRigidbodyForce(ulong getInstanceId, float forceX, float forceY);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetRigidbodyForce(ulong getInstanceId, ref float forceX, ref float forceY);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetRigidbodyMass(ulong getInstanceId, float mass);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetRigidbodyInverseMass(ulong getInstanceId, float inverseMass);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetRigidbodyDrag(ulong getInstanceId, float drag);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetRigidbodyAngle(ulong getInstanceId, float angle);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetRigidbodyAngularVelocity(ulong getInstanceId, float angularVelocity);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetRigidbodyTorque(ulong getInstanceId, float torque);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetRigidbodyInertiaMass(ulong getInstanceId, float inertiaMass);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetRigidbodyInverseInertiaMass(ulong getInstanceId, float inverseInertiaMass);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetRigidbodyAngularDrag(ulong getInstanceId, float angularDrag);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetRigidbodyUseGravity(ulong getInstanceId, bool useGravity);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetRigidbodyIsKinematic(ulong getInstanceId, bool isKinematic);

        #endregion

        #region SpriteRenderer

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetSpriteRenderFlipX(ulong id, bool isFacingRight);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetSpriteRenderFlipY(ulong id, bool flipY);

        #endregion
        
        #region Animation

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool PlayAnimation(ulong id, string clipName);

        #endregion

        #region Time

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern float GetDeltaTime();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern float GetFixedDeltaTime();

        #endregion
    }
}