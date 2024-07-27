// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#include "Corridor.h"
#include "DockingSite.h"
#include "Helperfunctions.h"
#include "RandomGenerator.h"

// ======================================================
// computation of t, position and derivation
// ======================================================

inline irr::core::vector3d<double> DunGen::CCorridor::ComputePosition(double t_)
{
	return (t_*t_*t_* PositionCoefficients[0] + t_*t_* PositionCoefficients[1] + t_* PositionCoefficients[2] + PositionCoefficients[3]);
}

inline irr::core::vector3d<double> DunGen::CCorridor::ComputeDerivation(double t_)
{
	return (t_*t_* DerivationCoefficients[0] + t_* DerivationCoefficients[1] + DerivationCoefficients[2]);
}

inline irr::core::vector2d<double> DunGen::CCorridor::ComputeT(
	const irr::core::vector3d<double>& lastPosition_, double lastT_, double distance_)
{
	// computation of next t with bisection

	// set start values
	double t = 1.0;
	irr::core::vector3d<double> newPosition = ComputePosition(t);
	double actDistanceSQ = (newPosition-lastPosition_).getLengthSQ(); // use squared distances for faster computation
	double distanceSQ = distance_*distance_;
	
	// if distance to t=1.0 (endpoint) already too small, no desired distance possible
	if (actDistanceSQ <= distanceSQ)
		return irr::core::vector2d<double>(t,sqrt(actDistanceSQ));

	// interval boundaries for searching t
	double upperBorder = 1.0;
	double lowerBorder = lastT_;	
	
	// iterate until desired precision is achieved
	while (abs(upperBorder-lowerBorder)>Precision)
	{
		// distance too big -> lower the distance, use lower interval half
		if (actDistanceSQ>distanceSQ)
			upperBorder = t;
		// distance too small -> enlarge distance, use upper interval half
		else
			lowerBorder = t;
		
		// update values:
		// t is median of upper and lower boundary
		t = 0.5*(lowerBorder+upperBorder);
		
		// compute new position and distance
		newPosition = ComputePosition(t);
		actDistanceSQ = (newPosition-lastPosition_).getLengthSQ();
	}

	// return result
	return irr::core::vector2d<double>(t,distance_);
}

// ======================================================
// creation of the corridor
// ======================================================

double DunGen::CCorridor::CreateCorridor(const SCorridorProfile& profile_,
	const SDockingSite& dockingSite0_, const SDockingSite& dockingSite1_, double distance_, double distanceTextureYPerDistance1_)
{
	// spline parameter t
	double t;
	// position and derivation for current t value
	irr::core::vector3d<double> actPosition;
	irr::core::vector3d<double> actDerivation;
	// local coordinate system (left handed)
	irr::core::vector3d<double> front;
	irr::core::vector3d<double> left;
	irr::core::vector3d<double> up;

	// parameters to determine the region of interests for the visibility test
	// they should have a high distance to axis P(1)-P(0) -> local maxima (there should be exactly 1 or 2)
	irr::core::vector3d<double> roiAxis = (Position[1]-Position[0]).normalize();
	NVisibilityTest::SViewRegionOfInterest actRegionOfInterest;
	double actDistanceSQ = 0.0;
	double lastDistanceSQ;
	bool roiStored = false;

	// security limit for mesh buffer: one profile segment
	unsigned int resMaxNumVertices = MaxVertexCount - profile_.Point.size();
	// create new mesh
	MeshCorridor = new irr::scene::SMesh();

	// create and add first buffer
	irr::scene::SMeshBuffer* meshBuffer = new irr::scene::SMeshBuffer();
	MeshCorridor->addMeshBuffer(meshBuffer);
	meshBuffer->drop(); // can be dropped now, because it is hold by the mesh
	// reserve for worst case
	meshBuffer->Vertices.set_used(MaxVertexCount); 
	meshBuffer->Indices.set_used(6*MaxVertexCount);	// 6 indices per vertex (2 triangles per vertex)
	unsigned int bufferVertices = 0;
	unsigned int bufferIndizes = 0;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// add fist corridor segment
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	double actTextureCoordY = 0;
	irr::core::vector3d<double> tmp;

	t = 0.0;
	actPosition = Position[0];
	actDerivation = Derivation[0];
	// compute coordinate system (left handed)
	front = actDerivation.normalize();
	left = front.crossProduct(UpStandard);
	left.normalize();
	up = left.crossProduct(front);

	// for every point of the profile:
	for (unsigned int i = 0; i<profile_.Point.size(); ++i)
	{
		// compute new vertex and add it to buffer, resulting position: (-left,up)*(X,Y)
		tmp = actPosition - profile_.Point[i].X*left + profile_.Point[i].Y*up;
		irr::video::S3DVertex& v = meshBuffer->Vertices[bufferVertices++];
		v.Pos.set(vec3F(tmp));
		
		// compute normal
		tmp = actPosition - tmp;
		tmp.normalize();
		v.Normal.set(vec3F(tmp));

		// set texture coordinates
		v.TCoords.set(static_cast<irr::f32>(profile_.TextureX[i]),static_cast<irr::f32>(actTextureCoordY));
	}

	// first segment is also a region of interest
	actRegionOfInterest.Position = actPosition;
	actRegionOfInterest.Left = left;
	actRegionOfInterest.Up = up;
	ViewTestRegions.push_back(actRegionOfInterest);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// go on further on the spline
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	irr::core::vector2d<double> computeTResult;
	while (t < 1.0)
	{
		// actualize t
		computeTResult = ComputeT(actPosition, t, distance_);
		t = computeTResult.X;
		// compute y texture coordinate and position
		actTextureCoordY+= computeTResult.Y * distanceTextureYPerDistance1_;
		actPosition = ComputePosition(t);

		// compute coordinate system (left handed)
		actDerivation = ComputeDerivation(t);
		front = actDerivation.normalize();
		left = front.crossProduct(UpStandard);
		left.normalize();
		up = left.crossProduct(front);

		// look for region of interest
		lastDistanceSQ = actDistanceSQ;
		tmp = actPosition-Position[0];
		actDistanceSQ = (tmp - tmp.dotProduct(roiAxis)*roiAxis).getLengthSQ();
		// distance gets larger: store data
		if (actDistanceSQ>lastDistanceSQ)
		{
			actRegionOfInterest.Position = actPosition;
			actRegionOfInterest.Left = left;
			actRegionOfInterest.Up = up;
			roiStored = false;
		}
		// distance gets smaller -> local maximum -> store region of interest
		else if (!roiStored)
		{
			// data of the last pass is stored (the data of the local maximum)
			ViewTestRegions.push_back(actRegionOfInterest);
			roiStored = true;
		}

		// for every point of the profile:
		for (unsigned int i = 0; i<profile_.Point.size(); ++i)
		{
			// compute new vertex and add it to buffer, resulting position: (-left,up)*(X,Y)
			tmp = actPosition - profile_.Point[i].X*left + profile_.Point[i].Y*up;
			irr::video::S3DVertex& v = meshBuffer->Vertices[bufferVertices++];
			v.Pos.set(vec3F(tmp));
		
			// compute normal
			tmp = actPosition - tmp;
			tmp.normalize();
			v.Normal.set(vec3F(tmp));

			// set texture coordinates
			v.TCoords.set(static_cast<irr::f32>(profile_.TextureX[i]),static_cast<irr::f32>(actTextureCoordY));
		}
		
		// create triangles
		// indices counter clockwise from outside
		// triangle 1
		meshBuffer->Indices[bufferIndizes++] = bufferVertices - profile_.Point.size() - 1;	// last point of last profile 
		meshBuffer->Indices[bufferIndizes++] = bufferVertices - profile_.Point.size();		// first point of actual profile
		meshBuffer->Indices[bufferIndizes++] = bufferVertices - 1;							// last point of actual profile

		// triangle 1
		meshBuffer->Indices[bufferIndizes++] = bufferVertices - profile_.Point.size() - 1;	// last point of last profile
		meshBuffer->Indices[bufferIndizes++] = bufferVertices - 2*profile_.Point.size();	// first point of last profile
		meshBuffer->Indices[bufferIndizes++] = bufferVertices - profile_.Point.size();		// first point of actual profile
	
		// rest:
		for (unsigned int i = 1; i<profile_.Point.size(); i++)
		{
			// triangle 1
			meshBuffer->Indices[bufferIndizes++] = bufferVertices - 2*profile_.Point.size() + i - 1;
			meshBuffer->Indices[bufferIndizes++] = bufferVertices - profile_.Point.size() + i;
			meshBuffer->Indices[bufferIndizes++] = bufferVertices - profile_.Point.size() + i - 1;			

			// triangle 2
			meshBuffer->Indices[bufferIndizes++] = bufferVertices - 2*profile_.Point.size() + i - 1;
			meshBuffer->Indices[bufferIndizes++] = bufferVertices - 2*profile_.Point.size() + i;
			meshBuffer->Indices[bufferIndizes++] = bufferVertices - profile_.Point.size() + i;			
		}

		// if mesh buffer full:
		if (bufferVertices > resMaxNumVertices && t < 1.0)
		{
			// old buffer: set final size and bounding box
			meshBuffer->Vertices.reallocate(bufferVertices);
			meshBuffer->Indices.reallocate(bufferIndizes);
			meshBuffer->recalculateBoundingBox();

			// create and add new buffer
			meshBuffer = new irr::scene::SMeshBuffer();
			bufferVertices = 0;	
			bufferIndizes = 0;
			MeshCorridor->addMeshBuffer(meshBuffer);
			meshBuffer->drop();
			meshBuffer->Vertices.set_used(MaxVertexCount);
			meshBuffer->Indices.set_used(6*MaxVertexCount);

			// add vertices of the last segment again, so the mesh remains connected
			// for every point of the profile:
			for (unsigned int i = 0; i<profile_.Point.size(); ++i)
			{
				// compute new vertex and add it to buffer, resulting position: (-left,up)*(X,Y)
				tmp = actPosition - profile_.Point[i].X*left + profile_.Point[i].Y*up;
				irr::video::S3DVertex& v = meshBuffer->Vertices[bufferVertices++];
				v.Pos.set(vec3F(tmp));
		
				// compute normal
				tmp = actPosition - tmp;
				tmp.normalize();
				v.Normal.set(vec3F(tmp));

				// set texture coordinates
				v.TCoords.set(static_cast<irr::f32>(profile_.TextureX[i]),static_cast<irr::f32>(actTextureCoordY));
			}
		}
	} // END: while

	// buffer: set final size and bounding box
	meshBuffer->Vertices.reallocate(bufferVertices);
	meshBuffer->Indices.reallocate(bufferIndizes);
	meshBuffer->recalculateBoundingBox();

	// set final bounding box of the mesh
	MeshCorridor->recalculateBoundingBox();	

	// last segment is also a region of interest
	actRegionOfInterest.Position = actPosition;
	actRegionOfInterest.Left = left;
	actRegionOfInterest.Up = up;
	ViewTestRegions.push_back(actRegionOfInterest);

	// return the y texture coordinate of the last segment (the first segment has 0.0)
	return actTextureCoordY;
}

// ======================================================
// detail objects
// ======================================================

void DunGen::CCorridor::PlaceDetailObject(const SDetailobjectParameters& parameters_, const CRandomGenerator* randomGenerator_)
{
	// create new detail object
	SDetailObject* newDetailObject = new SDetailObject();
	newDetailObject->Node = parameters_.Node;
	newDetailObject->Scale = parameters_.Scale;
	
	// compute matrix of the basic rotation
	irr::core::CMatrix4<double> basicRotationMatrix(irr::core::CMatrix4<double>::EM4CONST_IDENTITY);
	basicRotationMatrix.setRotationDegrees(parameters_.Rotation);
	
	// matrix of the spline rotation
	irr::core::CMatrix4<double> splineRotationMatrix(irr::core::CMatrix4<double>::EM4CONST_NOTHING);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// place objects along the spline
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~	

	// multiple of the _DistanceSampling
	unsigned int distanceFactor = parameters_.DistanceNumFactor*
		randomGenerator_->GetRandomNumberMinMax(parameters_.DistanceNumMinFirstElement,parameters_.DistanceNumMaxFirstElement);
	// spline parameter t
	double t = 0.0;
	// position and  derivation for current t value
	irr::core::vector3d<double> actPosition = ComputePosition(t);
	irr::core::vector3d<double> actDerivation;
	// local coordinate system
	irr::core::vector3d<double> front;
	irr::core::vector3d<double> left;
	irr::core::vector3d<double> up;

	// if distance is 0 -> place object
	if (distanceFactor==0)
	{
		// compute coordinate system (left handed)
		actDerivation = ComputeDerivation(t);	
		front = actDerivation.normalize();
		left = front.crossProduct(UpStandard);
		left.normalize();
		up = left.crossProduct(front);

		// resulting position: (-left,up)*(X,Y)
		newDetailObject->Position.push_back(actPosition - parameters_.Position.X*left + parameters_.Position.Y*up);

		// compute angle between (0,0,1) and front
		// and setup rotation matrix
		splineRotationMatrix.makeIdentity();
		splineRotationMatrix.setRotationDegrees(front.getHorizontalAngle());
		// aggregate rotations
		newDetailObject->Rotation.push_back((splineRotationMatrix*basicRotationMatrix).getRotationDegrees());

		// compute new distance factor
		distanceFactor = parameters_.DistanceNumFactor*
			randomGenerator_->GetRandomNumberMinMax(parameters_.DistanceNumMin,parameters_.DistanceNumMax);
	}

	// go on further on the spline
	irr::core::vector2d<double> computeTResult;
	while (t < 1.0)
	{
		// actualize
		computeTResult = ComputeT(actPosition, t, parameters_.DistanceSampling);
		t = computeTResult.X;
		actPosition = ComputePosition(t);
		--distanceFactor;

		// if not enough distance (can be for the last t): break
		if ((computeTResult.Y < (parameters_.DistanceSampling-Precision)) && (!parameters_.ObjectAtT1))
			break;

		// if distance is 0 -> place object
		if ((distanceFactor==0)||(t==1.0))
		{
			// compute coordinate system (left handed)
			actDerivation = ComputeDerivation(t);
			front = actDerivation.normalize();
			left = front.crossProduct(UpStandard);
			left.normalize();
			up = left.crossProduct(front);

			// resulting position: (-left,up)*(X,Y)
			newDetailObject->Position.push_back(actPosition - parameters_.Position.X*left + parameters_.Position.Y*up);

			// compute angle between (0,0,1) and front
			// and setup rotation matrix
			splineRotationMatrix.makeIdentity();
			splineRotationMatrix.setRotationDegrees(front.getHorizontalAngle());
			// aggregate rotations
			newDetailObject->Rotation.push_back((splineRotationMatrix*basicRotationMatrix).getRotationDegrees());

			// compute new distance factor
			distanceFactor = parameters_.DistanceNumFactor*
				randomGenerator_->GetRandomNumberMinMax(parameters_.DistanceNumMin,parameters_.DistanceNumMax);
		}
	}
	
	// store detailobjects
	DetailObjects.push_back(newDetailObject);
}