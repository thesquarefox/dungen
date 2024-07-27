// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#include "Corridor.h"
#include "DockingSite.h"
#include "VisibilityTest.h"

// ======================================================
// constructor
// ======================================================

DunGen::NVisibilityTest::SViewingVolume::SViewingVolume(const SViewRegionOfInterest* region1_, const SViewRegionOfInterest* region2_)
{
	// center of the projection plane
	PointOfReference = region1_->Position;
	// vectors to span the projection plane
	irr::core::vector3d<double> tmpVec1 = (region2_->Position - region1_->Position).normalize();	// front
	Left = (tmpVec1.crossProduct(CCorridor::UpStandard)).normalize();
	Up = Left.crossProduct(tmpVec1);
	irr::core::vector2d<double> tmpVec2;

	// BB (bounding box) 1 projection
	// project all 4 corner points and add then to the viewing volume BB
	tmpVec1 = -region1_->ProfileBB.UpperLeftCorner.X * region1_->Left + region1_->ProfileBB.UpperLeftCorner.Y * region1_->Up;	// as 3D-vector
	tmpVec2.X = -tmpVec1.dotProduct(Left);	// projection
	tmpVec2.Y = tmpVec1.dotProduct(Up);
	ViewingVolumeBB.UpperLeftCorner = tmpVec2; // first point of the VV-BB
	ViewingVolumeBB.LowerRightCorner = tmpVec2;

	tmpVec1 = -region1_->ProfileBB.LowerRightCorner.X * region1_->Left + region1_->ProfileBB.LowerRightCorner.Y * region1_->Up;	// as 3D-vector
	tmpVec2.X = -tmpVec1.dotProduct(Left);	// projection
	tmpVec2.Y = tmpVec1.dotProduct(Up);
	ViewingVolumeBB.addInternalPoint(tmpVec2);	// add point

	tmpVec1 = -region1_->ProfileBB.UpperLeftCorner.X * region1_->Left + region1_->ProfileBB.LowerRightCorner.Y * region1_->Up;	// as 3D-vector
	tmpVec2.X = -tmpVec1.dotProduct(Left);	// projection
	tmpVec2.Y = tmpVec1.dotProduct(Up);
	ViewingVolumeBB.addInternalPoint(tmpVec2);	// add point

	tmpVec1 = -region1_->ProfileBB.LowerRightCorner.X * region1_->Left + region1_->ProfileBB.UpperLeftCorner.Y * region1_->Up;	// as 3D-vector
	tmpVec2.X = -tmpVec1.dotProduct(Left);	// projection
	tmpVec2.Y = tmpVec1.dotProduct(Up);
	ViewingVolumeBB.addInternalPoint(tmpVec2);	// add point

	// BB 2 projection
	// project all 4 corner points and add then to the viewing volume BB
	tmpVec1 = -region2_->ProfileBB.UpperLeftCorner.X * region2_->Left + region2_->ProfileBB.UpperLeftCorner.Y * region2_->Up;	// as 3D-vector
	tmpVec2.X = -tmpVec1.dotProduct(Left);	// projection
	tmpVec2.Y = tmpVec1.dotProduct(Up);
	ViewingVolumeBB.addInternalPoint(tmpVec2);	// add point

	tmpVec1 = -region2_->ProfileBB.LowerRightCorner.X * region2_->Left + region2_->ProfileBB.LowerRightCorner.Y * region2_->Up;	// as 3D-vector
	tmpVec2.X = -tmpVec1.dotProduct(Left);	// projection
	tmpVec2.Y = tmpVec1.dotProduct(Up);
	ViewingVolumeBB.addInternalPoint(tmpVec2);	// add point

	tmpVec1 = -region2_->ProfileBB.UpperLeftCorner.X * region2_->Left + region2_->ProfileBB.LowerRightCorner.Y * region2_->Up;	// as 3D-vector
	tmpVec2.X = -tmpVec1.dotProduct(Left);	// projection
	tmpVec2.Y = tmpVec1.dotProduct(Up);
	ViewingVolumeBB.addInternalPoint(tmpVec2);	// add point

	tmpVec1 = -region2_->ProfileBB.LowerRightCorner.X * region2_->Left + region2_->ProfileBB.UpperLeftCorner.Y * region2_->Up;	// as 3D-vector
	tmpVec2.X = -tmpVec1.dotProduct(Left);	// projection
	tmpVec2.Y = tmpVec1.dotProduct(Up);
	ViewingVolumeBB.addInternalPoint(tmpVec2);	/// add point
}
// ======================================================
// visibility test
// ======================================================

bool DunGen::NVisibilityTest::IsSightBlocking(const SViewingVolume* viewingVolume_, const SViewRegionOfInterest* regionOfInterest_)
{
	irr::core::vector2d<double> tmpVec1;
	irr::core::vector3d<double> tmpVec2;

	// roiBB ... projected bounding box of the region of interest
	irr::core::rect<double> roiBB;
		
	// project BB RegionOfInterest onto plane of the ViewingVolume
	tmpVec2 = -regionOfInterest_->ProfileBB.UpperLeftCorner.X * regionOfInterest_->Left + regionOfInterest_->ProfileBB.UpperLeftCorner.Y * regionOfInterest_->Up;	// as 3D-vector
	tmpVec2 += regionOfInterest_->Position - viewingVolume_->PointOfReference;
	tmpVec1.X = -tmpVec2.dotProduct(viewingVolume_->Left);	// projection
	tmpVec1.Y = tmpVec2.dotProduct(viewingVolume_->Up);
	roiBB.UpperLeftCorner = tmpVec1;	// first point of the roiBB
	roiBB.LowerRightCorner = tmpVec1;

	tmpVec2 = -regionOfInterest_->ProfileBB.LowerRightCorner.X * regionOfInterest_->Left + regionOfInterest_->ProfileBB.LowerRightCorner.Y * regionOfInterest_->Up;	// as 3D-vector
	tmpVec2 += regionOfInterest_->Position - viewingVolume_->PointOfReference;
	tmpVec1.X = -tmpVec2.dotProduct(viewingVolume_->Left);	// projection
	tmpVec1.Y = tmpVec2.dotProduct(viewingVolume_->Up);
	roiBB.addInternalPoint(tmpVec1);	// add point

	tmpVec2 = -regionOfInterest_->ProfileBB.UpperLeftCorner.X * regionOfInterest_->Left + regionOfInterest_->ProfileBB.LowerRightCorner.Y * regionOfInterest_->Up;	// as 3D-vector
	tmpVec2 += regionOfInterest_->Position - viewingVolume_->PointOfReference;
	tmpVec1.X = -tmpVec2.dotProduct(viewingVolume_->Left);	// projection
	tmpVec1.Y = tmpVec2.dotProduct(viewingVolume_->Up);
	roiBB.addInternalPoint(tmpVec1);	// add point

	tmpVec2 = -regionOfInterest_->ProfileBB.LowerRightCorner.X * regionOfInterest_->Left + regionOfInterest_->ProfileBB.UpperLeftCorner.Y * regionOfInterest_->Up;	// as 3D-vector
	tmpVec2 += regionOfInterest_->Position - viewingVolume_->PointOfReference;
	tmpVec1.X = -tmpVec2.dotProduct(viewingVolume_->Left);	// projection
	tmpVec1.Y = tmpVec2.dotProduct(viewingVolume_->Up);
	roiBB.addInternalPoint(tmpVec1);	// add point
	
	// test for intersection: if is not intersecting, no view ray can pass the viewing volume
	// so this would be sight blocking
	return (!roiBB.isRectCollided(viewingVolume_->ViewingVolumeBB));
}

bool DunGen::NVisibilityTest::VisibilityTest(std::vector<SViewRegionOfInterest>& viewTestRegions_, const SCorridorProfile& profile_,
	const SDockingSite& dockingSite0_, const SDockingSite& dockingSite1_)
{
	// dimensions of the corridor profile
	irr::core::rect<double> corridorProfileBB;
	// regions of interests of docking sites
	SViewRegionOfInterest rOIDockingSite[2];
	
	// compute corridor profile dimensions
	corridorProfileBB.UpperLeftCorner.X = 0.0;
	corridorProfileBB.UpperLeftCorner.Y = 0.0;
	corridorProfileBB.LowerRightCorner.X = 0.0;
	corridorProfileBB.LowerRightCorner.Y = 0.0;
	for (unsigned int i=0; i<profile_.Point.size(); i++)
	{
		// upper left: smaller values
		if (profile_.Point[i].X < corridorProfileBB.UpperLeftCorner.X)
			corridorProfileBB.UpperLeftCorner.X = profile_.Point[i].X;
		if (profile_.Point[i].Y < corridorProfileBB.UpperLeftCorner.Y)
			corridorProfileBB.UpperLeftCorner.Y = profile_.Point[i].Y;
		
		// lower right: larger values
		if (profile_.Point[i].X > corridorProfileBB.LowerRightCorner.X)
			corridorProfileBB.LowerRightCorner.X = profile_.Point[i].X;
		if (profile_.Point[i].Y > corridorProfileBB.LowerRightCorner.Y)
			corridorProfileBB.LowerRightCorner.Y = profile_.Point[i].Y;
	}

	// transfer spline profile BB to the already computed view test regions
	for (unsigned int i=0; i<viewTestRegions_.size(); ++i)
		viewTestRegions_[i].ProfileBB = corridorProfileBB;

	// compute docking site ROIs
	irr::core::vector3d<double> front;
	const SDockingSite* dockingSite[2];
	dockingSite[0] = &dockingSite0_;
	dockingSite[1] = &dockingSite1_;
	irr::core::vector2d<double> tmpVec2D;

	for (unsigned int i=0; i<2; i++)
	{
		front = dockingSite[i]->Normal;
		rOIDockingSite[i].Left = (front.crossProduct(CCorridor::UpStandard)).normalize();
		rOIDockingSite[i].Up = (rOIDockingSite[i].Left).crossProduct(front);
		// position is at the inner end of the docking site
		if (0==i)
			rOIDockingSite[i].Position = dockingSite[i]->Center + dockingSite[i]->Extend*dockingSite[i]->Normal;
		else
			rOIDockingSite[i].Position = dockingSite[i]->Center - dockingSite[i]->Extend*dockingSite[i]->Normal;
		
		rOIDockingSite[i].ProfileBB.UpperLeftCorner.X = 0.0;
		rOIDockingSite[i].ProfileBB.UpperLeftCorner.Y = 0.0;
		rOIDockingSite[i].ProfileBB.LowerRightCorner.X = 0.0;
		rOIDockingSite[i].ProfileBB.LowerRightCorner.Y = 0.0;

		for (unsigned int j=0; j<dockingSite[i]->Point.size(); ++j)
		{
			// compute dimension: points have to be projected
			tmpVec2D = projectVectorToPlane(dockingSite[i]->Point[j] - dockingSite[i]->Center, rOIDockingSite[i].Left, rOIDockingSite[i].Up);

			// upper left: smaller values
			if (tmpVec2D.X < rOIDockingSite[i].ProfileBB.UpperLeftCorner.X)
				rOIDockingSite[i].ProfileBB.UpperLeftCorner.X = tmpVec2D.X;

			if (tmpVec2D.Y < rOIDockingSite[i].ProfileBB.UpperLeftCorner.Y)
				rOIDockingSite[i].ProfileBB.UpperLeftCorner.Y = tmpVec2D.Y;
		
			// lower right: larger values
			if (tmpVec2D.X > rOIDockingSite[i].ProfileBB.LowerRightCorner.X)
				rOIDockingSite[i].ProfileBB.LowerRightCorner.X = tmpVec2D.X;

			if (tmpVec2D.Y > rOIDockingSite[i].ProfileBB.LowerRightCorner.Y)
				rOIDockingSite[i].ProfileBB.LowerRightCorner.Y = tmpVec2D.Y;
		}

		// because adapter use both, the corridor profile and the docking site: merge both BB
		rOIDockingSite[i].ProfileBB.addInternalPoint(corridorProfileBB.LowerRightCorner);
		rOIDockingSite[i].ProfileBB.addInternalPoint(corridorProfileBB.UpperLeftCorner);
	}

	// aggregate viewtest regions
	std::vector<SViewRegionOfInterest*> viewTestRegionsComplete;
	viewTestRegionsComplete.push_back(&rOIDockingSite[0]);
	for (unsigned int i=0; i<viewTestRegions_.size(); ++i)
		viewTestRegionsComplete.push_back(&viewTestRegions_[i]);
	viewTestRegionsComplete.push_back(&rOIDockingSite[1]);

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// create viewing volumes
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	std::vector<SViewingVolume*> viewingVolumes;
	// comparing indices (all test objects have to be between both ends of the viewing volume)
	std::vector<unsigned int> testIndexStart;
	std::vector<unsigned int> testIndexEnd;
	for (unsigned int i=0; i<viewTestRegionsComplete.size(); ++i)
		for (unsigned int j=i+2; j<viewTestRegionsComplete.size(); ++j)	// minimum one region between
		{
			viewingVolumes.push_back(new SViewingVolume(viewTestRegionsComplete[i], viewTestRegionsComplete[j]) );
			// all regions between the "parent" regions have to be tested
			testIndexStart.push_back(i+1);
			testIndexEnd.push_back(j-1);
		}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// test
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~	
	bool definitivelySightBlocking = false;

	for (unsigned int i=0; i<viewingVolumes.size(); ++i)
		for (unsigned int j=testIndexStart[i]; j<=testIndexEnd[i]; ++j)
			if (IsSightBlocking(viewingVolumes[i],viewTestRegionsComplete[j]))
			{
				definitivelySightBlocking = true;
				i = viewingVolumes.size();
				break;
			}

	// delete the temporally created objects
	for (unsigned int i=0; i<viewingVolumes.size(); ++i)
		delete viewingVolumes[i];

	return definitivelySightBlocking;
}