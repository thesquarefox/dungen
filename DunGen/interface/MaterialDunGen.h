// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#ifndef MATERIAL_H
#define MATERIAL_H

#include <irrlicht.h>

namespace DunGen
{
	/// available Materials
	struct SMaterialType
	{
		enum Enum
		{
			CAVE_SINGLECOLOR,	///< the cave multicolor material
			CAVE_MULTICOLOR,	///< the cave single color material
			CAVE_CUSTOM,		///< custom cave material
			CORRIDOR_DEFAULT,	///< the default corridor material
			CORRIDOR_CUSTOM		///< custom corridor material
		};
	};

	class CMaterialDunGen : public irr::video::IShaderConstantSetCallBack
	{
	public:
		/// read the material
		inline irr::video::E_MATERIAL_TYPE GetMaterial() const
		{
			return Material;
		}

	protected:
		irr::video::E_MATERIAL_TYPE Material;	///< the material
	};
}

#endif
