/* Start Header
 *****************************************************************/
/*!
\file	Object.cs
\par	Ukemochi
\author WONG JUN YU, Kean, junyukean.wong, 2301234
\par	junyukean.wong\@digipen.edu
\par	Course: CSD2400/CSD2401
\date	29/10/24
\brief	This files is responsible for the base class for all objects in the engine

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
 *******************************************************************/
namespace Ukemochi
{
    public abstract class Object
    {
        // ==================== FIELDS ========================
        internal ulong _id = 0;
        public string name;
        
        // ==================== METHODS =======================

        protected ulong GetInstanceID()
        {
            return _id;
        }

        public override string ToString()
        {
            return name;
        }
    }
}