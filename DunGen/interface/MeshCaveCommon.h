// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#ifndef MESHCAVECOMMON_H
#define MESHCAVECOMMON_H

namespace DunGen
{
	/// Methods for normal weighting: how are the triangle normals weighted when combining to vertex normals.
	struct ENormalWeightMethod
	{
		enum Enum
		{
			BY_AREA		= 0,	///< Triangle normals are weighted with their area.
			BY_ANGLE	= 1,	///< Triangle normals are weighted with the angles they have in each vertex they belong to.
			UNIFORM		= 2		///< Triangle normals are weighted with 1.
		};
	};
}

#endif