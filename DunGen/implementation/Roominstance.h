// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#ifndef ROOMINSTANCE_H
#define ROOMINSTANCE_H

#include "DockingSite.h"
#include "Roompattern.h"
#include <irrlicht.h>
#include <vector>

// Namespace DunGen : DungeonGenerator
namespace DunGen
{
	/// concrete parameters for a room in the dungeon
	struct SRoomInstance
	{
		/// position
		irr::core::vector3d<double> Position;
		/// rotation
		irr::core::vector3d<double> Rotation;
		/// scaling
		irr::core::vector3d<double> ScaleFactor;
		
		/// the room pattern whereupon the room is derivated
		CRoomPattern* RoomPattern;

		/// the resulting docking sites of the room
		std::vector<SDockingSite> DockingSite;
	};
}
#endif