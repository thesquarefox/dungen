// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#ifndef ARCHITECT_H
#define ARCHITECT_H

#include "interface/ArchitectCommon.h"
#include <irrlicht.h>

// Namespace DunGen : DungeonGenerator
namespace DunGen
{
	class CMeshCave;
	class CVoxelCave;
	struct SDockingSite;

	class CArchitect
	{	
	public:
		// constructor
		CArchitect(CVoxelCave* _voxelCave, CMeshCave* _meshCave);
		// destructor
		~CArchitect();

		/// creates a docking site at a cave region
		bool CreateDockingSite(SDockingSite &dockingSite_, 
			const irr::core::vector3d<unsigned int>& minVox_, const irr::core::vector3d<unsigned int>& maxVox_,
			EDirection::Enum direction_);
	
	private:
		/// the voxel cave
		CVoxelCave* VoxelCave;
		/// the mesh cave
		CMeshCave* MeshCave;

		/// "infinite"
		static const unsigned int Infinite = 99999;
	};
};

#endif