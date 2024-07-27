// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#ifndef MATERIALCAVESINGLECOLOR_H
#define MATERIALCAVESINGLECOLOR_H

#include "interface/MaterialDunGen.h"
#include "Shader.h"
#include <iostream>

namespace DunGen
{
	class CMaterialCaveSingleColor : public CMaterialDunGen
	{
	public:
		/// set shader constants
		CMaterialCaveSingleColor(irr::IrrlichtDevice* irrDevice_)
		{
			IrrDevice = irrDevice_;
			OpenGLUsed = irrDevice_->getVideoDriver()->getDriverType() == irr::video::EDT_OPENGL;

			SetColor(irr::video::SColorf(1.0f,0.7f,0.4f,1.0f));
			// some useful values:
			// (1.0f,0.6f,0.2f,1.0f) sand cave
			// (0.0f,0.4f,0.8f,1.0f) ice cave

			std::cout << "[DunGen:] Creating material cave singlecolor ... ";

			// create material
			if (OpenGLUsed)
				Material = static_cast<irr::video::E_MATERIAL_TYPE>(
					irrDevice_->getVideoDriver()->getGPUProgrammingServices()->addHighLevelShaderMaterial(
					VertexShaderCaveSingleColor,
					"main",
					irr::video::EVST_VS_2_0,
					FragmentShaderCaveSingleColor,
					"main",
					irr::video::EPST_PS_2_0,
					this,
					irr::video::EMT_SOLID));
			else
				Material = static_cast<irr::video::E_MATERIAL_TYPE>(
					irrDevice_->getVideoDriver()->getGPUProgrammingServices()->addHighLevelShaderMaterial(
					ShaderCaveSingleColor_HLSL,
					"vertexMain",
					irr::video::EVST_VS_2_0,
					ShaderCaveSingleColor_HLSL,
					"pixelMain",
					irr::video::EPST_PS_2_0,
					this,
					irr::video::EMT_SOLID));

			std::cout << "done." << std::endl;
		}

		/// set cave color
		void SetColor(const irr::video::SColorf& color_)
		{
			CaveHalfColor = irr::video::SColorf(0.5f*color_.r,0.5f*color_.g,0.5f*color_.b,0.5f*color_.a);
		}

		/// set shader constants
		virtual void OnSetConstants(irr::video::IMaterialRendererServices* services_,
			irr::s32 userData_)
		{
			// camera position = position of the light source
			irr::core::vector3df pos = IrrDevice->getSceneManager()->getActiveCamera()->getAbsolutePosition();
			services_->setVertexShaderConstant("mLightPos", reinterpret_cast<irr::f32*>(&pos), 3);

			// pass over cave color (half of the color is passed to save a multiplication with 0.5 in the shader)
			services_->setPixelShaderConstant("mHalfColor",	reinterpret_cast<irr::f32*>(&CaveHalfColor), 4);

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
		irr::video::SColorf CaveHalfColor;		///< the half color (0.5 * color)
	};

} // END NAMESPACE DunGen

#endif