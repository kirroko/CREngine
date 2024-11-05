/* Start Header
 *****************************************************************/
/*!
\file	Rigidbody2D.cs
\par	Ukemochi
\author WONG JUN YU, Kean, junyukean.wong, 2301234
\par	junyukean.wong\@digipen.edu
\par	Course: CSD2400/CSD2401
\date	04/11/24
\brief	This files is responsible for the Rigidbody2D class that store the physics properties of the object in the engine

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
 *******************************************************************/
namespace UkemochiEngine.CoreModule
{
    public class Rigidbody2D : Component
    {
        // ==================== PROPERTIES ====================
        public Vector2 Velocity { get; set; }
        public Vector2 Acceleration { get; set; }
        public Vector2 Force { get; set; } 
        
        public float Mass { get; set; } = 1.0f;
        public float InverseMass { get; set; } = 1.0f;
        public float Drag { get; set; } = 0.9f;
        public float Angle { get; set; } = 0.0f;
        public float AngularVelocity { get; set; } = 0.0f;
        public float Torque { get; set; } = 0.0f;
        public float InertiaMass { get; set; } = 1.0f;
        public float InverseInertiaMass { get; set; } = 1.0f; // 1 / InertiaMass
        public float AngularDrag { get; set; } = 0.9f;
        
        public bool UseGravity { get; set; } = false;
        public bool IsKinematic { get; set; } = false;

        // ==================== METHODS =======================
        public void AddForce(Vector2 force)
        {
            Force = force;
        }

        public void AddForceX(float force)
        {
            Force = new Vector2(force, Force.y);
        }

        public void AddForceY(float force)
        {
            Force = new Vector2(Force.x, force);
        }

        public void RemoveForceX()
        {
            Force = new Vector2(0, Force.y);
            Velocity = new Vector2( Velocity.x * Drag, Velocity.y);
            if(System.Math.Abs(Velocity.x) < 0.01f)
                Velocity = new Vector2(0, Velocity.y);
        }

        public void RemoveForceY()
        {
            Force = new Vector2(Force.x, 0);
            Velocity = new Vector2(Velocity.x, Velocity.y * Drag);
            if(System.Math.Abs(Velocity.y) < 0.01f)
                Velocity = new Vector2(Velocity.x, 0);
        }
    }
}