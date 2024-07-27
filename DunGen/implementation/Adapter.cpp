#include "Adapter.h"
#include "Corridor.h"
#include "DockingSite.h"
#include "Helperfunctions.h"

// ======================================================
// helper functions
// ======================================================

double DunGen::CAdapter::ComputePositionInterpolationFactor(
	const irr::core::vector2d<double>& ma_, const irr::core::vector2d<double>& mb_, double angleAMP_)
{
	double ma = ma_.getLength();
	double mb = mb_.getLength();
	irr::core::vector2d<double> vecAB = mb_-ma_;
	double ab = (vecAB).getLength();

	if (ma == 0.0 || ab == 0.0)
		return 0.0;

	// compute angle by law of cosines: 0 to PI
	double angleBAM = acos(-ma_.dotProduct(vecAB) / (ma * ab));
	// law of internal angles of a triangle
	double angleMPA = M_PI - angleBAM - angleAMP_;
	// law of sines
	double ap = sin(angleAMP_)/sin(angleMPA) * ma;

	return ap/ab;
}

// ======================================================
// constructor / destructor
// ======================================================

DunGen::CAdapter::CAdapter(unsigned int end_,
	const irr::core::vector3d<double>& position_,irr::core::vector3d<double>& derivation_,
	const SCorridorProfile& profile_, double distanceTextureYPerDistance1_,	double lastTextureY_,
	const SDockingSite& dockingSite_)
{
	// create new mesh
	MeshAdapter = new irr::scene::SMesh();
	
	// if the docking site has no points -> no docking site should be created
	if (0 == dockingSite_.Point.size())
	{
		MeshAdapter->recalculateBoundingBox();
		return;
	}

	std::vector<SDockingSitePreprocessingInformation> dockingSitePPIs;
	std::vector<SCorridorDrawingInstruction> corridorDIs;
	std::vector<SDockingSiteDrawingInstruction> dockingSiteDIs;
	irr::core::vector3d<double> leftOfDS;
	irr::core::vector3d<double> upOfDS;

	// compute necessary information
	PreprocessDockingSite(dockingSite_, leftOfDS, upOfDS, dockingSitePPIs);
	ComputeCorridorDIs(position_, derivation_, profile_, lastTextureY_, corridorDIs);
	ComputeDockingSiteDIs(end_, dockingSitePPIs, corridorDIs, dockingSite_, leftOfDS, upOfDS,
		distanceTextureYPerDistance1_, dockingSiteDIs);
	ComputeDockingSiteXTextureCoordinates(leftOfDS, upOfDS, dockingSiteDIs);

	// draw
	DrawAdapter(end_, position_, dockingSite_, corridorDIs, dockingSiteDIs);
}

DunGen::CAdapter::~CAdapter()
{
	delete MeshAdapter;
}

// ======================================================
// generator functions
// ======================================================

void DunGen::CAdapter::PreprocessDockingSite(const SDockingSite& dockingSite_,
	irr::core::vector3d<double>& leftOfDS_, irr::core::vector3d<double>& upOfDS_,
	std::vector<SDockingSitePreprocessingInformation>& dockingSitePPIs_)
{
	// compute local coordinate system
	leftOfDS_ = dockingSite_.Normal.crossProduct(CCorridor::UpStandard);
	leftOfDS_.normalize();
	upOfDS_ = leftOfDS_.crossProduct(dockingSite_.Normal);

	unsigned int startIndex, actIndex;
	double angleTmp;
	std::vector<double> angles;
	irr::core::vector3d<double> tmp;

	// compute polar angles for each point: clockwise, 0 degrees is left-vector
	for (unsigned int i = 0; i<dockingSite_.Point.size(); ++i)
	{
		// origin is the center
		tmp = dockingSite_.Point[i]-dockingSite_.Center;		
		// use projected 2D-coordinates
		angleTmp = atan2(tmp.dotProduct(upOfDS_),tmp.dotProduct(leftOfDS_));
		angles.push_back(angleTmp);
	}

	// sort by increasing angle:

	// compute start index
	angleTmp = 3*M_PI;
	for (unsigned int i = 0; i<angles.size(); ++i)
		if (angles[i]<angleTmp)
		{
			angleTmp = angles[i];
			startIndex = i;
		};

	// first value
	SDockingSitePreprocessingInformation actDockingSitePPI;
	actIndex = startIndex;
	actDockingSitePPI.Point = dockingSite_.Point[actIndex];
	actDockingSitePPI.Angle = angles[actIndex];
	dockingSitePPIs_.push_back(actDockingSitePPI);
	++actIndex;
	if (actIndex==dockingSite_.Point.size())
		actIndex = 0;
	
	// remaining values
	while (actIndex != startIndex)
	{
		actDockingSitePPI.Point = dockingSite_.Point[actIndex];
		actDockingSitePPI.Angle = angles[actIndex];
		dockingSitePPIs_.push_back(actDockingSitePPI);
		++actIndex;
		if (actIndex==dockingSite_.Point.size())
			actIndex = 0;
	}
}

void DunGen::CAdapter::ComputeCorridorDIs(
	const irr::core::vector3d<double>& position_, const irr::core::vector3d<double>& derivation_,
	const SCorridorProfile& profile_, double lastTextureY_,
	std::vector<SCorridorDrawingInstruction>& corridorDIs_)
{	
	// compute local coordinate system
	irr::core::vector3d<double> front = derivation_;
	front.normalize();
	irr::core::vector3d<double> left = front.crossProduct(CCorridor::UpStandard);
	left.normalize();
	irr::core::vector3d<double>	up = left.crossProduct(front);

	unsigned int startIndex, actIndex;
	double angleTmp;
	std::vector<double> angles;

	// compute polar angles for each point: clockwise, 0 degrees is left-vector
	for (unsigned int i = 0; i<profile_.Point.size(); ++i)
	{
		// atan2: -PI bis PI
		angleTmp = atan2( profile_.Point[i].Y,- profile_.Point[i].X);
		angles.push_back(angleTmp);
	}
		
	// sort by increasing angle:

	// compute start index
	angleTmp = 3*M_PI;
	for (unsigned int i = 0; i<angles.size(); ++i)
		if (angles[i]<angleTmp)
		{
			angleTmp = angles[i];
			startIndex = i;
		};

	// first value
	SCorridorDrawingInstruction actCorridorDI;
	actIndex = startIndex;
	actCorridorDI.Point2D = profile_.Point[actIndex];
	actCorridorDI.Point = position_ - actCorridorDI.Point2D.X*left + actCorridorDI.Point2D.Y*up;
	actCorridorDI.Texture = irr::core::vector2d<double>(profile_.TextureX[actIndex],lastTextureY_);
	actCorridorDI.Angle = angles[actIndex];
	corridorDIs_.push_back(actCorridorDI);

	++actIndex;
	if (actIndex==profile_.Point.size())
		actIndex = 0;
	
	// remaining values
	while (actIndex != startIndex)
	{
		actCorridorDI.Point2D = profile_.Point[actIndex];
		actCorridorDI.Point = position_ - actCorridorDI.Point2D.X*left + actCorridorDI.Point2D.Y*up;
		actCorridorDI.Texture = irr::core::vector2d<double>(profile_.TextureX[actIndex],lastTextureY_);
		actCorridorDI.Angle = angles[actIndex];
		corridorDIs_.push_back(actCorridorDI);

		++actIndex;
		if (actIndex==profile_.Point.size())
			actIndex = 0;
	}
}

void DunGen::CAdapter::ComputeDockingSiteDIs(unsigned int end_,
	const std::vector<SDockingSitePreprocessingInformation>& dockingSitePPIs_, const std::vector<SCorridorDrawingInstruction> &corridorDIs_,
	const SDockingSite& dockingSite_, const irr::core::vector3d<double>& leftOfDS_, const irr::core::vector3d<double>& upOfDS_,
	double distanceTextureYPerDistance1_, std::vector<SDockingSiteDrawingInstruction>& dockingSiteDIs_)
{
	unsigned int actIndexDockingSite = 0;
	unsigned int actIndexCorridorSegment = 0;
	unsigned int lastIndexDockingSite = dockingSitePPIs_.size()-1;
	unsigned int lastIndexCorridorSegment = corridorDIs_.size()-1;
	double interpolationFactor;

	bool closedDockingSite = false;
	bool closedCorridorSegment = false;

	double angle1, angle2;
	irr::core::vector3d<double> tmp;

	// walk over all vertices (adapter has to be closed)
	while(!(closedDockingSite&&closedCorridorSegment))
	{
		// which is the next vertex? the one with the smaller angle!
		if (
			closedDockingSite ||
			((corridorDIs_[actIndexCorridorSegment].Angle < dockingSitePPIs_[actIndexDockingSite].Angle) && !closedCorridorSegment)	
			)
			// corridor vertex
		{
			// compute interpolation factor
			angle1 = corridorDIs_[actIndexCorridorSegment].Angle - dockingSitePPIs_[lastIndexDockingSite].Angle;

			interpolationFactor = ComputePositionInterpolationFactor(
				//  vector (p-m) in projected coordinates
				projectVectorToPlane(dockingSitePPIs_[lastIndexDockingSite].Point-dockingSite_.Center, leftOfDS_, upOfDS_),
				projectVectorToPlane(dockingSitePPIs_[actIndexDockingSite].Point-dockingSite_.Center, leftOfDS_, upOfDS_),
				angle1);

			// compute coordinates
			tmp = (1-interpolationFactor) * dockingSitePPIs_[lastIndexDockingSite].Point
				+ interpolationFactor * dockingSitePPIs_[actIndexDockingSite].Point;

			// create new drawing instruction
			SDockingSiteDrawingInstruction newDockingSiteDI;
			newDockingSiteDI.Point = tmp;
			newDockingSiteDI.CorrespondingIndex = actIndexCorridorSegment;
			newDockingSiteDI.Angle = corridorDIs_[actIndexCorridorSegment].Angle;

			// x texture coordinate is taken from corridor segment point
			newDockingSiteDI.TextureXfromCorridor = true;
			newDockingSiteDI.Texture.X = corridorDIs_[actIndexCorridorSegment].Texture.X;
			// y texture coordinate is computed by the distance to the corridor segment point
			newDockingSiteDI.Texture.Y = (newDockingSiteDI.Point - corridorDIs_[actIndexCorridorSegment].Point).getLength() * distanceTextureYPerDistance1_;
			if (0 == end_)
				newDockingSiteDI.Texture.Y = -newDockingSiteDI.Texture.Y;
			newDockingSiteDI.Texture.Y += corridorDIs_[actIndexCorridorSegment].Texture.Y;

			// store drawing instruction
			dockingSiteDIs_.push_back(newDockingSiteDI);

			// count on indices
			lastIndexCorridorSegment = actIndexCorridorSegment;
			++actIndexCorridorSegment;
			if (actIndexCorridorSegment == corridorDIs_.size())
			{
				actIndexCorridorSegment = 0;
				closedCorridorSegment = true;
			}

		}
		else if (closedCorridorSegment || (dockingSitePPIs_[actIndexDockingSite].Angle < corridorDIs_[actIndexCorridorSegment].Angle) )
			// docking site vertex
		{
			// create new drawing instruction
			SDockingSiteDrawingInstruction newDockingSiteDI;
			newDockingSiteDI.Point = dockingSitePPIs_[actIndexDockingSite].Point;
			newDockingSiteDI.Angle = dockingSitePPIs_[actIndexDockingSite].Angle;

			// search index of the corridor segment point with most similar angle
			angle1 = abs(dockingSitePPIs_[actIndexDockingSite].Angle - corridorDIs_[actIndexCorridorSegment].Angle);
			if (angle1>M_PI) angle1 = 2*M_PI - angle1; // smaller angle of 2PI
			angle2 = abs(dockingSitePPIs_[actIndexDockingSite].Angle - corridorDIs_[lastIndexCorridorSegment].Angle);
			if (angle2>M_PI) angle2 = 2*M_PI - angle2; // smaller angle of 2PI
			// index is the corresponding index of the drawing instruction
			if (angle1<angle2)
				newDockingSiteDI.CorrespondingIndex = actIndexCorridorSegment;
			else
				newDockingSiteDI.CorrespondingIndex = lastIndexCorridorSegment;

			// x texture coordinate will be computed later
			newDockingSiteDI.TextureXfromCorridor = false;

			// compute y texture coordinate:
			// compute interpolation factor
			angle1 =  newDockingSiteDI.Angle - corridorDIs_[lastIndexCorridorSegment].Angle;
			interpolationFactor = ComputePositionInterpolationFactor(
				corridorDIs_[lastIndexCorridorSegment].Point2D,
				corridorDIs_[actIndexCorridorSegment].Point2D,
				angle1);
			// compute coordinates of a temporary corridor segment point with similar angle
			tmp = (1-interpolationFactor) * corridorDIs_[lastIndexCorridorSegment].Point
				+ interpolationFactor * corridorDIs_[actIndexCorridorSegment].Point;

			// y texture coordinate is computed by the distance to the temporary corridor segment point
			newDockingSiteDI.Texture.Y = (newDockingSiteDI.Point - tmp).getLength() * distanceTextureYPerDistance1_;
			if (0 == end_)
				newDockingSiteDI.Texture.Y = -newDockingSiteDI.Texture.Y;
			newDockingSiteDI.Texture.Y += corridorDIs_[actIndexCorridorSegment].Texture.Y;

			// store drawing instruction
			dockingSiteDIs_.push_back(newDockingSiteDI);

			// count on indices
			lastIndexDockingSite = actIndexDockingSite;
			++actIndexDockingSite;
			if (actIndexDockingSite == dockingSitePPIs_.size())
			{
				actIndexDockingSite = 0;
				closedDockingSite = true;
			}
		}
		else
			// similar angles
		{
			// create new drawing instruction
			SDockingSiteDrawingInstruction newDockingSiteDI;
			newDockingSiteDI.Point = dockingSitePPIs_[actIndexDockingSite].Point;
			newDockingSiteDI.Angle = dockingSitePPIs_[actIndexDockingSite].Angle;
			newDockingSiteDI.CorrespondingIndex = actIndexCorridorSegment;

			// x texture coordinate is taken from corridor segment point
			newDockingSiteDI.TextureXfromCorridor = true;
			newDockingSiteDI.Texture.X = corridorDIs_[actIndexCorridorSegment].Texture.X;
			/// y texture coordinate is computed by the distance to the corridor segment point
			newDockingSiteDI.Texture.Y = (newDockingSiteDI.Point - corridorDIs_[actIndexCorridorSegment].Point).getLength() * distanceTextureYPerDistance1_;
			if (end_ == 0)
				newDockingSiteDI.Texture.Y = -newDockingSiteDI.Texture.Y;
			newDockingSiteDI.Texture.Y += corridorDIs_[actIndexCorridorSegment].Texture.Y;

			// store drawing instruction
			dockingSiteDIs_.push_back(newDockingSiteDI);

			// count on indices
			lastIndexCorridorSegment = actIndexCorridorSegment;
			++actIndexCorridorSegment;
			if (actIndexCorridorSegment == corridorDIs_.size())
			{
				actIndexCorridorSegment = 0;
				closedCorridorSegment = true;
			}
			// count on indices
			lastIndexDockingSite = actIndexDockingSite;
			++actIndexDockingSite;
			if (actIndexDockingSite == dockingSitePPIs_.size())
			{
				actIndexDockingSite = 0;
				closedDockingSite = true;
			}
		}

	} // END while
}

void DunGen::CAdapter::ComputeDockingSiteXTextureCoordinates(
	const irr::core::vector3d<double>& leftOfDS_,  const irr::core::vector3d<double>& upOfDS_,
	std::vector<SDockingSiteDrawingInstruction>& dockingSiteDIs)
{
	// compute x texture coordinates for drawing instructions
	// if not TextureXfromCooridor x texture coordinate is interpolated
	// from the enclosing TextureXfromCorridor points,
	// interpolation is based on the distances between the docking-site-plane-projected points

	unsigned int lastIndexDockingSite, actIndexDockingSite, interpolationIndex;
	double fullLength, actLength;
	double interpolationFactor;

	for (unsigned int i = 0; i<dockingSiteDIs.size(); ++i)
		if (dockingSiteDIs[i].TextureXfromCorridor)
		{
			// compute full length of the walk between the 2 enclosing TextureXfromCorridors
			fullLength = 0;
			lastIndexDockingSite = i;
			actIndexDockingSite = i+1;
			if (actIndexDockingSite>=dockingSiteDIs.size())
				actIndexDockingSite=0;

			// search next TextureXfromCorridor
			while (!dockingSiteDIs[actIndexDockingSite].TextureXfromCorridor)
			{
				fullLength += projectVectorToPlane(dockingSiteDIs[actIndexDockingSite].Point-dockingSiteDIs[lastIndexDockingSite].Point,
					leftOfDS_, upOfDS_).getLength();
				lastIndexDockingSite = actIndexDockingSite;
				++actIndexDockingSite;
				if (actIndexDockingSite>=dockingSiteDIs.size())
					actIndexDockingSite=0;
			}
			fullLength += projectVectorToPlane(dockingSiteDIs[actIndexDockingSite].Point-dockingSiteDIs[lastIndexDockingSite].Point,
				leftOfDS_, upOfDS_).getLength();

			// prepare interpolation
			interpolationIndex = actIndexDockingSite;
			actLength = 0;
			lastIndexDockingSite = i;
			actIndexDockingSite = i+1;
			if (actIndexDockingSite>=dockingSiteDIs.size())
				actIndexDockingSite=0;

			// interpolate for all points between the enclosing TextureXfromCorridors
			while (!dockingSiteDIs[actIndexDockingSite].TextureXfromCorridor)
			{
				actLength += projectVectorToPlane(dockingSiteDIs[actIndexDockingSite].Point-dockingSiteDIs[lastIndexDockingSite].Point,
					leftOfDS_, upOfDS_).getLength();
				
				// interpolation factor is based on distance ratio
				interpolationFactor = actLength/fullLength;
				dockingSiteDIs[actIndexDockingSite].Texture.X = (1-interpolationFactor) * dockingSiteDIs[i].Texture.X
						 + interpolationFactor * dockingSiteDIs[interpolationIndex].Texture.X;

				lastIndexDockingSite = actIndexDockingSite;
				++actIndexDockingSite;
				if (actIndexDockingSite>=dockingSiteDIs.size())
					actIndexDockingSite=0;
			}
		}
}

void DunGen::CAdapter::DrawAdapter(unsigned int end_,
	const irr::core::vector3d<double>& position_,
	const SDockingSite& dockingSite_,
	const std::vector<SCorridorDrawingInstruction>& corridorDIs_,
	const std::vector<SDockingSiteDrawingInstruction>& dockingSiteDIs_)
{
	// create mesh buffer (only one needed per adapter)
	irr::scene::SMeshBuffer* meshBuffer = new irr::scene::SMeshBuffer();
	MeshAdapter->addMeshBuffer(meshBuffer);
	meshBuffer->drop();
	// reserve for worst case
	meshBuffer->Vertices.set_used(CCorridor::MaxVertexCount); 
	meshBuffer->Indices.set_used(3*CCorridor::MaxVertexCount);	// 3 indices per vertex (1 triangle per vertex)
	unsigned int bufferVertices = 0;
	unsigned int bufferIndizes = 0;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// create corridor segment vertices
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	irr::core::vector3d<double> tmp;
	// for every point of the profile:
	for (unsigned int i = 0; i<corridorDIs_.size(); ++i)
	{
		irr::video::S3DVertex& v = meshBuffer->Vertices[bufferVertices++];
		v.Pos.set(vec3F(corridorDIs_[i].Point));

		// compute normal
		tmp = (position_- corridorDIs_[i].Point).normalize();
		v.Normal.set(vec3F(tmp));

		// set texture coordinates
		v.TCoords.set(static_cast<irr::f32>(corridorDIs_[i].Texture.X),static_cast<irr::f32>(corridorDIs_[i].Texture.Y));
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// creating triangles and docking site vertices
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	unsigned int lastCorrespondingIndex = dockingSiteDIs_[0].CorrespondingIndex;

	// create new vertex
	irr::video::S3DVertex& v = meshBuffer->Vertices[bufferVertices++];
	tmp = dockingSiteDIs_[0].Point;
	v.Pos.set(vec3F(tmp));

	// compute normal
	tmp = dockingSite_.Center - tmp;
	// projection on plane is used (better result in most cases)
	tmp -= dockingSite_.Normal.dotProduct(tmp)*dockingSite_.Normal;
	tmp.normalize();
	v.Normal.set(vec3F(tmp));

	// set texture coordinates
	v.TCoords.set(static_cast<irr::f32>(dockingSiteDIs_[0].Texture.X), static_cast<irr::f32>(dockingSiteDIs_[0].Texture.Y));

	double angle1, angle2;
	// remaining vertices
	for (unsigned int i = 1; i<dockingSiteDIs_.size(); ++i)
	{
		// create new vertex
		irr::video::S3DVertex& v = meshBuffer->Vertices[bufferVertices++];
		tmp = dockingSiteDIs_[i].Point;
		v.Pos.set(vec3F(tmp));

		// compute normal
		tmp = dockingSite_.Center - tmp;
		// projection on plane is used (better result in most cases)
		tmp -= dockingSite_.Normal.dotProduct(tmp)*dockingSite_.Normal;

		tmp.normalize();
		v.Normal.set(vec3F(tmp));

		// set texture coordinates
		v.TCoords.set(static_cast<irr::f32>(dockingSiteDIs_[i].Texture.X),static_cast<irr::f32>(dockingSiteDIs_[i].Texture.Y));

		// add 1 triangle?
		if (dockingSiteDIs_[i].CorrespondingIndex == lastCorrespondingIndex)
		{
			meshBuffer->Indices[bufferIndizes++] = bufferVertices-1;

			if (0 == end_)	// direction is based on the end of the corridor
			{
				meshBuffer->Indices[bufferIndizes++] = dockingSiteDIs_[i].CorrespondingIndex;
				meshBuffer->Indices[bufferIndizes++] = bufferVertices-2;
			}
			else
			{
				meshBuffer->Indices[bufferIndizes++] = bufferVertices-2;
				meshBuffer->Indices[bufferIndizes++] = dockingSiteDIs_[i].CorrespondingIndex;
			}
		}
		// add 2 triangles?
		else		
		{
			// which of both vertices is nearer to the corresponding vertex (angle wise)
			// -> this vertex is on the diagonal axis of both triangles

			angle1 = abs(dockingSiteDIs_[i].Angle - corridorDIs_[lastCorrespondingIndex].Angle);
			if (angle1>M_PI) angle1 = 2*M_PI - angle1;	// smaller angle of 2PI
			angle2 = abs(dockingSiteDIs_[i-1].Angle  - corridorDIs_[dockingSiteDIs_[i].CorrespondingIndex].Angle);
			if (angle2>M_PI) angle2 = 2*M_PI - angle2;  // smaller angle of 2PI

			if (angle1<angle2)
			{
				// i and lastCorrespondingIndex are on the diagonal axis

				// add triangle:
				meshBuffer->Indices[bufferIndizes++] = bufferVertices-1;
				if (0 == end_)	// direction is based on the end of the corridor
				{
					meshBuffer->Indices[bufferIndizes++] = lastCorrespondingIndex;
					meshBuffer->Indices[bufferIndizes++] = bufferVertices-2;
				}
				else
				{
					meshBuffer->Indices[bufferIndizes++] = bufferVertices-2;
					meshBuffer->Indices[bufferIndizes++] = lastCorrespondingIndex;
				}
				// add triangle:
				meshBuffer->Indices[bufferIndizes++] = bufferVertices-1;
				if (0 == end_)	// direction is based on the end of the corridor
				{
					meshBuffer->Indices[bufferIndizes++] = dockingSiteDIs_[i].CorrespondingIndex;
					meshBuffer->Indices[bufferIndizes++] = lastCorrespondingIndex;
				}
				else
				{
					meshBuffer->Indices[bufferIndizes++] = lastCorrespondingIndex;
					meshBuffer->Indices[bufferIndizes++] = dockingSiteDIs_[i].CorrespondingIndex;
				}
			}
			else
			{
				// i-1 and the actual corresponding index are on the diagonal axis

				// add triangle:
				meshBuffer->Indices[bufferIndizes++] = bufferVertices-2;
				if (0 == end_)	// direction is based on the end of the corridor
				{
					meshBuffer->Indices[bufferIndizes++] = bufferVertices-1;
					meshBuffer->Indices[bufferIndizes++] = dockingSiteDIs_[i].CorrespondingIndex;					
				}
				else
				{					
					meshBuffer->Indices[bufferIndizes++] = dockingSiteDIs_[i].CorrespondingIndex;
					meshBuffer->Indices[bufferIndizes++] = bufferVertices-1;
				}
				// add triangle:
				meshBuffer->Indices[bufferIndizes++] = bufferVertices-2;
				if (0 == end_)	// direction is based on the end of the corridor
				{
					meshBuffer->Indices[bufferIndizes++] = dockingSiteDIs_[i].CorrespondingIndex;
					meshBuffer->Indices[bufferIndizes++] = lastCorrespondingIndex;
				}
				else
				{
					meshBuffer->Indices[bufferIndizes++] = lastCorrespondingIndex;
					meshBuffer->Indices[bufferIndizes++] = dockingSiteDIs_[i].CorrespondingIndex;
				}
			}

		} // END: inset triangles

		lastCorrespondingIndex = dockingSiteDIs_[i].CorrespondingIndex;

	} // END for

	// ----------------------------------
	// last triangles of the adapter
	// ----------------------------------

	// add 1 triangle?
	if (dockingSiteDIs_[0].CorrespondingIndex == lastCorrespondingIndex)
	{
		meshBuffer->Indices[bufferIndizes++] = bufferVertices-dockingSiteDIs_.size();

		if (0 == end_)	// direction is based on the end of the corridor
		{
			meshBuffer->Indices[bufferIndizes++] = dockingSiteDIs_[0].CorrespondingIndex;
			meshBuffer->Indices[bufferIndizes++] = bufferVertices-1;
		}
		else
		{
			meshBuffer->Indices[bufferIndizes++] = bufferVertices-1;
			meshBuffer->Indices[bufferIndizes++] = dockingSiteDIs_[0].CorrespondingIndex;
		}
	}
	// add 2 triangles?
	else		
	{
		// which of both vertices is nearer to the corresponding vertex (angle wise)
		// -> this vertex is on the diagonal axis of both triangles

		angle1 = abs(dockingSiteDIs_[0].Angle - corridorDIs_[lastCorrespondingIndex].Angle);
		if (angle1>M_PI) angle1 = 2*M_PI - angle1;	// smaller angle of 2PI
		angle2 = abs(dockingSiteDIs_.back().Angle  - corridorDIs_[dockingSiteDIs_[0].CorrespondingIndex].Angle);
		if (angle2>M_PI) angle2 = 2*M_PI - angle2;  // smaller angle of 2PI

		if (angle1<angle2)
		{
			// add triangle:
			meshBuffer->Indices[bufferIndizes++] = bufferVertices-dockingSiteDIs_.size(); // first docking site vertex

			if (0 == end_)	// direction is based on the end of the corridor
			{
				meshBuffer->Indices[bufferIndizes++] = lastCorrespondingIndex;
				meshBuffer->Indices[bufferIndizes++] = bufferVertices-1;
			}
			else
			{
				meshBuffer->Indices[bufferIndizes++] = bufferVertices-1;
				meshBuffer->Indices[bufferIndizes++] = lastCorrespondingIndex;
			}

			// add triangle:
			meshBuffer->Indices[bufferIndizes++] = bufferVertices-dockingSiteDIs_.size(); // first docking site vertex

			if (0 == end_)	// direction is based on the end of the corridor
			{
				meshBuffer->Indices[bufferIndizes++] = dockingSiteDIs_[0].CorrespondingIndex;
				meshBuffer->Indices[bufferIndizes++] = lastCorrespondingIndex;
			}
			else
			{
				meshBuffer->Indices[bufferIndizes++] = lastCorrespondingIndex;
				meshBuffer->Indices[bufferIndizes++] = dockingSiteDIs_[0].CorrespondingIndex;
			}
		}
		else
		{
			// add triangle:
			meshBuffer->Indices[bufferIndizes++] = bufferVertices-1; // last docking site vertex

			if (0 == end_)	// direction is based on the end of the corridor
			{
				meshBuffer->Indices[bufferIndizes++] = dockingSiteDIs_[0].CorrespondingIndex;
				meshBuffer->Indices[bufferIndizes++] = lastCorrespondingIndex;
			}
			else
			{				
				meshBuffer->Indices[bufferIndizes++] = lastCorrespondingIndex;
				meshBuffer->Indices[bufferIndizes++] = dockingSiteDIs_[0].CorrespondingIndex;
			}

			// add triangle:
			meshBuffer->Indices[bufferIndizes++] = bufferVertices-1; // last docking site vertex

			if (0 == end_)	// direction is based on the end of the corridor
			{
				meshBuffer->Indices[bufferIndizes++] = bufferVertices-dockingSiteDIs_.size();
				meshBuffer->Indices[bufferIndizes++] = dockingSiteDIs_[0].CorrespondingIndex;

			}
			else
			{
				meshBuffer->Indices[bufferIndizes++] = dockingSiteDIs_[0].CorrespondingIndex;
				meshBuffer->Indices[bufferIndizes++] = bufferVertices-dockingSiteDIs_.size();
			}
		}

	} // END: create triangles

	// compute final values of the mesh buffer
	meshBuffer->Vertices.reallocate(bufferVertices);
	meshBuffer->Indices.reallocate(bufferIndizes);
	meshBuffer->recalculateBoundingBox();

	// set final bounding box for the mesh
	MeshAdapter->recalculateBoundingBox();	
}

irr::scene::SMesh* DunGen::CAdapter::GetMesh()
{
	return MeshAdapter;
}