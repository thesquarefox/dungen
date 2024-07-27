// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#include "Adapter.h"
#include "Corridor.h"
#include "DockingSite.h"

// ======================================================
// initialization & de-initialization & read values
// ======================================================

// set precision
const double DunGen::CCorridor::Precision = 0.00001;

// set height axis (Y axis here)
const irr::core::vector3d<double> DunGen::CCorridor::UpStandard = irr::core::vector3d<double>(0,1,0);

DunGen::CCorridor::CCorridor(const SCorridorProfile& profile_, const SDockingSite& dockingSite0_, const SDockingSite& dockingSite1_,
	const irr::core::vector3d<double>& position0_, const irr::core::vector3d<double>& position1_,
	const irr::core::vector3d<double>& derivation0_, const irr::core::vector3d<double>& derivation1_,
	double distance_, double distanceTextureYPerDistance1_)
{
	// store values
	Position[0] = position0_;
	Position[1] = position1_;
	Derivation[0] = derivation0_;
	Derivation[1] = -derivation1_; // reverse derivation, so it is now in correct spline direction
	// compute the spline coefficients
	ComputeResultingCoefficients();
		
	// reverse order of the docking site at P1
	SDockingSite dockingSite1rev;
	for (unsigned int i=1; i<=dockingSite1_.Point.size(); ++i)
		dockingSite1rev.Point.push_back(dockingSite1_.Point[dockingSite1_.Point.size()-i]);
	dockingSite1rev.Normal = -dockingSite1_.Normal;
	dockingSite1rev.Center = dockingSite1_.Center;
	dockingSite1rev.Extend = dockingSite1_.Extend;
	
	// create the corridor
	double lastTextureY = CreateCorridor(profile_,dockingSite0_,dockingSite1rev,distance_,distanceTextureYPerDistance1_);

	// create adapters
	Adapter[0] = new CAdapter(0,Position[0],Derivation[0], profile_, distanceTextureYPerDistance1_, 0.0, dockingSite0_);
	Adapter[1] = new CAdapter(1,Position[1],Derivation[1], profile_, distanceTextureYPerDistance1_, lastTextureY, dockingSite1rev);

	// test for visibility
	DefinitivelySightBlocking = NVisibilityTest::VisibilityTest(ViewTestRegions, profile_, dockingSite0_, dockingSite1rev);
}

DunGen::CCorridor::~CCorridor()
{
	// drop corridor mesh
	MeshCorridor->drop();

	// delete adapters
	delete Adapter[0];
	delete Adapter[1];

	// delete detail objects
	for (unsigned int i=0; i<DetailObjects.size(); ++i)
		delete DetailObjects[i];
}

void DunGen::CCorridor::ComputeResultingCoefficients()
{
	// compute position coefficients
	PositionCoefficients[0] = 2 * Position[0] - 2 * Position[1] + Derivation[0] + Derivation[1];
	PositionCoefficients[1] = -3 * Position[0] + 3 * Position[1] - 2 * Derivation[0] - Derivation[1];
	PositionCoefficients[2] = Derivation[0];
	PositionCoefficients[3] = Position[0];

	// compute derivation coefficients
	DerivationCoefficients[0] = 6 * Position[0] -6 * Position[1] + 3 * Derivation[0] + 3 * Derivation[1];
	DerivationCoefficients[1] = -6 * Position[0] + 6 * Position[1] - 4 * Derivation[0] - 2 * Derivation[1];
	DerivationCoefficients[2] = Derivation[0];
}

bool DunGen::CCorridor::GetDefinitivelySightBlocking()
{
	return DefinitivelySightBlocking;
}

irr::scene::SMesh* DunGen::CCorridor::GetMesh()
{
	return MeshCorridor;
}

irr::scene::SMesh* DunGen::CCorridor::GetMeshAdapter(unsigned int i_)
{
	return Adapter[i_]->GetMesh();
}

DunGen::CCorridor::SDetailObject* DunGen::CCorridor::GetDetailObject(unsigned int i_)
{
	if (DetailObjects.size() <= i_)
		return NULL;
	return DetailObjects[i_];
}