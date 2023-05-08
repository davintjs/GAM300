/*!***************************************************************************************
\file			uuid.h
\project
\author			Matthew Lau

\par			Course: GAM200
\par			Section:
\date			06/01/2023

\brief
	The declaration of UUID struct is contained in this file.
	The UUID struct's purpose is to generate unique universal ids for general use across Copium Engine.

All content � 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/
#ifndef UUID_H
#define UUID_H

using UUID = size_t;

UUID CreateUUID();

#endif // !UUID_H

