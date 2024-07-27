// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#ifndef MATERIALMANAGER_H
#define MATERIALMANAGER_H

#include <irrlicht.h>

class CMaterialCaveMultiColor;
class CMaterialCaveSingleColor;
class CMaterialCorridor;
class CMaterialDunGen;

// Namespace DunGen : DungeonGenerator
namespace DunGen
{
	class CMaterialProvider
	{
	public:
		/// constructor
		CMaterialProvider(irr::IrrlichtDevice* irrDevice_);

		/// destructor
		~CMaterialProvider();

		/// returns the material of the specified type
		irr::video::E_MATERIAL_TYPE GetMaterial(SMaterialType::Enum type_);

		/// set the cave color for the single color material
		void SetCaveColor(const irr::video::SColorf& color_);

		/// set material
		void SetMaterial(SMaterialType::Enum type_, CMaterialDunGen* material_);

	private:
		CMaterialCaveMultiColor* MaterialCaveMultiColor;				///< the cave multicolor material
		CMaterialCaveSingleColor* MaterialCaveSingleColor;				///< the cave single color material
		CMaterialDunGen* MaterialCaveCustom;							///< custom cave material
		CMaterialCorridor* MaterialCorridorDefault;						///< the corridor material
		CMaterialDunGen* MaterialCorridorCustom;						///< custom corridor material
	};

} // END NAMESPACE DunGen

#endif