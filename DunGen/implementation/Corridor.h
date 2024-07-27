// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#ifndef CORRIDOR_H
#define CORRIDOR_H

#include "interface/CorridorCommon.h"
#include "VisibilityTest.h"
#include <irrlicht.h>
#include <vector>

// Namespace DunGen : DungeonGenerator
namespace DunGen
{
	class CAdapter;
	class CRandomGenerator;
	struct SDockingSite;

	/// a corridor profile
	///
	/// a chain of 3d points as a closed curve
	/// clockwise direction
	/// important: polar angle of the points has to be monotonic increasing circularly
	/// the 0 point has to be enclosed by the projected curve  (-> at least 3 points necessary)
	struct SCorridorProfile
	{
		std::vector<irr::core::vector2d<double>> Point;	///< points of the profile
		std::vector<double> TextureX;					///< x texture coordinate of each point
	};

	/// a corridor in a dungeon
	///
	/// uses a cubical hermite spline as base curve
	class CCorridor
	{
	public:
		/// detail objects along the corridor: e.g. torches
		struct SDetailObject
		{
			irr::scene::ISceneNode* Node;							///< node of all instances
			irr::core::vector3d<double> Scale;						///< scale of all instances
			std::vector<irr::core::vector3d<double>> Position;		///< position of each instance
			std::vector<irr::core::vector3d<double>> Rotation;		///< rotation of each instance
		};

	public:
		/// constructor: create corridor
		///
		/// important: derivation has to be unequal to the y axis vector (0,y,0)
		CCorridor(const SCorridorProfile& profile_, const SDockingSite& dockingSite0_, const SDockingSite& dockingSite1_,
			const irr::core::vector3d<double>& position0_, const irr::core::vector3d<double>& position1_,
			const irr::core::vector3d<double>& derivation0_, const irr::core::vector3d<double>& derivation1_,
			double distance_, double distanceTextureYPerDistance1_);
		 /// destructor
		~CCorridor();

		/// place detailobject
		/// 
		/// rotation is the rotation, if the corridor points to (0,0,1)
		/// position in 2D to the center of a corridor segment
		/// object is placed in random distance DistanceSampling*DistanceNumFactor*[DistanceNumMin,DistanceNumMax]
		/// first element is placed in distance DistanceSampling*DistanceNumFactor*[DistanceNumMinFirstElement,DistanceNumMaxFirstElement] from position0
		/// if ObjectAtT1, an object is placed at position1, even if the distance to the last object would be too small
		///
		/// important: Max>=Min, DistanceNumMin>=1, DistanceNumMinFirstElement>=0
		void PlaceDetailObject(const SDetailobjectParameters& parameters_, const CRandomGenerator* randomGenerator_);

		/// is the corridor sight blocking: do one can not see from one end to the other?
		bool GetDefinitivelySightBlocking();

		/// read mesh corridor
		irr::scene::SMesh* GetMesh();

		/// read mesh adapter
		irr::scene::SMesh* GetMeshAdapter(unsigned int i_);

		/// reads a detailobject, if no detailobject with this index exits, NULL is returned
		SDetailObject* GetDetailObject(unsigned int i_);

	private:
		/// creates the triangle mesh of the corridor
		///
		/// _Distance ... desired distance between the segments of the corridor
		/// \returns lastTextureY
		double CreateCorridor(const SCorridorProfile& profile_, const SDockingSite& dockingSite0_, const SDockingSite& dockingSite1_,
			double distance_, double distanceTextureYPerDistance1_);

		/// computes new t value for point in given destination
		///
		/// \returns (new t, distance from point of last t to point of new t)
		inline irr::core::vector2d<double> ComputeT(const irr::core::vector3d<double>& lastPosition_, double lastT_, double distance_);

		/// returns position to given t
		inline irr::core::vector3d<double> ComputePosition(double t_);

		/// returns derivation to given t
		inline irr::core::vector3d<double> ComputeDerivation(double t_);

		/// compute cubical hermite spline coefficients for position and derivation
		void ComputeResultingCoefficients();

	public:
		/// standard vector for Up, defines the height axis
		static const irr::core::vector3d<double> UpStandard;

		/// maximal number of vertices per meshbuffer
		static const unsigned int MaxVertexCount = 65500;

	private:
		/// precision for distance when searching the next parameter t
		static const double Precision;

		/// triangular mesh of the corridor
		irr::scene::SMesh* MeshCorridor;

		/// the adapters of the corridor
		CAdapter* Adapter[2];

		/// detail objects
		std::vector<SDetailObject*> DetailObjects;

		/// start- and endposition of the corridor
		irr::core::vector3d<double> Position[2];

		/// derivation of the hermite curve in the start- and endposition of the corridor
		irr::core::vector3d<double> Derivation[2];

		/// coefficients for computing the position P(t) 3x4 matrix
		///
		/// P(t) = (t^3,t^2,t,1) * PositionCoefficients[0...3]
		irr::core::vector3d<double> PositionCoefficients[4];
		/// coefficients for computing the first derivation P1(t): 3x3 matrix
		///
		/// P1(t) = (t^2,t,1) * DerivationCoefficients[0...2]
		irr::core::vector3d<double> DerivationCoefficients[3];

		/// region of interests
		///
		/// number is the number of local maxima regarding the distance point<->axis P(1)-P(0) -> normally 1 or 2 values
		/// regions are ordered from P(0) to P(1)
		std::vector<NVisibilityTest::SViewRegionOfInterest> ViewTestRegions;

		/// is the corridor sight blocking: do one can not see from one end to the other?
		bool DefinitivelySightBlocking;
	};

} // END NAMESPACE DunGen

#endif