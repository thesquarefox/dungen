// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#ifndef MATERIALCAVECUSTOM_H
#define MATERIALCAVECUSTOM_H

#include <DunGen.h>
#include <iostream>
#include <time.h>

class CMaterialCaveCustom : public DunGen::CMaterialDunGen
{
public:
	/// set shader constants
	CMaterialCaveCustom(irr::IrrlichtDevice* irrDevice_)
	{
		IrrDevice = irrDevice_;
		Offset[3] = 0.0f;

		std::cout << "Creating material cave custom ... ";

		// create material
		Material = static_cast<irr::video::E_MATERIAL_TYPE>(
			irrDevice_->getVideoDriver()->getGPUProgrammingServices()->addHighLevelShaderMaterialFromFiles(
			"data/CaveTut03.shader",
			"vertexMain",
			irr::video::EVST_VS_2_0,
			"data/CaveTut03.shader",
			"pixelMain",
			irr::video::EPST_PS_2_0,
			this,
			irr::video::EMT_SOLID));

		std::cout << "done." << std::endl;
	}

	/// set shader constants
	virtual void OnSetConstants(irr::video::IMaterialRendererServices* services_,
		irr::s32 userData_)
	{
		// compute color position offsets
		Offset[0] = static_cast<float>(static_cast<int>(clock()/7)%1024);
		if (Offset[0] > 512.0f)
			Offset[0] = 1024.0f - Offset[0];
		Offset[1] = static_cast<float>(static_cast<int>(clock()/11)%1024);
		if (Offset[1] > 512.0f)
			Offset[1] = 1024.0f - Offset[1];
		Offset[2] = static_cast<float>(static_cast<int>(clock()/13)%1024);
		if (Offset[2] > 512.0f)
			Offset[2] = 1024.0f - Offset[2];

		// camera position = position of the light source
		irr::core::vector3df pos = IrrDevice->getSceneManager()->getActiveCamera()->getAbsolutePosition();
		services_->setVertexShaderConstant("mLightPos", reinterpret_cast<irr::f32*>(&pos), 3);
		services_->setVertexShaderConstant("mOffset", reinterpret_cast<irr::f32*>(&Offset), 4);

		// set world-view-projection matrix
		irr::core::matrix4 worldViewProj;
		worldViewProj = IrrDevice->getVideoDriver()->getTransform(irr::video::ETS_PROJECTION);
		worldViewProj *= IrrDevice->getVideoDriver()->getTransform(irr::video::ETS_VIEW);
		worldViewProj *= IrrDevice->getVideoDriver()->getTransform(irr::video::ETS_WORLD);
		services_->setVertexShaderConstant("mWorldViewProj", worldViewProj.pointer(), 16);
	}

private:
	irr::IrrlichtDevice* IrrDevice;			///< the irrlicht device
	irr::f32 Offset[4];						///< current color position offsets
};

#endif