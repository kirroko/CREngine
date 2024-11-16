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

namespace Ukemochi
{
    public class Rigidbody2D : Component
    {
        // ==================== FIELDS ========================
        private float _mass = 1.0f;
        private float _inverseMass = 1.0f;
        private float _drag = 0.9f;
        private float _angle;
        private float _angularVelocity;
        private float _torque;
        private float _inertiaMass = 1.0f;
        private float _inverseInertiaMass = 1.0f; // 1 / InertiaMass
        private float _angularDrag = 0.9f;

        private bool _useGravity;
        private bool _isKinematic;

        // ==================== PROPERTIES ====================

        #region Properties

        public Vector2 Velocity
        {
            get
            {
                float x = 0.0f;
                float y = 0.0f;
                EngineInterop.GetRigidbodyVelocity(GetInstanceID(), ref x, ref y);
                return new Vector2(x, y);
            }
            set => EngineInterop.SetRigidbodyVelocity(GetInstanceID(), value.x, value.y);
        }

        public Vector2 Acceleration
        {
            get
            {
                float x = 0.0f;
                float y = 0.0f;
                EngineInterop.GetRigidbodyAcceleration(GetInstanceID(), ref x, ref y);
                return new Vector2(x, y);
            }
            set => EngineInterop.SetRigidbodyAcceleration(GetInstanceID(), value.x, value.y);
        }

        public Vector2 Force
        {
            get
            {
                float x = 0.0f;
                float y = 0.0f;
                EngineInterop.GetRigidbodyForce(GetInstanceID(), ref x, ref y);
                return new Vector2(x, y);
            }
            set => EngineInterop.SetRigidbodyForce(GetInstanceID(), value.x, value.y);
        }

        public float Mass // TODO: To be refactored
        {
            get => _mass;
            set
            {
                _mass = value;
                EngineInterop.SetRigidbodyMass(GetInstanceID(), _mass);
            }
        }

        public float InverseMass
        {
            get => _inverseMass;
            set
            {
                _inverseMass = value / _mass;
                EngineInterop.SetRigidbodyInverseMass(GetInstanceID(), _inverseMass);
            }
        }

        public float Drag
        {
            get => _drag;
            set
            {
                _drag = value;
                EngineInterop.SetRigidbodyDrag(GetInstanceID(), _drag);
            }
        }

        public float Angle
        {
            get => _angle;
            set
            {
                _angle = value;
                EngineInterop.SetRigidbodyAngle(GetInstanceID(), _angle);
            }
        }

        public float AngularVelocity
        {
            get => _angularVelocity;
            set
            {
                _angularVelocity = value;
                EngineInterop.SetRigidbodyAngularVelocity(GetInstanceID(), _angularVelocity);
            }
        }

        public float Torque
        {
            get => _torque;
            set
            {
                _torque = value;
                EngineInterop.SetRigidbodyTorque(GetInstanceID(), _torque);
            }
        }

        public float InertiaMass
        {
            get => _inertiaMass;
            set
            {
                _inertiaMass = value;
                EngineInterop.SetRigidbodyInertiaMass(GetInstanceID(), _inertiaMass);
            }
        }

        public float InverseInertiaMass
        {
            get => _inverseInertiaMass;
            set
            {
                _inverseInertiaMass = value;
                EngineInterop.SetRigidbodyInverseInertiaMass(GetInstanceID(), _inverseInertiaMass);
            }
        }

        public float AngularDrag
        {
            get => _angularDrag;
            set
            {
                _angularDrag = value;
                EngineInterop.SetRigidbodyAngularDrag(GetInstanceID(), _angularDrag);
            }
        }

        public bool UseGravity
        {
            get => _useGravity;
            set
            {
                _useGravity = value;
                EngineInterop.SetRigidbodyUseGravity(GetInstanceID(), _useGravity);
            }
        }

        public bool IsKinematic
        {
            get => _isKinematic;
            set
            {
                _isKinematic = value;
                EngineInterop.SetRigidbodyIsKinematic(GetInstanceID(), _isKinematic);
            }
        }

        #endregion

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
            // _force.y = force;
        }

        public void AddTorque(float torque)
        {
            Torque = torque;
        }

        public void RemoveTorque()
        {
            Torque = 0.0f;
        }

        public void RemoveForceX()
        {
            Force = new Vector2(0, Force.y);
        }

        public void RemoveForceY()
        {
            Force = new Vector2(Force.x, 0);
        }
    }
}