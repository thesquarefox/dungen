// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#include "MeshCave.h"

// ======================================================
// structs
// ======================================================

unsigned int DunGen::CMeshCave::SOctreeNode::CountVertices()
{
	// sum up vertex count from child nodes
	if (NULL != ChildNode[0])
	{
		VertexNumber = 0;
		for (unsigned int i=0; i<8; ++i)
			VertexNumber += ChildNode[i]->CountVertices();
	}
	// return the result
	return VertexNumber;
}

bool DunGen::CMeshCave::SVertexInformations::operator<(const SVertexInformations& other_)
{
	// sorted by the Y coordinate
	return (Y < other_.Y);
}

// ======================================================
// setters
// ======================================================

void DunGen::CMeshCave::SetWarpRandomSeed(unsigned int seed_)
{
	RandomSeed = seed_;
}

void DunGen::CMeshCave::SetWarpStrength(double value_)
{
	WarpStrength = value_;
	// clamp
	if (WarpStrength > MaxClampDistance)
		WarpStrength = MaxClampDistance;
	if (WarpStrength < 0.0)
		WarpStrength = 0.0;
}

void DunGen::CMeshCave::SetWarpOption(bool enabled_)
{
	WarpEnabled = enabled_;
}

void DunGen::CMeshCave::SetSmoothOption(bool enabled_)
{
	SmoothEnabled = enabled_;
}

void DunGen::CMeshCave::SetNormalWeightMethod(ENormalWeightMethod::Enum normalWeightMethod_)
{
	NormalWeightMethod = normalWeightMethod_;
}

void DunGen::CMeshCave::SetPrintToConsole(bool enabled_)
{
	PrintToConsole = enabled_;
}

// ======================================================
// getters
// ======================================================

irr::scene::SMesh* DunGen::CMeshCave::GetMesh()
{
	return Mesh;
}

// ======================================================
// initialization
// ======================================================

const double DunGen::CMeshCave::MaxClampDistance = 0.49;

// no inlining
__declspec(noinline) DunGen::CMeshCave::CMeshCave(CVoxelCave* voxelCave_, const CRandomGenerator* randomGenerator_)
	: VoxelCave(voxelCave_)
	, RandomGenerator(randomGenerator_)
	, WarpEnabled(true)
	, SmoothEnabled(true)
	, WarpStrength(0.35)
	, NormalWeightMethod(ENormalWeightMethod::BY_AREA)
	, PrintToConsole(false)
{
	// create empty mesh
	Mesh = new irr::scene::SMesh();
	Mesh->recalculateBoundingBox();

	// initialize hashtable
	for (unsigned int i=0; i<SVoxelSpace::DimX+1; ++i)
		for (unsigned int j=0; j<SVoxelSpace::DimY+1; ++j)
			HashTable[i][j] = NULL;

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// init lookup table for warp directions:
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// adress: voxels as bits (1 or 0)
	// order of voxels: X ascending, Y ascending, Z ascending
	// -> (X,Y,Z) = (-1,-1,-1),(0,-1,-1),(-1,0,-1),(0,0,-1), (-1,-1,0),(0,-1,0),(-1,0,0),(0,0,0)

	// preallocate memory
	unsigned char voxel1, voxel2, voxel3, voxel4, voxel5, voxel6, voxel7, voxel8;
	int xNegative, xPositive, yNegative, yPositive, zNegative, zPositive;
	int tempInt;

	// walp over all indices of the table
	for (unsigned short i=0; i<256; ++i)
	{
		// retrieve voxel bits
		voxel1 = ((i & 1) > 0) ? 1 : 0;
		voxel2 = ((i & 2) > 0) ? 1 : 0;
		voxel3 = ((i & 4) > 0) ? 1 : 0;
		voxel4 = ((i & 8) > 0) ? 1 : 0;
		voxel5 = ((i & 16) > 0) ? 1 : 0;
		voxel6 = ((i & 32) > 0) ? 1 : 0;
		voxel7 = ((i & 64) > 0) ? 1 : 0;
		voxel8 = ((i & 128) > 0) ? 1 : 0;

		// initialize direction weightings
		xNegative = 0, xPositive = 0;
		yNegative = 0, yPositive = 0;
		zNegative = 0, zPositive = 0;
		
		// X negative:
		tempInt = voxel1 + voxel3 + voxel5 +voxel7;
		if ((4 > tempInt) && (1 < tempInt)) // does an edge points in this direction?
		{
			xNegative = 1;
			// does an 2-cell-sized area points in this direction?
			if ((2 == tempInt) && (voxel1 != voxel7))
					xNegative = 2;
		}
		// X positive:
		tempInt = voxel2 + voxel4 + voxel6 +voxel8;
		if ((4 > tempInt) && (1 < tempInt)) // does an edge points in this direction?
		{
			xPositive = 1;
			// does an 2-cell-sized area points in this direction?
			if ((2 == tempInt) && (voxel2 != voxel8))
					xPositive = 2;
		}

		// Y negative:
		tempInt = voxel1 + voxel2 + voxel5 +voxel6;
		if ((4 > tempInt) && (1 < tempInt)) // does an edge points in this direction?
		{
			yNegative = 1;
			// does an 2-cell-sized area points in this direction?
			if ((2 == tempInt) && (voxel1 != voxel6))
					yNegative = 2;
		}
		// Y positive:
		tempInt = voxel3 + voxel4 + voxel7 +voxel8;
		if ((4 > tempInt) && (1 < tempInt)) // does an edge points in this direction?
		{
			yPositive = 1;
			// does an 2-cell-sized area points in this direction?
			if ((2 == tempInt) && (voxel3 != voxel8))
					yPositive = 2;
		}

		// Z negative:
		tempInt = voxel1 + voxel2 + voxel3 +voxel4;
		if ((4 > tempInt) && (1 < tempInt)) // does an edge points in this direction?
		{
			zNegative = 1;
			// does an 2-cell-sized area points in this direction?
			if ((2 == tempInt) && (voxel1 != voxel4))
					zNegative = 2;
		}
		// Z positive:
		tempInt = voxel5 + voxel6 + voxel7 + voxel8;
		if ((4 > tempInt) && (1 < tempInt)) // does an edge points in this direction?
		{
			zPositive = 1;
			// does an 2-cell-sized area points in this direction?
			if ((2 == tempInt) && (voxel5 != voxel8))
					zPositive = 2;
		}

		// set modificators: allowed warp directions
		// (-1 ... only negative direction, 0 ... both directions, 1 ... only positive direction)
		if (xPositive == xNegative)
			VertexWarpDirections[i].DirectionX = 0;		// both directions allowed
		else if (xPositive > xNegative)
			VertexWarpDirections[i].DirectionX = 1;		// only positive direction allowed
		else
			VertexWarpDirections[i].DirectionX = -1;	// only negative direction allowed

		if (yPositive == yNegative)
			VertexWarpDirections[i].DirectionY = 0;		// both directions allowed
		else if (yPositive > yNegative)
			VertexWarpDirections[i].DirectionY = 1;		// only positive direction allowed
		else
			VertexWarpDirections[i].DirectionY = -1;	// only negative direction allowed

		if (zPositive == zNegative)
			VertexWarpDirections[i].DirectionZ = 0;		// both directions allowed
		else if (zPositive > zNegative)
			VertexWarpDirections[i].DirectionZ = 1;		// only positive direction allowed
		else
			VertexWarpDirections[i].DirectionZ = -1;	// only negative direction allowed

	} // ENDE: initialization lookup table for warp directions

	// ~~~~~~~~~~~~~~~~~~
	// precompute octree:
	// ~~~~~~~~~~~~~~~~~~
	// the voxelspace will be splitup, so that the maximum amount
	// of vertices needed for one leaf is at most MaxVertexCount

	// create root node
	SOctreeNode* actualOctreeNode = new SOctreeNode();
	actualOctreeNode->BorderMinX = SVoxelSpace::MinBorder;
	actualOctreeNode->BorderMinY = SVoxelSpace::MinBorder;
	actualOctreeNode->BorderMinZ = SVoxelSpace::MinBorder;
	actualOctreeNode->BorderMaxX = SVoxelSpace::DimX - SVoxelSpace::MinBorder - 1;
	actualOctreeNode->BorderMaxY = SVoxelSpace::DimY - SVoxelSpace::MinBorder - 1;
	actualOctreeNode->BorderMaxZ = SVoxelSpace::DimZ - SVoxelSpace::MinBorder - 1;
	for (unsigned int i=0; i<8; ++i)
		actualOctreeNode->ChildNode[i] = NULL;
	actualOctreeNode->ParentNode = NULL;
	
	Octree.push_back(actualOctreeNode);

	// build up octree to the required depth
	// maximum needed vertices = (dimX+1) * (dimY+1) * (dimZ+1)
	unsigned int borderX, borderY, borderZ;
	unsigned int actualNodeID = 0;
	while (actualNodeID<Octree.size())
	{
		// choose next node
		actualOctreeNode = Octree[actualNodeID];

		// if size too large: split up
		if (
		((actualOctreeNode->BorderMaxX-actualOctreeNode->BorderMinX+1)*
		(actualOctreeNode->BorderMaxX-actualOctreeNode->BorderMinX+1)*
		(actualOctreeNode->BorderMaxX-actualOctreeNode->BorderMinX+1)) > MaxVertexCount)
		{
			// compute borders for splitting
			borderX = actualOctreeNode->BorderMinX+(actualOctreeNode->BorderMaxX-actualOctreeNode->BorderMinX)/2;
			borderY = actualOctreeNode->BorderMinY+(actualOctreeNode->BorderMaxY-actualOctreeNode->BorderMinY)/2;
			borderZ = actualOctreeNode->BorderMinZ+(actualOctreeNode->BorderMaxZ-actualOctreeNode->BorderMinZ)/2;

			// create child nodes
			actualOctreeNode->ChildNode[0] = new SOctreeNode();
			actualOctreeNode->ChildNode[0]->BorderMinX = actualOctreeNode->BorderMinX;
			actualOctreeNode->ChildNode[0]->BorderMaxX = borderX;
			actualOctreeNode->ChildNode[0]->BorderMinY = actualOctreeNode->BorderMinY;
			actualOctreeNode->ChildNode[0]->BorderMaxY = borderY;
			actualOctreeNode->ChildNode[0]->BorderMinZ = actualOctreeNode->BorderMinZ;
			actualOctreeNode->ChildNode[0]->BorderMaxZ = borderZ;

			actualOctreeNode->ChildNode[1] = new SOctreeNode();
			actualOctreeNode->ChildNode[1]->BorderMinX = borderX+1;
			actualOctreeNode->ChildNode[1]->BorderMaxX = actualOctreeNode->BorderMaxX;
			actualOctreeNode->ChildNode[1]->BorderMinY = actualOctreeNode->BorderMinY;
			actualOctreeNode->ChildNode[1]->BorderMaxY = borderY;
			actualOctreeNode->ChildNode[1]->BorderMinZ = actualOctreeNode->BorderMinZ;
			actualOctreeNode->ChildNode[1]->BorderMaxZ = borderZ;

			actualOctreeNode->ChildNode[2] = new SOctreeNode();
			actualOctreeNode->ChildNode[2]->BorderMinX = actualOctreeNode->BorderMinX;
			actualOctreeNode->ChildNode[2]->BorderMaxX = borderX;
			actualOctreeNode->ChildNode[2]->BorderMinY = borderY+1;
			actualOctreeNode->ChildNode[2]->BorderMaxY = actualOctreeNode->BorderMaxY;
			actualOctreeNode->ChildNode[2]->BorderMinZ = actualOctreeNode->BorderMinZ;
			actualOctreeNode->ChildNode[2]->BorderMaxZ = borderZ;

			actualOctreeNode->ChildNode[3] = new SOctreeNode();
			actualOctreeNode->ChildNode[3]->BorderMinX = borderX+1;
			actualOctreeNode->ChildNode[3]->BorderMaxX = actualOctreeNode->BorderMaxX;
			actualOctreeNode->ChildNode[3]->BorderMinY = borderY+1;
			actualOctreeNode->ChildNode[3]->BorderMaxY = actualOctreeNode->BorderMaxY;
			actualOctreeNode->ChildNode[3]->BorderMinZ = actualOctreeNode->BorderMinZ;
			actualOctreeNode->ChildNode[3]->BorderMaxZ = borderZ;

			actualOctreeNode->ChildNode[4] = new SOctreeNode();
			actualOctreeNode->ChildNode[4]->BorderMinX = actualOctreeNode->BorderMinX;
			actualOctreeNode->ChildNode[4]->BorderMaxX = borderX;
			actualOctreeNode->ChildNode[4]->BorderMinY = actualOctreeNode->BorderMinY;
			actualOctreeNode->ChildNode[4]->BorderMaxY = borderY;
			actualOctreeNode->ChildNode[4]->BorderMinZ = borderZ+1;
			actualOctreeNode->ChildNode[4]->BorderMaxZ = actualOctreeNode->BorderMaxZ;

			actualOctreeNode->ChildNode[5] = new SOctreeNode();
			actualOctreeNode->ChildNode[5]->BorderMinX = borderX+1;
			actualOctreeNode->ChildNode[5]->BorderMaxX = actualOctreeNode->BorderMaxX;
			actualOctreeNode->ChildNode[5]->BorderMinY = actualOctreeNode->BorderMinY;
			actualOctreeNode->ChildNode[5]->BorderMaxY = borderY;
			actualOctreeNode->ChildNode[5]->BorderMinZ = borderZ+1;
			actualOctreeNode->ChildNode[5]->BorderMaxZ = actualOctreeNode->BorderMaxZ;

			actualOctreeNode->ChildNode[6] = new SOctreeNode();
			actualOctreeNode->ChildNode[6]->BorderMinX = actualOctreeNode->BorderMinX;
			actualOctreeNode->ChildNode[6]->BorderMaxX = borderX;
			actualOctreeNode->ChildNode[6]->BorderMinY = borderY+1;
			actualOctreeNode->ChildNode[6]->BorderMaxY = actualOctreeNode->BorderMaxY;
			actualOctreeNode->ChildNode[6]->BorderMinZ = borderZ+1;
			actualOctreeNode->ChildNode[6]->BorderMaxZ = actualOctreeNode->BorderMaxZ;

			actualOctreeNode->ChildNode[7] = new SOctreeNode();
			actualOctreeNode->ChildNode[7]->BorderMinX = borderX+1;
			actualOctreeNode->ChildNode[7]->BorderMaxX = actualOctreeNode->BorderMaxX;
			actualOctreeNode->ChildNode[7]->BorderMinY = borderY+1;
			actualOctreeNode->ChildNode[7]->BorderMaxY = actualOctreeNode->BorderMaxY;
			actualOctreeNode->ChildNode[7]->BorderMinZ = borderZ+1;
			actualOctreeNode->ChildNode[7]->BorderMaxZ = actualOctreeNode->BorderMaxZ;			
			
			// for all child nodes
			for (unsigned int i=0; i<8; ++i)
			{
				// store parent node
				actualOctreeNode->ChildNode[i]->ParentNode = actualOctreeNode;

				// child nodes have no childs currently (are leafs)
				for (unsigned int j=0; j<8; ++j)
					actualOctreeNode->ChildNode[i]->ChildNode[j] = NULL;
				
				// add child nodes to the octree
				Octree.push_back(actualOctreeNode->ChildNode[i]);
			}
		}
		// next node
		++actualNodeID;
	}

} // END: constructor


DunGen::CMeshCave::~CMeshCave()
{
	// delete all octree nodes
	for (unsigned int i=0; i<Octree.size(); ++i)
		delete Octree[i];
	
	// drop mesh
	Mesh->drop();
}