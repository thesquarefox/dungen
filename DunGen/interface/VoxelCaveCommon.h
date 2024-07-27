// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#ifndef VOXELCAVECOMMON_H
#define VOXELCAVECOMMON_H

namespace DunGen
{
	/// Parameters for the voxelspace.
	struct SVoxelSpace
	{
		/// X dimension of the Voxel space.
		static const unsigned int DimX = 512;
		/// Y dimension of the Voxel space.
		static const unsigned int DimY = 512;
		/// Z dimension of the Voxel space.
		static const unsigned int DimZ = 512;

		/// Minimal distance of 1-voxels to the border of the voxel space.
		///
		/// This distance has to be 3 at least!
		/// The resulting 0-voxel-border allows it, to not have to check for special cases at the borders of the voxel space.
		/// This results in a huge speedup, because fewer branching is required.
		static const unsigned int MinBorder = 3;
	};	
}

#endif