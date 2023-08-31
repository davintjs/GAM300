/*!***************************************************************************************
\file			editor-layers.h
\project
\author			Sean Ngo
\co-author		Matthew Lau

\par			Course: GAM250
\par			Section:
\date			13/01/2023

\brief
	This file holds the declarations of the functions in the editor layers gui

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/
#ifndef EDITOR_LAYERS_H
#define EDITOR_LAYERS_H

#include "Utilities/sorting-layers.h"
#include "Utilities/layers.h"

namespace Copium
{
	class EditorLayers
	{
	public:
		/***************************************************************************/
		/*!
		\brief
			Initializes the editor layering system
		*/
		/***************************************************************************/
		void init();

		/***************************************************************************/
		/*!
		\brief
			Updates the editor layering system
		*/
		/***************************************************************************/
		void update();

		/***************************************************************************/
		/*!
		\brief
			Exits the editor layering system
		*/
		/***************************************************************************/
		void exit();

		/***************************************************************************/
		/*!
		\brief
			Get the SortingLayers struct

		\return
			pointer to the SortingLayers struct
		*/
		/***************************************************************************/
		SortingLayers* SortLayers() { return &sortingLayers; }

		/***************************************************************************/
		/*!
		\brief
			Get the Layering struct

		\return
			pointer to the Layering struct
		*/
		/***************************************************************************/
		Layering* Layers() { return &layers; }

	private:
		/* Layer Data *******************************************************************/
		SortingLayers sortingLayers;
		Layering layers;

		
	};
}

#endif // !EDITOR_LAYERS_H

