// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#ifndef SHADER_H
#define SHADER_H

namespace DunGen
{
	// note: light position is camera position in these shaders

	static const char * VertexShaderCorridor =
	"uniform vec3 mLightPos;"										// position of the camera
	"varying vec3 normal,lightDir;"									// normal and direction to the camera

	"void main(void)"
	"{"	
		"gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"	// transform position
		"normal = gl_Normal;"										// pass normal
		"lightDir = mLightPos - gl_Vertex.xyz;"						// compute direction to light
		"gl_TexCoord[0] = gl_MultiTexCoord0;"						// pass texture coordinate
	"}";

	static const char * FragmentShaderCorridor =
	"uniform sampler2D mTexture;"											// the texture
	"varying vec3 normal,lightDir;"											// normal and direction to the camera

	"void main (void)"
	"{"
		"float NdotL = dot(normalize(normal),normalize(lightDir));"			// compute N * L
		"NdotL = abs(NdotL) + 0.3;"											// lighted from both sides
		"gl_FragColor = NdotL * texture2D(mTexture, vec2(gl_TexCoord[0]));"	// compute the final color
	"}";

	static const char * VertexShaderCaveSingleColor =
	"uniform vec3 mLightPos;"										// position of the camera
	"varying vec3 normal,lightDir;"									// normal and direction to the camera

	"void main(void)"
	"{"	
		"gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"	// transform position
		"normal = gl_Normal;"										// pass normal
		"lightDir = mLightPos - gl_Vertex.xyz;"						// compute direction to light
	"}";

	static const char * FragmentShaderCaveSingleColor =
	"uniform vec4 mHalfColor;"										// half of the color value
	"varying vec3 normal,lightDir;"									// normal and direction to the camera

	"void main (void)"
	"{"
		"float NdotL = dot(normalize(normal),normalize(lightDir));"	// compute N * L
		"gl_FragColor = (1.0-NdotL)*mHalfColor;"					// compute final color: interpolation over 180 degrees instead of usual 90 degrees
	"}";

	static const char * VertexShaderCaveMultiColor =
	"uniform vec3 mLightPos;"
	"varying vec3 normal,lightDir;"									// normal and direction to the camera
	"varying vec4 position;"										// position of the 3D sampling point

	"void main(void)"
	"{"	
		"gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"	// transform position
		"normal = gl_Normal;"										// pass normal
		"position = gl_Vertex;"										// pass position
		"lightDir = mLightPos - gl_Vertex.xyz;"						// compute direction to light
	"}";

	static const char * FragmentShaderCaveMultiColor =
	"varying vec3 normal,lightDir;"									// normal and direction to the camera
	"varying vec4 position;"										// position of the 3D sampling point

	"void main (void)"
	"{"
		"float NdotL = dot(normalize(normal),normalize(lightDir));"	// compute N * L
		"gl_FragColor = (1.0-NdotL)*0.002*position;"				// compute final color: interpolation over 180 degrees instead of usual 90 degrees, using position as RGB base
	"}";

	static const char * ShaderCorridor_HLSL =
	"float4x4 mWorldViewProj;"										// World * View * Projection transformation
	"float3 mLightPos;"												// Light position

	"struct VertexShaderInput"
	"{"
		"float4 Position : POSITION0;"
		"float3 Normal : NORMAL0;"
		"float2 TexCoord : TEXCOORD0;"
	"};"

	"struct VertexShaderOutput"
	"{"
		"float4 PositionProj : POSITION0;"
		"float2 TexCoord : TEXCOORD0;"
		"float3 Normal : TEXCOORD1;"
		"float3 LightDirection: TEXCOORD2;"
	"};"

	"VertexShaderOutput vertexMain(VertexShaderInput input)"
	"{"
		"VertexShaderOutput output;	"
		"output.PositionProj = mul(input.Position, mWorldViewProj);"	// transform position to clip space
		"output.TexCoord = input.TexCoord;"								// pass texture coordinates
		"output.Normal = input.Normal;"									// pass normal
		"output.LightDirection = mLightPos - input.Position.xyz;"		// compute light direction
		"return output;"
	"}"

	"sampler2D mTexture;"																// the texture

	"float4 pixelMain(VertexShaderOutput input) : COLOR0"
	"{"
		"float NdotL = dot(normalize(input.Normal),normalize(input.LightDirection));"	// compute N * L
		"NdotL = abs(NdotL) + 0.3;"														// lighted from both sides
		"return (NdotL*tex2D(mTexture,input.TexCoord));"								// compute the final color
	"}";

	static const char * ShaderCaveSingleColor_HLSL =
	"float4x4 mWorldViewProj;"										// World * View * Projection transformation
	"float3 mLightPos;"												// Light position
	"float4 mHalfColor;"											// half of the color value

	"struct VertexShaderInput"
	"{"
		"float4 Position : POSITION0;"
		"float3 Normal : NORMAL0;"
	"};"

	"struct VertexShaderOutput"
	"{"
		"float4 PositionProj : POSITION0;"
		"float3 Normal : TEXCOORD0;"
		"float3 LightDirection: TEXCOORD1;"
	"};"

	"VertexShaderOutput vertexMain(VertexShaderInput input)"
	"{"
		"VertexShaderOutput output;	"
		"output.PositionProj = mul(input.Position, mWorldViewProj);"// transform position to clip space
		"output.Normal = input.Normal;"								// pass normal
		"output.LightDirection = mLightPos - input.Position.xyz;"	// compute light direction
		"return output;"
	"}"

	"float4 pixelMain(VertexShaderOutput input) : COLOR0"
	"{"
		"float NdotL = dot(normalize(input.Normal),normalize(input.LightDirection));"	// compute N * L
		"return ((1.0-NdotL)*mHalfColor);"												// compute the final color
	"}";


	static const char * ShaderCaveMultiColor_HLSL =
	"float4x4 mWorldViewProj;"										// World * View * Projection transformation
	"float3 mLightPos;"												// Light position

	"struct VertexShaderInput"
	"{"
		"float4 Position : POSITION0;"
		"float3 Normal : NORMAL0;"
	"};"

	"struct VertexShaderOutput"
	"{"
		"float4 PositionProj : POSITION0;"
		"float3 Normal : TEXCOORD0;"
		"float3 LightDirection: TEXCOORD1;"
		"float4 Position: TEXCOORD2;"
	"};"

	"VertexShaderOutput vertexMain(VertexShaderInput input)"
	"{"
		"VertexShaderOutput output;	"
		"output.PositionProj = mul(input.Position, mWorldViewProj);"// transform position to clip space
		"output.Position = input.Position;"							// pass position
		"output.Normal = input.Normal;"								// pass normal
		"output.LightDirection = mLightPos - input.Position.xyz;"	// compute light direction
		"return output;"
	"}"

	"float4 pixelMain(VertexShaderOutput input) : COLOR0"
	"{"
		"float NdotL = dot(normalize(input.Normal),normalize(input.LightDirection));"	// compute N * L
		"return ((1.0-NdotL)*0.002*input.Position);"									// compute the final color
	"}";

} // END NAMESPACE DunGen

#endif