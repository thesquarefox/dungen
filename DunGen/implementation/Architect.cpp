// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#include "Architect.h"
#include "DockingSite.h"
#include "MeshCave.h"
#include "VoxelCave.h"

DunGen::CArchitect::CArchitect(CVoxelCave* _voxelCave, CMeshCave* _meshCave)
	: VoxelCave(_voxelCave)
	, MeshCave(_meshCave)	
{
}

DunGen::CArchitect::~CArchitect()
{
}

bool DunGen::CArchitect::CreateDockingSite(SDockingSite &dockingSite_,
	const irr::core::vector3d<unsigned int>& minVox_, const irr::core::vector3d<unsigned int>& maxVox_,
	EDirection::Enum direction_)
{
	// empty docking site
	dockingSite_.Point.clear();
	dockingSite_.Extend = 0.5;	// because of warping
	irr::f32 tmp = 0.0f;

	// create new parameters and adjust cave
	switch (direction_)
	{
	case EDirection::X_NEGATIVE:
	case EDirection::X_POSITIVE:
		{
			// adjust cave
			int begin = Infinite;

			const unsigned int startX = (EDirection::X_POSITIVE == direction_) ? minVox_.X : maxVox_.X;
			const unsigned int endX = (EDirection::X_POSITIVE == direction_) ?
				SVoxelSpace::DimX-SVoxelSpace::MinBorder : VoxelCave->MinBorderFilter; // 3 stone voxel at border
			const int increment = (EDirection::X_POSITIVE == direction_) ? 1 : -1;
			const int depthModificator = maxVox_.X - minVox_.X + 1;

			// check for begin of voxel cave
			for (unsigned int x=startX; x!=endX && Infinite == begin; x+=increment)
				for (unsigned int y=minVox_.Y; y<=maxVox_.Y; ++y)
					for (unsigned int z=minVox_.Z; z<=maxVox_.Z; ++z)
						if (1 == VoxelCave->GetVoxel(x,y,z))
						{
							begin = x;
							y = maxVox_.Y+1;		// break outer loop
							break;
						}

			// no valid docking position?
			if (Infinite == begin)
				return false;

			// adjust docking position
			begin -= depthModificator * increment;
			// docking position has to be at least 2 voxel from border, so that digging starts 3 voxel away from border
			if (begin < static_cast<int>(VoxelCave->MinBorderFilter))
				begin = VoxelCave->MinBorderFilter;
			else if (begin > static_cast<int>(SVoxelSpace::DimX-SVoxelSpace::MinBorder))
				begin = SVoxelSpace::DimX-SVoxelSpace::MinBorder;

			// dig away the stone voxels & mark docking site
			for (unsigned int x=begin+increment; x!=endX; x+=increment)
			{
				bool done = true;
				for (unsigned int y=minVox_.Y; y<=maxVox_.Y; ++y)
					for (unsigned int z=minVox_.Z; z<=maxVox_.Z; ++z)
						if (CVoxelCave::DockingVoxel != VoxelCave->GetVoxel(begin,y,z))	// not done here?
						{
							done = false;
							if (0 == VoxelCave->GetVoxel(x,y,z))
								VoxelCave->SetVoxel(x,y,z,1);							// dig away stone voxel
							else
								VoxelCave->SetVoxel(begin,y,z,CVoxelCave::DockingVoxel);// digging done here, free space achieved -> mark docking voxel
						}
				if (done)
					break;
			}

			// create docking site
			if (EDirection::X_POSITIVE == direction_)
				++begin; // upper vertex coordinate of voxel coordinate is +1

			dockingSite_.Normal = irr::core::vector3d<double>(-increment,0,0);
			dockingSite_.Center = irr::core::vector3d<double>(
				static_cast<double>(begin),
				static_cast<double>(minVox_.Y+maxVox_.Y+1)*0.5,
				static_cast<double>(minVox_.Z+maxVox_.Z+1)*0.5
				);

			// add points: clockwise in corridor direction
			irr::core::vector3df position;
			irr::f32 tmp = 0.0f;
			for (unsigned int i=maxVox_.Z+1; i>=minVox_.Z; --i)		// Z-axis: (Ymin,Zmax+1) to (Ymin,Zmin)
			{
				position = MeshCave->ComputeVertexCoordinates(begin, minVox_.Y, i, tmp);
				dockingSite_.Point.push_back(vec3D(position));
			}		
			for (unsigned int i=minVox_.Y+1; i<=maxVox_.Y; ++i)		// Y-Axis: (Ymin+1,Zmin) to (Ymax,Zmin)
			{
				position = MeshCave->ComputeVertexCoordinates(begin, i, minVox_.Z, tmp);
				dockingSite_.Point.push_back(vec3D(position));
			}		
			for (unsigned int i=minVox_.Z; i<=maxVox_.Z+1; ++i)		// Z-Axis: (Ymax+1,Zmin) to (Ymax+1,Zmax+1)
			{
				position = MeshCave->ComputeVertexCoordinates(begin, maxVox_.Y+1, i, tmp);
				dockingSite_.Point.push_back(vec3D(position));
			}
			for (unsigned int i=maxVox_.Y; i>=minVox_.Y+1; --i)		// Y-Axis: (Ymax,Zmax+1) to (Ymin+1,Zmax+1)
			{
				position = MeshCave->ComputeVertexCoordinates(begin, i, maxVox_.Z+1, tmp);
				dockingSite_.Point.push_back(vec3D(position));
			}

			// reverse point direction if necessary
			if (EDirection::X_NEGATIVE == direction_)
				std::reverse(dockingSite_.Point.begin(), dockingSite_.Point.end());
		}
		break;
	case EDirection::Z_NEGATIVE:
	case EDirection::Z_POSITIVE:
		{
			// adjust cave
			int begin = Infinite;

			const unsigned int startZ = (EDirection::Z_POSITIVE == direction_) ? minVox_.Z : maxVox_.Z;
			const unsigned int endZ = (EDirection::Z_POSITIVE == direction_) ?
				SVoxelSpace::DimZ-SVoxelSpace::MinBorder : VoxelCave->MinBorderFilter; // 3 stone voxel at border
			const int increment = (EDirection::Z_POSITIVE == direction_) ? 1 : -1;
			const int depthModificator = maxVox_.Z - minVox_.Z + 1;

			// check for begin of voxel cave
			for (unsigned int z=startZ; z!=endZ && Infinite == begin; z+=increment)
				for (unsigned int x=minVox_.X; x<=maxVox_.X; ++x)
					for (unsigned int y=minVox_.Y; y<=maxVox_.Y; ++y)
						if (1 == VoxelCave->GetVoxel(x,y,z))
						{
							begin = z;
							x = maxVox_.X+1;		// break outer loop
							break;
						}

			// no valid docking position?
			if (Infinite == begin)
				return false;

			// adjust docking position
			begin -= depthModificator * increment;
			// docking position has to be at least 2 voxel from border, so that digging starts 3 voxel away from border
			if (begin < static_cast<int>(VoxelCave->MinBorderFilter))
				begin = VoxelCave->MinBorderFilter;
			else if (begin > static_cast<int>(SVoxelSpace::DimZ-SVoxelSpace::MinBorder))
				begin = SVoxelSpace::DimZ-SVoxelSpace::MinBorder;

			// dig away the stone voxels & mark docking site
			for (unsigned int z=begin+increment; z!=endZ; z+=increment)
			{
				bool done = true;
				for (unsigned int x=minVox_.X; x<=maxVox_.X; ++x)
					for (unsigned int y=minVox_.Y; y<=maxVox_.Y; ++y)
						if (CVoxelCave::DockingVoxel != VoxelCave->GetVoxel(x,y,begin))	// not done here?
						{
							done = false;
							if (0 == VoxelCave->GetVoxel(x,y,z))
								VoxelCave->SetVoxel(x,y,z,1);							// dig away stone voxel
							else
								VoxelCave->SetVoxel(x,y,begin,CVoxelCave::DockingVoxel);// digging done here, free space achieved -> mark docking voxel
						}
				if (done)
					break;
			}

			// create docking site
			if (EDirection::Z_POSITIVE == direction_)
				++begin; // upper vertex coordinate of voxel coordinate is +1

			dockingSite_.Normal = irr::core::vector3d<double>(0,0,-increment);
			dockingSite_.Center = irr::core::vector3d<double>(
				static_cast<double>(minVox_.X+maxVox_.X+1)*0.5,
				static_cast<double>(minVox_.Y+maxVox_.Y+1)*0.5,
				static_cast<double>(begin)
				);

			// add points: clockwise in corridor direction
			irr::core::vector3df position;
			for (unsigned int i=maxVox_.X+1; i>=minVox_.X; --i)		// Z-axis: (Ymin,Zmax+1) to (Ymin,Zmin)
			{
				position = MeshCave->ComputeVertexCoordinates(i, maxVox_.Y+1, begin, tmp);
				dockingSite_.Point.push_back(vec3D(position));
			}		
			for (unsigned int i=maxVox_.Y+1; i>=minVox_.Y+1; --i)	// Y-Axis: (Ymin+1,Zmin) to (Ymax,Zmin)
			{
				position = MeshCave->ComputeVertexCoordinates(minVox_.X, i, begin, tmp);
				dockingSite_.Point.push_back(vec3D(position));
			}		
			for (unsigned int i=minVox_.X; i<=maxVox_.X+1; ++i)		// Z-Axis: (Ymax+1,Zmin) to (Ymax+1,Zmax+1)
			{
				position = MeshCave->ComputeVertexCoordinates(i, minVox_.Y, begin, tmp);
				dockingSite_.Point.push_back(vec3D(position));
			}
			for (unsigned int i=minVox_.Y+1; i<=maxVox_.Y+1; ++i)	// Y-Axis: (Ymax,Zmax+1) to (Ymin+1,Zmax+1)
			{
				position = MeshCave->ComputeVertexCoordinates(maxVox_.X+1, i, begin, tmp);
				dockingSite_.Point.push_back(vec3D(position));
			}

			// reverse point direction if necessary
			if (EDirection::Z_NEGATIVE == direction_)
				std::reverse(dockingSite_.Point.begin(), dockingSite_.Point.end());
		}
		break;
	default:
		{
			return false;
		}
		break;
	}

	return true;
}

