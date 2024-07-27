// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#include "interface/MaterialDunGen.h"
#include "MaterialCaveMultiColor.h"
#include "MaterialCaveSingleColor.h"
#include "MaterialCorridor.h"
#include "MaterialProvider.h"

DunGen::CMaterialProvider::CMaterialProvider(irr::IrrlichtDevice* irrDevice_)
: MaterialCaveMultiColor(new CMaterialCaveMultiColor(irrDevice_))
, MaterialCaveSingleColor(new CMaterialCaveSingleColor(irrDevice_))
, MaterialCorridorDefault(new CMaterialCorridor(irrDevice_))
{
}

DunGen::CMaterialProvider::~CMaterialProvider()
{
	delete MaterialCorridorDefault;
	delete MaterialCaveSingleColor;
	delete MaterialCaveMultiColor;
}

irr::video::E_MATERIAL_TYPE DunGen::CMaterialProvider::GetMaterial( SMaterialType::Enum type_ )
{
	switch (type_)
	{
	case SMaterialType::CAVE_SINGLECOLOR:
		return MaterialCaveSingleColor->GetMaterial();
	case SMaterialType::CAVE_MULTICOLOR:
		return MaterialCaveMultiColor->GetMaterial();
	case SMaterialType::CAVE_CUSTOM:
		return MaterialCaveCustom->GetMaterial();
	case SMaterialType::CORRIDOR_CUSTOM:
		return MaterialCorridorCustom->GetMaterial();
	default:
		return MaterialCorridorDefault->GetMaterial();
	}
}

void DunGen::CMaterialProvider::SetCaveColor(const irr::video::SColorf& color_)
{
	MaterialCaveSingleColor->SetColor(color_);
}

void DunGen::CMaterialProvider::SetMaterial( SMaterialType::Enum type_, CMaterialDunGen* material_ )
{
	switch (type_)
	{
	case SMaterialType::CAVE_CUSTOM:
		MaterialCaveCustom = material_;
	case SMaterialType::CORRIDOR_CUSTOM:
		MaterialCorridorCustom = material_;
	}
}