// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#ifndef ADAPTER_H
#define ADAPTER_H

#include <irrlicht.h>
#include <vector>

// Namespace DunGen : DungeonGenerator
namespace DunGen
{
	struct SCorridorProfile;
	struct SDockingSite;

	/// a corridor in a dungeon
	///
	/// uses a cubical hermite spline as base curve
	class CAdapter
	{
	private:
		/// instructions for drawing a adapter: corridor information
		struct SCorridorDrawingInstruction
		{
			irr::core::vector3d<double> Point;		///< point to draw
			irr::core::vector2d<double> Point2D;	///< 2D coordinates of the point
			irr::core::vector2d<double> Texture;	///< texture coordinates of the point
			double Angle;							///< polar angle of the point when projected in docking site plane
		};

		/// preprocessing information for docking site
		struct SDockingSitePreprocessingInformation
		{
			irr::core::vector3d<double> Point;		///< point to draw
			double Angle;							///< polar angle of the point when projected in docking site plane
		};

		/// instructions for drawing a adapter: docking site information
		struct SDockingSiteDrawingInstruction
		{
			irr::core::vector3d<double> Point;		///< point to draw
			irr::core::vector2d<double> Texture;	///< texture coordinates of the point
			double Angle;							///< polar angle of the point when projected in docking site plane
			unsigned int CorrespondingIndex;		///< which corridor segment index has most similar angle?
			bool TextureXfromCorridor;				///< is the x texture coordiante directly taken from the corridor segment?
		};

	public:
		/// constructor: create adapter
		CAdapter(unsigned int end_, const irr::core::vector3d<double>& position_,irr::core::vector3d<double>& derivation_,
			const SCorridorProfile& profile_, double distanceTextureYPerDistance1_,	double lastTextureY_,
			const SDockingSite& dockingSite_);

		 /// destructor
		~CAdapter();

		/// read mesh adapter
		irr::scene::SMesh* GetMesh();

	private:
		/// computes the docking site preprocessing information
		void PreprocessDockingSite(const SDockingSite& dockingSite_,
			irr::core::vector3d<double>& leftOfDS_, irr::core::vector3d<double>& upOfDS_,
			std::vector<SDockingSitePreprocessingInformation>& dockingSitePPIs_);

		/// computes the drawing instructions for the corridor vertices
		void ComputeCorridorDIs(const irr::core::vector3d<double>& position_, const irr::core::vector3d<double>& derivation_,
			const SCorridorProfile& profile_, double lastTextureY_,
			std::vector<SCorridorDrawingInstruction>& corridorDIs_);

		/// computes the drawing instructions for the docking site
		void ComputeDockingSiteDIs(unsigned int end_,
			const std::vector<SDockingSitePreprocessingInformation>& dockingSitePPIs_, const std::vector<SCorridorDrawingInstruction> &corridorDIs_,
			const SDockingSite& dockingSite_, const irr::core::vector3d<double>& leftOfDS_, const irr::core::vector3d<double>& upOfDS_,
			double distanceTextureYPerDistance1_, std::vector<SDockingSiteDrawingInstruction>& dockingSiteDIs_);		

		/// computes the X texture coordinates for the docking site vertices and adds them to the drawing instructions
		void ComputeDockingSiteXTextureCoordinates(const irr::core::vector3d<double>& leftOfDS_,  const irr::core::vector3d<double>& upOfDS_,
			std::vector<SDockingSiteDrawingInstruction>& dockingSiteDIs_);

		/// creates the adapters vertices and triangles
		void DrawAdapter(unsigned int end_,
			const irr::core::vector3d<double>& position_,
			const SDockingSite& dockingSite_,
			const std::vector<SCorridorDrawingInstruction>& corridorDI,
			const std::vector<SDockingSiteDrawingInstruction>& dockingSiteDIs_);

		/// compute the interpolation factor for position P, P is on line segment AB of triangle AMB
		double ComputePositionInterpolationFactor(const irr::core::vector2d<double>& ma_, const irr::core::vector2d<double>& mb_, double angleAMP_);

	private:
		/// triangular meshes of the adapters
		irr::scene::SMesh* MeshAdapter;
	};

} // END NAMESPACE DunGen

#endif