// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#include "MeshCave.h"
#include "Helperfunctions.h"
#include "RandomGenerator.h"
#include "VoxelCave.h"
#include <iostream>
#include <queue>

// ======================================================
// convert
// ======================================================
void DunGen::CMeshCave::CreateMeshFromVoxels()
{
	// actualize the octree
	ComputeOctree();

	// create the geometry
	ComputeGeometry();

	// compute the normals
	ComputeNormals();
}

void DunGen::CMeshCave::ComputeOctree()
{
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// count vertices in every octree-node
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	if (PrintToConsole) std::cout << "voxel-to-mesh step 1: count needed vertices..." << std::endl;

	SOctreeNode* actualOctreeNode;
	unsigned int actualSweepPlane1=0;
	unsigned int actualSweepPlane2=1;
	unsigned int tempInt;

    // loop over every node
	for (unsigned int i=0; i<Octree.size(); ++i)
	{
		actualOctreeNode = Octree[i];
		actualOctreeNode->VertexNumber = 0;

		// only when leaf: count vertices
		if (NULL == actualOctreeNode->ChildNode[0])
		{
			// initialize sweep planes: they tell, if a vertex on this position is needed or not
			// (0 ... is needed, 1 ... isn't needed)
			for (unsigned int k=actualOctreeNode->BorderMinY; k<=actualOctreeNode->BorderMaxY+1; ++k)
				for (unsigned int l=actualOctreeNode->BorderMinZ; l<=actualOctreeNode->BorderMaxZ+1; ++l)
				{
					GlobalSweepPlane[actualSweepPlane1][k][l] = 0;
					GlobalSweepPlane[actualSweepPlane2][k][l] = 0;
				}
			
			// sweep along X-axis:
			for (unsigned int j=actualOctreeNode->BorderMinX; j<=actualOctreeNode->BorderMaxX; ++j)
			{
				// YZ-plane is the sweep plane
				for (unsigned int k=actualOctreeNode->BorderMinY; k<=actualOctreeNode->BorderMaxY; ++k)
					for (unsigned int l=actualOctreeNode->BorderMinZ; l<=actualOctreeNode->BorderMaxZ; ++l)
					{
						// if actual voxel is 1 and neighbor voxel is 0 -> vertices needed
						// (because a quad from 2 triangles will be created here later)
						if (1 == VoxelCave->GetVoxel(j,k,l))
						{
							if (0 == VoxelCave->GetVoxel(j-1,k,l))
							{
								GlobalSweepPlane[actualSweepPlane1][k][l] = 1;
								GlobalSweepPlane[actualSweepPlane1][k+1][l] = 1;
								GlobalSweepPlane[actualSweepPlane1][k][l+1] = 1;
								GlobalSweepPlane[actualSweepPlane1][k+1][l+1] = 1;
							}
							if (0 == VoxelCave->GetVoxel(j+1,k,l))
							{
								GlobalSweepPlane[actualSweepPlane2][k][l] = 1;
								GlobalSweepPlane[actualSweepPlane2][k+1][l] = 1;
								GlobalSweepPlane[actualSweepPlane2][k][l+1] = 1;
								GlobalSweepPlane[actualSweepPlane2][k+1][l+1] = 1;
							}
							if (0 == VoxelCave->GetVoxel(j,k-1,l))
							{
								GlobalSweepPlane[actualSweepPlane1][k][l] = 1;
								GlobalSweepPlane[actualSweepPlane1][k][l+1] = 1;
								GlobalSweepPlane[actualSweepPlane2][k][l] = 1;
								GlobalSweepPlane[actualSweepPlane2][k][l+1] = 1;
							}
							if (0 == VoxelCave->GetVoxel(j,k+1,l))
							{
								GlobalSweepPlane[actualSweepPlane1][k+1][l] = 1;
								GlobalSweepPlane[actualSweepPlane1][k+1][l+1] = 1;
								GlobalSweepPlane[actualSweepPlane2][k+1][l] = 1;
								GlobalSweepPlane[actualSweepPlane2][k+1][l+1] = 1;
							}
							if (0 == VoxelCave->GetVoxel(j,k,l-1))
							{
								GlobalSweepPlane[actualSweepPlane1][k][l] = 1;
								GlobalSweepPlane[actualSweepPlane1][k+1][l] = 1;
								GlobalSweepPlane[actualSweepPlane2][k][l] = 1;
								GlobalSweepPlane[actualSweepPlane2][k+1][l] = 1;
							}
							if (0 == VoxelCave->GetVoxel(j,k,l+1))
							{
								GlobalSweepPlane[actualSweepPlane1][k][l+1] = 1;
								GlobalSweepPlane[actualSweepPlane1][k+1][l+1] = 1;
								GlobalSweepPlane[actualSweepPlane2][k][l+1] = 1;
								GlobalSweepPlane[actualSweepPlane2][k+1][l+1] = 1;
							}
						}
					} // END: YZ-plane

				// reset used sweep plane area and increment vertex count
				for (unsigned int k=actualOctreeNode->BorderMinY; k<=actualOctreeNode->BorderMaxY+1; ++k)
					for (unsigned int l=actualOctreeNode->BorderMinZ; l<=actualOctreeNode->BorderMaxZ+1; ++l)
					{
						actualOctreeNode->VertexNumber += GlobalSweepPlane[actualSweepPlane1][k][l];
						GlobalSweepPlane[actualSweepPlane1][k][l] = 0;
					}
					
				// swap sweep planes
				tempInt = actualSweepPlane1;
				actualSweepPlane1 = actualSweepPlane2;
				actualSweepPlane2 = tempInt;

			} // END: sweep along X-axis
			
			// final increment
			for (unsigned int k=actualOctreeNode->BorderMinY; k<=actualOctreeNode->BorderMaxY+1; ++k)
				for (unsigned int l=actualOctreeNode->BorderMinZ; l<=actualOctreeNode->BorderMaxZ+1; ++l)
					actualOctreeNode->VertexNumber += GlobalSweepPlane[actualSweepPlane1][k][l];

		} // END: leaf check
			
	} // END: loop over every node

	// compute vertex count in non-leaf-nodes
	// note: this number could be higher than the real amount,
	// because the overlapping border vertices of the leaf nodes only will be created once,
	// if they belong to the same meshbuffer.
	Octree[0]->CountVertices(); // count from the root node
	if (PrintToConsole) std::cout << "#vertices needed (at most): " << Octree[0]->VertexNumber << std::endl;
}

void DunGen::CMeshCave::ComputeGeometry()
{
	// ~~~~~~~~~~~~~~~~
	// convert to mesh:
	// ~~~~~~~~~~~~~~~~

	if (PrintToConsole) std::cout << "voxel-to-mesh step 2: converting to mesh..." << std::endl;

	// delete old mesh and create new one
	Mesh->drop();
	Mesh = new irr::scene::SMesh();

	SOctreeNode* actualOctreeNode;
	unsigned int actualSweepPlane1;
	unsigned int actualSweepPlane2;
	unsigned int tempInt;

	// create conversion queue and load up root node
	std::queue<SOctreeNode*> conversionQueue;
	conversionQueue.push(Octree[0]);

	// until queue is empty
	while (!conversionQueue.empty())
	{
		actualOctreeNode = conversionQueue.front();

		// to much vertices: use child nodes
		if (actualOctreeNode->VertexNumber > MaxVertexCount)
			for (unsigned int i=0; i<8; ++i)
				conversionQueue.push(actualOctreeNode->ChildNode[i]);
		
		// else if any of vertices: convert
		else if (0 < actualOctreeNode->VertexNumber)
		{
			if (PrintToConsole) std::cout << "new meshbuffer is created..."  << std::endl;

			// create and add buffer
			irr::scene::SMeshBuffer* meshBuffer = new irr::scene::SMeshBuffer();
			Mesh->addMeshBuffer(meshBuffer);
			// decrement reference counter, because the mesh is now responsible for the buffer
			meshBuffer->drop();
			unsigned int bufferVertices = 0;
			unsigned int bufferIndizes = 0;

			// reserve for the worst case
			meshBuffer->Vertices.set_used(MaxVertexCount); 
			// max per vertex: 12 quads * 2 triangles (worst case)
			meshBuffer->Indices.set_used(24*MaxVertexCount);

			actualSweepPlane1=0;
			actualSweepPlane2=1;

			// initialize sweep planes: storing the vertex indices now
			// index > MaxVertexCount --> index not set
			for (unsigned int j=actualOctreeNode->BorderMinY; j<=actualOctreeNode->BorderMaxY+1; ++j)
				for (unsigned int k=actualOctreeNode->BorderMinZ; k<=actualOctreeNode->BorderMaxZ+1; ++k)
				{
					GlobalSweepPlane[actualSweepPlane1][j][k] = MaxVertexCount+1;
					GlobalSweepPlane[actualSweepPlane2][j][k] = MaxVertexCount+1;
				}

			// converting per sweep:
			// sweep along X-axis
			for (unsigned int i=actualOctreeNode->BorderMinX; i<=actualOctreeNode->BorderMaxX; ++i)
			{
				// YZ-plane is the sweep plane
				for (unsigned int j=actualOctreeNode->BorderMinY; j<=actualOctreeNode->BorderMaxY; ++j)
					for (unsigned int k=actualOctreeNode->BorderMinZ; k<=actualOctreeNode->BorderMaxZ; ++k)
					{
						// if actual voxel is 1 and neighbor voxel is 0 -> creating triangles
						if (1 == VoxelCave->GetVoxel(i,j,k))
						{		
							// test along X-axis
							if (0 == VoxelCave->GetVoxel(i-1,j,k))
							{
								// add 4 vertices (if not already present) & store index in sweep plane
								CreateVertex(meshBuffer, bufferVertices, actualOctreeNode, actualSweepPlane1, i, j, k);
								CreateVertex(meshBuffer, bufferVertices, actualOctreeNode, actualSweepPlane1, i, j+1, k);
								CreateVertex(meshBuffer, bufferVertices, actualOctreeNode, actualSweepPlane1, i, j, k+1);
								CreateVertex(meshBuffer, bufferVertices, actualOctreeNode, actualSweepPlane1, i, j+1, k+1);

								// add 2 triangles: order of the vertices is clockwise in the left-handed irrlicht coordinate system
								meshBuffer->Indices[bufferIndizes++] = GlobalSweepPlane[actualSweepPlane1][j][k];
								meshBuffer->Indices[bufferIndizes++] = GlobalSweepPlane[actualSweepPlane1][j+1][k];
								meshBuffer->Indices[bufferIndizes++] = GlobalSweepPlane[actualSweepPlane1][j+1][k+1];
																
								meshBuffer->Indices[bufferIndizes++] = GlobalSweepPlane[actualSweepPlane1][j][k];
								meshBuffer->Indices[bufferIndizes++] = GlobalSweepPlane[actualSweepPlane1][j+1][k+1];
								meshBuffer->Indices[bufferIndizes++] = GlobalSweepPlane[actualSweepPlane1][j][k+1];
							}

							if (0 == VoxelCave->GetVoxel(i+1,j,k))
							{
								// add 4 vertices (if not already present) & store index in sweep plane
								CreateVertex(meshBuffer, bufferVertices, actualOctreeNode, actualSweepPlane2, i+1, j, k);
								CreateVertex(meshBuffer, bufferVertices, actualOctreeNode, actualSweepPlane2, i+1, j+1, k);
								CreateVertex(meshBuffer, bufferVertices, actualOctreeNode, actualSweepPlane2, i+1, j, k+1);
								CreateVertex(meshBuffer, bufferVertices, actualOctreeNode, actualSweepPlane2, i+1, j+1, k+1);
			
								// add 2 triangles: order of the vertices is clockwise in the left-handed irrlicht coordinate system
								meshBuffer->Indices[bufferIndizes++] = GlobalSweepPlane[actualSweepPlane2][j][k];
								meshBuffer->Indices[bufferIndizes++] = GlobalSweepPlane[actualSweepPlane2][j+1][k+1];
								meshBuffer->Indices[bufferIndizes++] = GlobalSweepPlane[actualSweepPlane2][j+1][k];
								
								meshBuffer->Indices[bufferIndizes++] = GlobalSweepPlane[actualSweepPlane2][j][k];
								meshBuffer->Indices[bufferIndizes++] = GlobalSweepPlane[actualSweepPlane2][j][k+1];
								meshBuffer->Indices[bufferIndizes++] = GlobalSweepPlane[actualSweepPlane2][j+1][k+1];
							}

							// test along Y-axis
							if (0 == VoxelCave->GetVoxel(i,j-1,k))
							{
								// add 4 vertices (if not already present) & store index in sweep plane
								CreateVertex(meshBuffer, bufferVertices, actualOctreeNode, actualSweepPlane1, i, j, k);
								CreateVertex(meshBuffer, bufferVertices, actualOctreeNode, actualSweepPlane1, i, j, k+1);
								CreateVertex(meshBuffer, bufferVertices, actualOctreeNode, actualSweepPlane2, i+1, j, k);
								CreateVertex(meshBuffer, bufferVertices, actualOctreeNode, actualSweepPlane2, i+1, j, k+1);

								// add 2 triangles: order of the vertices is clockwise in the left-handed irrlicht coordinate system
								meshBuffer->Indices[bufferIndizes++] = GlobalSweepPlane[actualSweepPlane1][j][k];
								meshBuffer->Indices[bufferIndizes++] = GlobalSweepPlane[actualSweepPlane1][j][k+1];
								meshBuffer->Indices[bufferIndizes++] = GlobalSweepPlane[actualSweepPlane2][j][k+1];

								meshBuffer->Indices[bufferIndizes++] = GlobalSweepPlane[actualSweepPlane1][j][k];
								meshBuffer->Indices[bufferIndizes++] = GlobalSweepPlane[actualSweepPlane2][j][k+1];
								meshBuffer->Indices[bufferIndizes++] = GlobalSweepPlane[actualSweepPlane2][j][k];
							}

							if (0 == VoxelCave->GetVoxel(i,j+1,k))
							{
								// add 4 vertices (if not already present) & store index in sweep plane
								CreateVertex(meshBuffer, bufferVertices, actualOctreeNode, actualSweepPlane1, i, j+1, k);
								CreateVertex(meshBuffer, bufferVertices, actualOctreeNode, actualSweepPlane1, i, j+1, k+1);
								CreateVertex(meshBuffer, bufferVertices, actualOctreeNode, actualSweepPlane2, i+1, j+1, k);
								CreateVertex(meshBuffer, bufferVertices, actualOctreeNode, actualSweepPlane2, i+1, j+1, k+1);

								// add 2 triangles: order of the vertices is clockwise in the left-handed irrlicht coordinate system
								meshBuffer->Indices[bufferIndizes++] = GlobalSweepPlane[actualSweepPlane1][j+1][k];
								meshBuffer->Indices[bufferIndizes++] = GlobalSweepPlane[actualSweepPlane2][j+1][k+1];
								meshBuffer->Indices[bufferIndizes++] = GlobalSweepPlane[actualSweepPlane1][j+1][k+1];

								meshBuffer->Indices[bufferIndizes++] = GlobalSweepPlane[actualSweepPlane1][j+1][k];
								meshBuffer->Indices[bufferIndizes++] = GlobalSweepPlane[actualSweepPlane2][j+1][k];
								meshBuffer->Indices[bufferIndizes++] = GlobalSweepPlane[actualSweepPlane2][j+1][k+1];
							}

							// test along Z-axis
							if (0 == VoxelCave->GetVoxel(i,j,k-1))
							{
								// add 4 vertices (if not already present) & store index in sweep plane
								CreateVertex(meshBuffer, bufferVertices, actualOctreeNode, actualSweepPlane1, i, j, k);
								CreateVertex(meshBuffer, bufferVertices, actualOctreeNode, actualSweepPlane1, i, j+1, k);
								CreateVertex(meshBuffer, bufferVertices, actualOctreeNode, actualSweepPlane2, i+1, j, k);
								CreateVertex(meshBuffer, bufferVertices, actualOctreeNode, actualSweepPlane2, i+1, j+1, k);

								// add 2 triangles: order of the vertices is clockwise in the left-handed irrlicht coordinate system
								meshBuffer->Indices[bufferIndizes++] = GlobalSweepPlane[actualSweepPlane1][j][k];
								meshBuffer->Indices[bufferIndizes++] = GlobalSweepPlane[actualSweepPlane2][j+1][k];
								meshBuffer->Indices[bufferIndizes++] = GlobalSweepPlane[actualSweepPlane1][j+1][k];

								meshBuffer->Indices[bufferIndizes++] = GlobalSweepPlane[actualSweepPlane1][j][k];
								meshBuffer->Indices[bufferIndizes++] = GlobalSweepPlane[actualSweepPlane2][j][k];
								meshBuffer->Indices[bufferIndizes++] = GlobalSweepPlane[actualSweepPlane2][j+1][k];
							}

							if (0 == VoxelCave->GetVoxel(i,j,k+1))
							{
								// add 4 vertices (if not already present) & store index in sweep plane
								CreateVertex(meshBuffer, bufferVertices, actualOctreeNode, actualSweepPlane1, i, j, k+1);
								CreateVertex(meshBuffer, bufferVertices, actualOctreeNode, actualSweepPlane1, i, j+1, k+1);
								CreateVertex(meshBuffer, bufferVertices, actualOctreeNode, actualSweepPlane2, i+1, j, k+1);
								CreateVertex(meshBuffer, bufferVertices, actualOctreeNode, actualSweepPlane2, i+1, j+1, k+1);
							
								// add 2 triangles: order of the vertices is clockwise in the left-handed irrlicht coordinate system
								meshBuffer->Indices[bufferIndizes++] = GlobalSweepPlane[actualSweepPlane1][j][k+1];
								meshBuffer->Indices[bufferIndizes++] = GlobalSweepPlane[actualSweepPlane1][j+1][k+1];
								meshBuffer->Indices[bufferIndizes++] = GlobalSweepPlane[actualSweepPlane2][j+1][k+1];
								
								meshBuffer->Indices[bufferIndizes++] = GlobalSweepPlane[actualSweepPlane1][j][k+1];
								meshBuffer->Indices[bufferIndizes++] = GlobalSweepPlane[actualSweepPlane2][j+1][k+1];
								meshBuffer->Indices[bufferIndizes++] = GlobalSweepPlane[actualSweepPlane2][j][k+1];
							}

						} // END: if actual voxel is 1

					} // END: YZ-plane

				// reset used area of the sweep plane
				for (unsigned int j=actualOctreeNode->BorderMinY; j<=actualOctreeNode->BorderMaxY+1; ++j)
					for (unsigned int k=actualOctreeNode->BorderMinZ; k<=actualOctreeNode->BorderMaxZ+1; ++k)
						GlobalSweepPlane[actualSweepPlane1][j][k] = MaxVertexCount+1;
					
				// swap sweep planes
				tempInt = actualSweepPlane1;
				actualSweepPlane1 = actualSweepPlane2;
				actualSweepPlane2 = tempInt;

			} // END: sweep along X-axis

			// compute final values of the mesh buffer
			meshBuffer->Vertices.reallocate(bufferVertices);
			meshBuffer->Indices.reallocate(bufferIndizes);
			meshBuffer->recalculateBoundingBox();

		} // END: conversion of this node

		// next node
		conversionQueue.pop();
	}

	// compute final values for the mesh
	Mesh->recalculateBoundingBox();
}

void DunGen::CMeshCave::ComputeNormals()
{
	if (PrintToConsole) std::cout << "voxel-to-mesh step 3.1: computing raw normals..." << std::endl;

	// ~~~~~~~~~~~~~~~~~~~
	// compute raw normals
	// ~~~~~~~~~~~~~~~~~~~

	// preallocate memory
	irr::scene::IMeshBuffer* meshBuffer;
	unsigned int vertexCount;
	unsigned int indexCount;

	irr::core::vector3d<double> normal;
	irr::core::vector3d<double> vertex1;
	irr::core::vector3d<double> vertex2;
	irr::core::vector3d<double> vertex3;		
	irr::core::vector3d<double> tempVec;
	
	// do this for all meshbuffers
	for (unsigned int i=0; i<Mesh->getMeshBufferCount(); ++i)
	{
		// read meshbuffer and its parameters
		meshBuffer = Mesh->getMeshBuffer(i);
		vertexCount = meshBuffer->getVertexCount();
		indexCount = meshBuffer->getIndexCount();
		const irr::u16* indices = meshBuffer->getIndices();

		// set all normals to 0
		for (unsigned int j=0; j<vertexCount; ++j)
			meshBuffer->getNormal(j).set(0.0f,0.0f,0.0f);

		// compute normals of every triangle and add them up
		// with the appropriate weighting in each affected vertex
		for (unsigned int j=0; j<indexCount; j+=3)
		{
			// compute normal of the actual triangle
			vertex1 = vec3D(meshBuffer->getPosition(indices[j+0]));
			vertex2 = vec3D(meshBuffer->getPosition(indices[j+1]));
			vertex3 = vec3D(meshBuffer->getPosition(indices[j+2]));

			normal = (vertex2-vertex1).crossProduct(vertex3-vertex1);

			// compute weight of the normal in each of the 3 corner points of the triangle
			switch (NormalWeightMethod)
			{
			case ENormalWeightMethod::BY_AREA:
				{	
					// normal is already weighted with area, if it is not normalized
					tempVec.set(1.0,1.0,1.0);
				}
				break;
			case ENormalWeightMethod::BY_ANGLE:
				{
					normal.normalize();
					tempVec = computeAngleWeight(vertex1,vertex2,vertex3);
				}
				break;
			case ENormalWeightMethod::UNIFORM:
				{
					normal.normalize();
					tempVec.set(1.0,1.0,1.0);
				}
				break;
			}
			
			// sum up weighted normal
			meshBuffer->getNormal(indices[j+0]) += vec3F(tempVec.X * normal);
			meshBuffer->getNormal(indices[j+1]) += vec3F(tempVec.Y * normal);
			meshBuffer->getNormal(indices[j+2]) += vec3F(tempVec.Z * normal);
		}
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// combine normals of border vertices
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	if (PrintToConsole) std::cout << "voxel-to-mesh step 3.2: combine normals from border vertices..." << std::endl;

	unsigned int hashX, hashZ;

	for (unsigned int i=0; i<Mesh->getMeshBufferCount(); ++i)
	{
		// read meshbuffer
		meshBuffer = Mesh->getMeshBuffer(i);
		vertexCount = meshBuffer->getVertexCount();
		
		for (unsigned int j=0; j<vertexCount; ++j)
			// if border vertex:
			if (meshBuffer->getTCoords(j).X > 0.0)
			{
				// read vertex information
				SVertexInformations newVertexInfo;
				newVertexInfo.Normal = vec3D(meshBuffer->getNormal(j));
				tempVec = vec3D(meshBuffer->getPosition(j));
				newVertexInfo.Y = d2i(tempVec.Y);
				
				// store adress of the vertex
				newVertexInfo.Address.MeshbufferID = i;
				newVertexInfo.Address.VertexID = j;

				// save this in the hash table
				hashX = d2i(tempVec.X);
				hashZ = d2i(tempVec.Z);

				if (NULL == HashTable[hashX][hashZ])
					HashTable[hashX][hashZ] = new std::list<SVertexInformations>();

				HashTable[hashX][hashZ]->push_front(newVertexInfo);
			}
	}

	// preallocate memory
	unsigned int actualY;
	std::queue<SVertexAddress> verticesToActualize;
	SVertexAddress actualVertexAdress;

	// process hash table
	for (unsigned int i=0; i<SVoxelSpace::DimX+1; ++i)
		for (unsigned int j=0; j<SVoxelSpace::DimY+1; ++j)
			if (NULL != HashTable[i][j])
			{
				// sort table by Y
				HashTable[i][j]->sort();

				// combine table entries
				actualY = HashTable[i][j]->front().Y;
				normal = irr::core::vector3d<double>(0.0,0.0,0.0);
				
				// walk over all entries
				for (std::list<SVertexInformations>::iterator it=HashTable[i][j]->begin() ; it != HashTable[i][j]->end(); ++it)
				{
					// if new vertex -> store old normal
					if (actualY!=(*it).Y)
					{
						// save normal to all affected vertices
						while (!verticesToActualize.empty())
						{
							actualVertexAdress = verticesToActualize.front();
							Mesh->getMeshBuffer(actualVertexAdress.MeshbufferID)
								->getNormal(actualVertexAdress.VertexID) = vec3F(normal);
							verticesToActualize.pop();
						}
						// store new Y
						actualY = (*it).Y;
						// reset normal
						normal = irr::core::vector3d<double>(0.0,0.0,0.0);
					}
					// sum up normal and store vertex adress
					normal += (*it).Normal;
					verticesToActualize.push((*it).Address);
				}

				// process last values
				// save normal to all affected vertices
				while (!verticesToActualize.empty())
				{
					actualVertexAdress = verticesToActualize.front();
					Mesh->getMeshBuffer(actualVertexAdress.MeshbufferID)
						->getNormal(actualVertexAdress.VertexID) = vec3F(normal);
					verticesToActualize.pop();
				}

				// delete hash entries, because they are processed
				delete HashTable[i][j];
				HashTable[i][j] = NULL;
			}

	// ~~~~~~~~~~~~~~~~~~~~~
	// normalize all normals
	// ~~~~~~~~~~~~~~~~~~~~~
	if (PrintToConsole) std::cout << "voxel-to-mesh step 3.3: normalize normals..." << std::endl;
	for (unsigned int i=0; i<Mesh->getMeshBufferCount(); ++i)
	{
		// read meshbuffer
		meshBuffer = Mesh->getMeshBuffer(i);
		vertexCount = meshBuffer->getVertexCount();
		
		// for all vertices of the buffer: normalize normal
		for (unsigned int j=0; j<vertexCount; ++j)
			meshBuffer->getNormal(j).normalize();
	}
}

inline void DunGen::CMeshCave::CreateVertex(irr::scene::SMeshBuffer* meshBuffer_, unsigned int& bufferVertices_, SOctreeNode* octreeNode_,
	unsigned int sweepPlaneLayer_, unsigned int x_, unsigned int y_, unsigned int z_)
{
	// test if no vertex is present already
	if (GlobalSweepPlane[sweepPlaneLayer_][y_][z_] > MaxVertexCount)
	{	
		// create new vertex
		irr::video::S3DVertex& v = meshBuffer_->Vertices[bufferVertices_];
		GlobalSweepPlane[sweepPlaneLayer_][y_][z_] = bufferVertices_++;

		// marking will be computed by ComputeVertexCoordinates() and saved as texture coordinate Y
		irr::f32 markingDockingVertex;

		// compute and set vertex coordinates
		v.Pos.set(ComputeVertexCoordinates(x_,y_,z_,markingDockingVertex));

		// compute features of the vertex: bordervertex, dockingvertex -> is saved as texture coordinates
		// (the texture coordinates are not being used for other reasons)
		v.TCoords.set(IsBorderVertex(x_,y_,z_, octreeNode_), markingDockingVertex);
	}
}

irr::core::vector3d<irr::f32> DunGen::CMeshCave::ComputeVertexCoordinates(unsigned int x_, unsigned int y_, unsigned int z_, irr::f32& markingDockingVertex_)
{
	// no warping: grid coordinates are used
	if (!WarpEnabled)
		return irr::core::vector3d<irr::f32>(static_cast<irr::f32>(x_),static_cast<irr::f32>(y_),static_cast<irr::f32>(z_));

	// set random seed based on coordinates, try to avoid symmetry
	// important: process has to be deterministic
	// (the coordinates of bordervertices have to be identical for all affected meshbuffers)
	RandomGenerator->SetSeed(RandomSeed + x_ + (SVoxelSpace::DimX+1)*y_ + (SVoxelSpace::DimX+1)*(SVoxelSpace::DimY+1)*z_); 
	double deltaX, deltaY, deltaZ;

	// if a 6-connected voxel is marked with 3 (dockingvoxel), this is a dockingvertex
	// other voxels only have a marking of 0 or 1 here
	bool dockingVertex = (CVoxelCave::DockingVoxel == (VoxelCave->GetVoxel(x_-1,y_,z_)|VoxelCave->GetVoxel(x_,y_-1,z_)
		|VoxelCave->GetVoxel(x_,y_,z_-1)|VoxelCave->GetVoxel(x_-1,y_-1,z_)|VoxelCave->GetVoxel(x_-1,y_,z_-1)
		|VoxelCave->GetVoxel(x_,y_-1,z_-1)|VoxelCave->GetVoxel(x_-1,y_-1,z_-1)|VoxelCave->GetVoxel(x_,y_,z_)) );
	
	// smooth warping if wished, don't smooth warp dockingvertices
	if (SmoothEnabled && !dockingVertex)
	{
		// compute index for the lookup table
		unsigned int indexWarpTable = (VoxelCave->GetVoxel(x_-1,y_-1,z_-1)) | ((VoxelCave->GetVoxel(x_,y_-1,z_-1))<<1)
			| ((VoxelCave->GetVoxel(x_-1,y_,z_-1))<<2) | ((VoxelCave->GetVoxel(x_,y_,z_-1))<<3)
			| ((VoxelCave->GetVoxel(x_-1,y_-1,z_))<<4) | ((VoxelCave->GetVoxel(x_,y_-1,z_))<<5)
			| ((VoxelCave->GetVoxel(x_-1,y_,z_))<<6) | ((VoxelCave->GetVoxel(x_,y_,z_))<<7);

		// read directions from the lookup table
		SVertexWarpDirections warpDirections = VertexWarpDirections[indexWarpTable];		

		// warp with the appropriate warp strenght and direction
		// X-direction
		if (0 == warpDirections.DirectionX)
			deltaX = -WarpStrength + 2*WarpStrength * RandomGenerator->GetRandomNumber01();
		else if (warpDirections.DirectionX > 0)
			deltaX = WarpStrength * RandomGenerator->GetRandomNumber01();
		else
			deltaX = -WarpStrength * RandomGenerator->GetRandomNumber01();

		// Y-direction
		if (0 == warpDirections.DirectionY)
			deltaY = -WarpStrength + 2*WarpStrength * RandomGenerator->GetRandomNumber01();
		else if (warpDirections.DirectionY > 0)
			deltaY = WarpStrength * RandomGenerator->GetRandomNumber01();
		else
			deltaY = -WarpStrength * RandomGenerator->GetRandomNumber01();

		// Z-direction
		if (0 == warpDirections.DirectionZ)
			deltaZ = -WarpStrength + 2*WarpStrength * RandomGenerator->GetRandomNumber01();
		else if (warpDirections.DirectionZ > 0)
			deltaZ = WarpStrength * RandomGenerator->GetRandomNumber01();
		else
			deltaZ = -WarpStrength * RandomGenerator->GetRandomNumber01();
	}
	else // warping without smoothing
	{
		// warping in the interval between -WarpStrength and +WarpStrength
		deltaX = -WarpStrength + 2*WarpStrength * RandomGenerator->GetRandomNumber01();
		deltaY = -WarpStrength + 2*WarpStrength * RandomGenerator->GetRandomNumber01();
		deltaZ = -WarpStrength + 2*WarpStrength * RandomGenerator->GetRandomNumber01();
	}

	// clamping to adjust positioning to prevent intersecting triangles
	// the following must be true:
	// the manhatten distance to the grid origin has to be smaller than 0.5 in each plane

	// compute absolut delta values
	double absDeltaX = abs(deltaX);
	double absDeltaY = abs(deltaY);
	double absDeltaZ = abs(deltaZ);

	// compute case and use appropriate clamping strategy
	if ((absDeltaX+absDeltaY)>MaxClampDistance) // clamp XY-plane
	{
		if ((absDeltaX+absDeltaZ)>MaxClampDistance) // clamp XZ-plane
		{
			if ((absDeltaY+absDeltaZ)>MaxClampDistance) // clamp YZ-plane
			{
				// all 3 planes -> normalize all deltas
				double normalizationFactor = 1.0 / std::max(std::max(absDeltaX+absDeltaY,absDeltaX+absDeltaZ),absDeltaY+absDeltaZ);
				deltaX *= MaxClampDistance * normalizationFactor;
				deltaY *= MaxClampDistance * normalizationFactor;
				deltaZ *= MaxClampDistance * normalizationFactor;
			}
			else
			{
				// 2 planes: XY, XZ -> clamp common delta
				double absNeu = std::min(MaxClampDistance - absDeltaY, MaxClampDistance - absDeltaZ);
				deltaX =  (deltaX>0.0) ? absNeu : -absNeu;
			}
		}
		else
		{
			if ((absDeltaY+absDeltaZ)>MaxClampDistance) // clamp YZ-plane
			{
				// 2 planes: XY, YZ -> clamp common delta
				double absNeu = std::min(MaxClampDistance - absDeltaX, MaxClampDistance - absDeltaZ);
				deltaY =  (deltaY>0.0) ? absNeu : -absNeu;
			}
			else
			{
				// 1 plane: XY -> normalize both affected deltas
				double normalizationFactor = 1.0 / (absDeltaX+absDeltaY);
				deltaX *= MaxClampDistance * normalizationFactor;
				deltaY *= MaxClampDistance * normalizationFactor;
			}
		}
	}
	else
	{
		if ((absDeltaX+absDeltaZ)>MaxClampDistance) // clamp XZ-plane
		{
			if ((absDeltaY+absDeltaZ)>MaxClampDistance) // clamp YZ-plane
			{
				// 2 planes: XZ, YZ -> clamp common delta
				double absNeu = std::min(MaxClampDistance - absDeltaX, MaxClampDistance - absDeltaY);
				deltaZ =  (deltaZ>0.0) ? absNeu : -absNeu;
			}
			else
			{
				// 1 plane: XZ -> normalize both affected deltas
				double normalizationFactor = 1.0 / (absDeltaX+absDeltaZ);
				deltaX *= MaxClampDistance * normalizationFactor;
				deltaZ *= MaxClampDistance * normalizationFactor;
			}
		}
		else
		{
			if ((absDeltaY+absDeltaZ)>MaxClampDistance) // clamp YZ-plane
			{
				// 1 plane: YZ -> normalize both affected deltas
				double normalizationFactor = 1.0 / (absDeltaY+absDeltaZ);
				deltaY *= MaxClampDistance * normalizationFactor;
				deltaZ *= MaxClampDistance * normalizationFactor;
			}
			// else { /* 0 planes -> nothing to do */ }
		}
	}

	// compute resulting position
	irr::core::vector3d<irr::f32> result;
	result.X = static_cast<irr::f32>(x_) + static_cast<irr::f32>(deltaX);
	result.Y = static_cast<irr::f32>(y_) + static_cast<irr::f32>(deltaY);
	result.Z = static_cast<irr::f32>(z_) + static_cast<irr::f32>(deltaZ);

	// return dockingvertex information
	if (dockingVertex)
		markingDockingVertex_ = 1.0f;
	else
		markingDockingVertex_ = -1.0f;

	// return resulting position
	return result;
}

irr::f32 DunGen::CMeshCave::IsBorderVertex(unsigned int x_, unsigned int y_, unsigned int z_, SOctreeNode* octreeNode_)
{
	// vertex is shared by voxel[X-1,Y-1,Z-1] to voxel[X,Y,Z] (8 voxel total)
	// it is tested if the vertex is also used by an adjacent region

	// test the 4 voxels in the adjacent region
	if (x_ == octreeNode_->BorderMinX)
	{
		// is one border voxel of the adjacent region 1 and an arbitrary adjacent border voxel 0?
		if (1 == VoxelCave->GetVoxel(x_-1,y_,z_))
			if (0 == (VoxelCave->GetVoxel(x_,y_,z_) * VoxelCave->GetVoxel(x_-1,y_-1,z_)
				* VoxelCave->GetVoxel(x_-1,y_,z_-1)))
				return 1.0f;

		if (1 == VoxelCave->GetVoxel(x_-1,y_-1,z_))
			if (0 == (VoxelCave->GetVoxel(x_,y_-1,z_) * VoxelCave->GetVoxel(x_-1,y_,z_)
				* VoxelCave->GetVoxel(x_-1,y_-1,z_-1)))				
				return 1.0f;

		if (1 == VoxelCave->GetVoxel(x_-1,y_,z_-1))
			if (0 == (VoxelCave->GetVoxel(x_,y_,z_-1) * VoxelCave->GetVoxel(x_-1,y_-1,z_-1)
				* VoxelCave->GetVoxel(x_-1,y_,z_)))
				return 1.0f;

		if (1 == VoxelCave->GetVoxel(x_-1,y_-1,z_-1))
			if (0 == (VoxelCave->GetVoxel(x_,y_-1,z_-1) * VoxelCave->GetVoxel(x_-1,y_,z_-1)
				* VoxelCave->GetVoxel(x_-1,y_-1,z_)))
				return 1.0f;
	}
	// test the 4 voxels in the adjacent region
	else if (x_ == octreeNode_->BorderMaxX+1)
	{
		// is one border voxel of the adjacent region 1 and an arbitrary adjacent border voxel 0?
		if (1 == VoxelCave->GetVoxel(x_,y_,z_))
			if (0 == (VoxelCave->GetVoxel(x_-1,y_,z_) * VoxelCave->GetVoxel(x_,y_-1,z_)
				* VoxelCave->GetVoxel(x_,y_,z_-1)))
				return 1.0f;

		if (1 == VoxelCave->GetVoxel(x_,y_-1,z_))
			if (0 == (VoxelCave->GetVoxel(x_-1,y_-1,z_) * VoxelCave->GetVoxel(x_,y_,z_)
				* VoxelCave->GetVoxel(x_,y_-1,z_-1)))
				return 1.0f;

		if (1 == VoxelCave->GetVoxel(x_,y_,z_-1))
			if (0 == (VoxelCave->GetVoxel(x_-1,y_,z_-1) * VoxelCave->GetVoxel(x_,y_-1,z_-1)
				* VoxelCave->GetVoxel(x_,y_,z_)))
				return 1.0f;

		if (1 == VoxelCave->GetVoxel(x_,y_-1,z_-1))
			if (0 == (VoxelCave->GetVoxel(x_-1,y_-1,z_-1) * VoxelCave->GetVoxel(x_,y_,z_-1)
				* VoxelCave->GetVoxel(x_,y_-1,z_)))
				return 1.0f;
	}
	// test the 4 voxels in the adjacent region
	if (y_ == octreeNode_->BorderMinY)
	{
		// is one border voxel of the adjacent region 1 and an arbitrary adjacent border voxel 0?
		if (1 == VoxelCave->GetVoxel(x_,y_-1,z_))
			if (0 == (VoxelCave->GetVoxel(x_-1,y_-1,z_) * VoxelCave->GetVoxel(x_,y_,z_)
				* VoxelCave->GetVoxel(x_,y_-1,z_-1)))
				return 1.0f;

		if (1 == VoxelCave->GetVoxel(x_-1,y_-1,z_))
			if (!(VoxelCave->GetVoxel(x_,y_-1,z_) * VoxelCave->GetVoxel(x_-1,y_,z_)
				* VoxelCave->GetVoxel(x_-1,y_-1,z_-1)))
				return 1.0f;

		if (1 == VoxelCave->GetVoxel(x_,y_-1,z_-1))
			if (0 == (VoxelCave->GetVoxel(x_-1,y_-1,z_-1) * VoxelCave->GetVoxel(x_,y_,z_-1)
				* VoxelCave->GetVoxel(x_,y_-1,z_)))
				return 1.0f;

		if (1 == VoxelCave->GetVoxel(x_-1,y_-1,z_-1))
			if (0 == (VoxelCave->GetVoxel(x_,y_-1,z_-1) * VoxelCave->GetVoxel(x_-1,y_,z_-1)
				* VoxelCave->GetVoxel(x_-1,y_-1,z_)))
				return 1.0f;
	}
	// test the 4 voxels in the adjacent region
	else if (y_ == octreeNode_->BorderMaxY+1)
	{
		// is one border voxel of the adjacent region 1 and an arbitrary adjacent border voxel 0?
		if (1 == VoxelCave->GetVoxel(x_,y_,z_))
			if (0 == (VoxelCave->GetVoxel(x_-1,y_,z_) * VoxelCave->GetVoxel(x_,y_-1,z_)
				* VoxelCave->GetVoxel(x_,y_,z_-1)))
				return 1.0f;

		if (1 == VoxelCave->GetVoxel(x_-1,y_,z_))
			if (0 == (VoxelCave->GetVoxel(x_,y_,z_) * VoxelCave->GetVoxel(x_-1,y_-1,z_)
				* VoxelCave->GetVoxel(x_-1,y_,z_-1)))
				return 1.0f;

		if (1 == VoxelCave->GetVoxel(x_,y_,z_-1))
			if (0 == (VoxelCave->GetVoxel(x_-1,y_,z_-1) * VoxelCave->GetVoxel(x_,y_-1,z_-1)
				* VoxelCave->GetVoxel(x_,y_,z_)))
				return 1.0f;

		if (1 == VoxelCave->GetVoxel(x_-1,y_,z_-1))
			if (0 == (VoxelCave->GetVoxel(x_,y_,z_-1) * VoxelCave->GetVoxel(x_-1,y_-1,z_-1)
				* VoxelCave->GetVoxel(x_-1,y_,z_)))
				return 1.0f;
	}
	// test the 4 voxels in the adjacent region
	if (z_ == octreeNode_->BorderMinZ)
	{
		// is one border voxel of the adjacent region 1 and an arbitrary adjacent border voxel 0?
		if (1 == VoxelCave->GetVoxel(x_,y_,z_-1))
			if (0 == (VoxelCave->GetVoxel(x_-1,y_,z_-1) * VoxelCave->GetVoxel(x_,y_-1,z_-1)
				* VoxelCave->GetVoxel(x_,y_,z_)))
				return 1.0f;

		if (1 == VoxelCave->GetVoxel(x_-1,y_,z_-1))
			if (0 == (VoxelCave->GetVoxel(x_,y_,z_-1) * VoxelCave->GetVoxel(x_-1,y_-1,z_-1)
				* VoxelCave->GetVoxel(x_-1,y_,z_)))
				return 1.0f;

		if (1 == VoxelCave->GetVoxel(x_,y_-1,z_-1))
			if (0 == (VoxelCave->GetVoxel(x_-1,y_-1,z_-1) * VoxelCave->GetVoxel(x_,y_,z_-1)
				* VoxelCave->GetVoxel(x_,y_-1,z_)))
				return 1.0f;

		if (1 == VoxelCave->GetVoxel(x_-1,y_-1,z_-1))
			if (0 == (VoxelCave->GetVoxel(x_,y_-1,z_-1) * VoxelCave->GetVoxel(x_-1,y_,z_-1)
				* VoxelCave->GetVoxel(x_-1,y_-1,z_)))
				return 1.0f;
	}
	// test the 4 voxels in the adjacent region
	else if (z_ == octreeNode_->BorderMaxZ+1)
	{
		// is one border voxel of the adjacent region 1 and an arbitrary adjacent border voxel 0?
		if (1 == VoxelCave->GetVoxel(x_,y_,z_))
			if (0 == (VoxelCave->GetVoxel(x_-1,y_,z_) * VoxelCave->GetVoxel(x_,y_-1,z_)
				* VoxelCave->GetVoxel(x_,y_,z_-1)))
				return 1.0f;

		if (1 == VoxelCave->GetVoxel(x_-1,y_,z_))
			if (0 == (VoxelCave->GetVoxel(x_,y_,z_) * VoxelCave->GetVoxel(x_-1,y_-1,z_)
				* VoxelCave->GetVoxel(x_-1,y_,z_-1)))
				return 1.0f;

		if (1 == VoxelCave->GetVoxel(x_,y_-1,z_))
			if (0 == (VoxelCave->GetVoxel(x_-1,y_-1,z_) * VoxelCave->GetVoxel(x_,y_,z_)
				* VoxelCave->GetVoxel(x_,y_-1,z_-1)))
				return 1.0f;

		if (1 == VoxelCave->GetVoxel(x_-1,y_-1,z_))
			if (0 == (VoxelCave->GetVoxel(x_,y_-1,z_) * VoxelCave->GetVoxel(x_-1,y_,z_)
				* VoxelCave->GetVoxel(x_-1,y_-1,z_-1)))
				return 1.0f;
	}

	// no border vertex
	return -1.0f;
}