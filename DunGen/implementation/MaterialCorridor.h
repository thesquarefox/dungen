// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#ifndef MATERIALCORRIDOR_H
#define MATERIALCORRIDOR_H

#include "interface/MaterialDunGen.h"
#include "Shader.h"
#include <iostream>

namespace DunGen
{
	class CMaterialCorridor : public CMaterialDunGen
	{
	public:
		/// constructor
		CMaterialCorridor(irr::IrrlichtDevice* irrDevice_)
		{
			IrrDevice = irrDevice_;
			OpenGLUsed = irrDevice_->getVideoDriver()->getDriverType() == irr::video::EDT_OPENGL;

			std::cout << "[DunGen:] Creating material corridor ... ";

			// create material
			if (OpenGLUsed)
				Material = static_cast<irr::video::E_MATERIAL_TYPE>(
					irrDevice_->getVideoDriver()->getGPUProgrammingServices()->addHighLevelShaderMaterial(
					VertexShaderCorridor,
					"main",
					irr::video::EVST_VS_2_0,
					FragmentShaderCorridor,
					"main",
					irr::video::EPST_PS_2_0,
					this,
					irr::video::EMT_SOLID));
			else
				Material = static_cast<irr::video::E_MATERIAL_TYPE>(
					irrDevice_->getVideoDriver()->getGPUProgrammingServices()->addHighLevelShaderMaterial(
					ShaderCorridor_HLSL,
					"vertexMain",
					irr::video::EVST_VS_2_0,
					ShaderCorridor_HLSL,
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
			// camera position = position of the light source
			irr::core::vector3df pos = IrrDevice->getSceneManager()->getActiveCamera()->getAbsolutePosition();
			services_->setVertexShaderConstant("mLightPos", reinterpret_cast<irr::f32*>(&pos), 3);
		
			if (!OpenGLUsed)
			{
				irr::core::matrix4 worldViewProj;
				worldViewProj = IrrDevice->getVideoDriver()->getTransform(irr::video::ETS_PROJECTION);
				worldViewProj *= IrrDevice->getVideoDriver()->getTransform(irr::video::ETS_VIEW);
				worldViewProj *= IrrDevice->getVideoDriver()->getTransform(irr::video::ETS_WORLD);
				services_->setVertexShaderConstant("mWorldViewProj", worldViewProj.pointer(), 16);
			}
		}

	private:
		irr::IrrlichtDevice* IrrDevice;			///< the irrlicht device
		bool OpenGLUsed;						///< Is OpenGL used (otherwise DirectX)?
	};

} // END NAMESPACE DunGen

#endif