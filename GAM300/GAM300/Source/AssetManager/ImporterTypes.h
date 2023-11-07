#pragma once

#include "Utilities/GUID.h"
#include <Properties.h>

struct DefaultImporter : property::base
{
	Engine::HexID guid;
	property_vtable();
};


property_begin_name(DefaultImporter, "") {
	property_var(guid).Name("guid"),
} property_vend_h(DefaultImporter)

struct ModelImporter : DefaultImporter
{
	property_vtable();
};

property_begin_name(ModelImporter, "ModelImporter") {
	property_parent(DefaultImporter),
} property_vend_h(ModelImporter)