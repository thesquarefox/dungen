// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#ifndef ARCHITECTCOMMON_H
#define ARCHITECTCOMMON_H

namespace DunGen
{
	/// Directions for docking corridors on the cave. Tells the direction from the view of the corridor towards the cave.
	struct EDirection
	{	
		enum Enum
		{
			X_POSITIVE			= 0,	///< Positive X direction.
			X_NEGATIVE			= 1,	///< Negative X direction.
			Z_POSITIVE			= 2,	///< Positive Z direction.
			Z_NEGATIVE			= 3,	///< Negative Z direction.
		};
	};
}

#endif