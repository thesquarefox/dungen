// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#ifndef LSYSTEMCOMMON_H
#define LSYSTEMCOMMON_H

namespace DunGen
{
	/// Parameters of a L-system.
	struct ELSystemParameter
	{	
		enum Enum
		{
			ANGLE_YAW			= 0,	///< The yaw angle.
			ANGLE_PITCH			= 1,	///< The pitch angle.
			ANGLE_ROLL			= 2,	///< The roll angle.
			RADIUS_START		= 3,	///< The start radius in voxels.
			RADIUS_FACTOR		= 4,	///< The factor, with which the radius is multiplied, when decreasing the radius, has to be in intervall [0,1].
			RADIUS_DECREMENT	= 5		///< The constant in voxels, which is substracted from the radius, when descreasing the radius (applies after multiplying with RADIUS_FACTOR).
		};
	};
}

#endif