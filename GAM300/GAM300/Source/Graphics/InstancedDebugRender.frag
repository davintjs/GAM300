/**************************************************************************************/
/*!
//    \file			InstancedDebugRender.frag
//    \author(s) 	Euphrasia Theophelia Tan Ee Mun
//
//    \date   	    15th September 2023
//    \brief		This file is the frag file for drawing debug lines.
//
//    \Percentage   Theophelia 100%
//
//    Copyright (C) 2022 DigiPen Institute of Technology.
//    Reproduction or disclosure of this file or its contents without the
//    prior written consent of DigiPen Institute of Technology is prohibited.
*/
/**************************************************************************************/

#version 450 core

uniform vec3 uColor;

out vec4 FragColor;

void main()
{
	FragColor = vec4(uColor, 1.f);
}