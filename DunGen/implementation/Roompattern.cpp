// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#include "Roompattern.h"
#include "Helperfunctions.h"
#include <algorithm>
#include <iostream>
#include <stack>

// ======================================================
// struct functions
// ======================================================

void DunGen::CRoomPattern::SAdjacencyList::AddAdjacentVertex(unsigned int index_)
{
	// search if already present
	for (unsigned int i=0; i<AdjacentVertices.size(); ++i)
		if (index_ == AdjacentVertices[i].Index)
		{
			// multiple entry
			AdjacentVertices[i].Multiple = true;	
			return;
		}

	// not present -> add
	SAdjacentVertex newAdjacentVertex;
	newAdjacentVertex.Index = index_;
	newAdjacentVertex.Multiple = false;
	AdjacentVertices.push_back(newAdjacentVertex);
}

// ======================================================
// constructor and destructor
// ======================================================

DunGen::CRoomPattern::CRoomPattern(irr::IrrlichtDevice* irrDevice_, const irr::io::path& filename_)
{
	// get filesystem			
	irr::io::IFileSystem* fileSystem = irrDevice_->getFileSystem();
	// create own scenemanager
	PrivateSceneManager =  irrDevice_->getSceneManager()->createNewSceneManager(false);

	// save working directory path
	irr::io::path workingDirectory = fileSystem->getWorkingDirectory();

	// change working directory to the path of the _Filename
	// so that all file references there can be loaded correctly
	fileSystem->changeWorkingDirectoryTo(fileSystem->getFileDir(filename_));
	
	// read file
	PrivateSceneManager->loadScene(fileSystem->getFileBasename(filename_));

	// compute docking sites
	int i = 0;
	while (ComputeDockingSite(i))
		++i;

	// restore working directory path
	fileSystem->changeWorkingDirectoryTo(workingDirectory);
}

DunGen::CRoomPattern::~CRoomPattern()
{	
	// delete own scenemanager and scene
	PrivateSceneManager->drop();
}

// ======================================================
// create room instance
// ======================================================

void DunGen::CRoomPattern::AddRoomInstance(irr::scene::ISceneNode* parentNode_, irr::scene::ISceneManager* sceneManager_)
{	
	// copy all childs from the root node of the own scene graph
	// under the specified node of the specified scenegraph
	irr::core::list<irr::scene::ISceneNode*> const & childNode = PrivateSceneManager->getRootSceneNode()->getChildren();
    for (irr::core::list<irr::scene::ISceneNode*>::ConstIterator child(childNode.begin()); child != childNode.end(); ++child)
		(*child)->clone(parentNode_,sceneManager_);
}

// ======================================================
// compute docking site
// ======================================================

bool DunGen::CRoomPattern::ComputeDockingSite(unsigned int index_)
{
	// maximum are 1000 docking sites
	if (999 < index_) return false;

	// compute docking site name: 'dockingsite_XXX' with XXX as decimal number from 000 to 999
	char dockingSiteName[255];
	snprintf(dockingSiteName, 255, "dockingsite_%03u", index_);
	irr::scene::IMeshSceneNode* dockingSiteMeshNode = (irr::scene::IMeshSceneNode*) PrivateSceneManager->getSceneNodeFromName(dockingSiteName);

	// no docking site?
	if (!dockingSiteMeshNode) return false;

	// docking site may only have one mesh buffer (e.g. .obj-file mesh with one group)
	irr::scene::IMeshBuffer* dockingSiteMeshBuffer = dockingSiteMeshNode->getMesh()->getMeshBuffer(0);

	// ~~~~~~~~~~~~~~~~~~~~~~
	// create adjacency lists
	// ~~~~~~~~~~~~~~~~~~~~~~

	SAdjacencyList** adjacencyLists = new SAdjacencyList*[dockingSiteMeshBuffer->getVertexCount()];
	for (unsigned int i = 0; i<dockingSiteMeshBuffer->getVertexCount(); ++i)
		adjacencyLists[i] = new SAdjacencyList();

	// store adjacencies: 3 edges per triangle, 2 adjacencies per edge
	for (unsigned int i=0; i<dockingSiteMeshBuffer->getIndexCount(); i+=3)
	{		
		adjacencyLists[dockingSiteMeshBuffer->getIndices()[i]]->AddAdjacentVertex(dockingSiteMeshBuffer->getIndices()[i+1]);
		adjacencyLists[dockingSiteMeshBuffer->getIndices()[i]]->AddAdjacentVertex(dockingSiteMeshBuffer->getIndices()[i+2]);
		adjacencyLists[dockingSiteMeshBuffer->getIndices()[i+1]]->AddAdjacentVertex(dockingSiteMeshBuffer->getIndices()[i]);
		adjacencyLists[dockingSiteMeshBuffer->getIndices()[i+1]]->AddAdjacentVertex(dockingSiteMeshBuffer->getIndices()[i+2]);
		adjacencyLists[dockingSiteMeshBuffer->getIndices()[i+2]]->AddAdjacentVertex(dockingSiteMeshBuffer->getIndices()[i]);
		adjacencyLists[dockingSiteMeshBuffer->getIndices()[i+2]]->AddAdjacentVertex(dockingSiteMeshBuffer->getIndices()[i+1]);
	}

	// ~~~~~~~~~~~~~~~~~~~~~
	// computer border curve
	// ~~~~~~~~~~~~~~~~~~~~~

	unsigned int startVertex = dockingSiteMeshBuffer->getVertexCount();
	unsigned int actualVertex = 0;
	unsigned int tempIndex;

	// search for starting border vertex
	while (actualVertex<startVertex)
	{
		// search for not multiple edge (condition for border vertex)
		tempIndex = 0;
		while (adjacencyLists[actualVertex]->AdjacentVertices.size() > tempIndex)
		{
			if (!adjacencyLists[actualVertex]->AdjacentVertices[tempIndex].Multiple)
			{
				startVertex = actualVertex;
				break;
			}
			++tempIndex;
		}		
		++actualVertex;
	}

	// invalid mesh (no border found)?
	if (dockingSiteMeshBuffer->getVertexCount() == startVertex)
		return false;

	SDockingSite newDockingSite;
	DockingSite.push_back(newDockingSite);
	// add start vertex	
	DockingSite[index_].Point.push_back(vec3D(dockingSiteMeshBuffer->getPosition(startVertex)));
	// select next vertex (tempIndex is already a non multiple entry here)
	actualVertex = adjacencyLists[startVertex]->AdjacentVertices[tempIndex].Index;
	unsigned int lastVertex = startVertex;

	irr::core::vector3d<double> tempVec;
	double maxDistanceZpositive = 0.0;
	double maxDistanceZnegative = 0.0;
	// walk a closed loop
	while (actualVertex != startVertex)
	{
		// add the vertex
		tempVec = vec3D(dockingSiteMeshBuffer->getPosition(actualVertex));
		DockingSite[index_].Point.push_back(tempVec);

		// maximum of a vertex distance to the plane
		maxDistanceZpositive = std::max(maxDistanceZpositive,tempVec.Z);
		maxDistanceZnegative = std::max(maxDistanceZnegative,-tempVec.Z);

		// find next vertex
		tempIndex = 0;
		while ((adjacencyLists[actualVertex]->AdjacentVertices[tempIndex].Multiple)
			|| (lastVertex == adjacencyLists[actualVertex]->AdjacentVertices[tempIndex].Index)) 
		{
			++tempIndex;

			// invalid mesh (no 2 neighbors for each border vertex)?
			if (tempIndex == adjacencyLists[actualVertex]->AdjacentVertices.size())
			{
				DockingSite.pop_back();
				return false;
			}
		}

		// step on to next vertex
		lastVertex = actualVertex;
		actualVertex = adjacencyLists[actualVertex]->AdjacentVertices[tempIndex].Index;

		// invalid mesh (too large or no correct border)?
		if (DockingSite[index_].Point.size()>999)
		{
			DockingSite.pop_back();
			return false;
		}
	}

	// ~~~~~~~~~~~~~~~~~~~~~~
	// delete adjacency lists
	// ~~~~~~~~~~~~~~~~~~~~~~

	for (unsigned int i = 0; i<dockingSiteMeshBuffer->getVertexCount(); ++i)
		delete adjacencyLists[i];
	delete adjacencyLists;

	// ~~~~~~~~~~~~~~~~~~~~~
	// finalize docking site
	// ~~~~~~~~~~~~~~~~~~~~~

	// set center
	DockingSite[index_].Center = irr::core::vector3d<double>(0.0,0.0,0.0);
	// set normal: Z-axis of the dockingSite mesh
	DockingSite[index_].Normal = irr::core::vector3d<double>(0.0,0.0,1.0);
	// set extend
	DockingSite[index_].Extend = 1.0;

	// compute a helper normal: used to determine the order of the vertices
	// the polar angles of all vertices, projected in the XY-plane, have to be monotone ascending or descending
	irr::core::vector3d<double> tempNormal =
		irr::core::vector3d<double>(DockingSite[index_].Point[1].X,DockingSite[index_].Point[1].Y,0.0)
		.crossProduct(irr::core::vector3d<double>(DockingSite[index_].Point[0].X,DockingSite[index_].Point[0].Y,0.0));

	// actualize absolute transformation of the docking site node
	std::stack<irr::scene::ISceneNode*> updateTransformationStack;
	irr::scene::ISceneNode* tempSceneNode = dockingSiteMeshNode;
	while (tempSceneNode != PrivateSceneManager->getRootSceneNode())
	{
		updateTransformationStack.push(tempSceneNode);
		tempSceneNode = tempSceneNode->getParent();
	}
	while (!updateTransformationStack.empty())
	{
		tempSceneNode = updateTransformationStack.top();
		tempSceneNode->updateAbsolutePosition();
		updateTransformationStack.pop();
	}
	// read the result
	SMatrix4D absoluteTransformation(dockingSiteMeshNode->getAbsoluteTransformation());
	// transform the docking site
	DockingSite[index_].TransformDockingSite(absoluteTransformation);
	
	// invert normal?
	// normal has to point away from the center of the scene (0,0,0)
	// (which must be not equal to the center/position of the docking site)
	if (DockingSite[index_].Center.dotProduct(DockingSite[index_].Normal) < 0.0)
	{
		DockingSite[index_].Normal = -DockingSite[index_].Normal;
		DockingSite[index_].Extend *= maxDistanceZnegative;
	}
	else
		DockingSite[index_].Extend *= maxDistanceZpositive;

	// transform helper normal
	absoluteTransformation.MultiplyWith1x3_0(tempNormal);
	tempNormal.normalize();

	// revert order of the vertices? -> if both normals do not point in the same direction
	if (tempNormal.dotProduct(DockingSite[index_].Normal) < 0.0)
		std::reverse(DockingSite[index_].Point.begin(), DockingSite[index_].Point.end());

	// all necessary data was retrieved -> delete docking site scene node
	dockingSiteMeshNode->remove();

	return true;
}
unsigned int DunGen::CRoomPattern::GetDockingSiteNumber()
{
	return DockingSite.size();
}

DunGen::SDockingSite DunGen::CRoomPattern::GetDockingSite(unsigned int index_)
{
	return DockingSite[index_];
}