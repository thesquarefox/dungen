float4x4 mWorldViewProj;
float4 mOffset;
float3 mLightPos;

struct VertexShaderInput
{
	float4 Position : POSITION0;
	float3 Normal : NORMAL0;
	float2 TexCoord : TEXCOORD0;
};

struct VertexShaderOutput
{
	float4 PositionProj : POSITION0;
	float2 TexCoord : TEXCOORD0;
	float3 Normal : TEXCOORD1;
	float3 LightDirection: TEXCOORD2;
	float4 Position: TEXCOORD3;
};

VertexShaderOutput vertexMain(VertexShaderInput input)
{
	VertexShaderOutput output;	
	output.PositionProj = mul(input.Position, mWorldViewProj);
	output.Position = abs(input.Position - mOffset);
	output.TexCoord = input.TexCoord;
	output.Normal = input.Normal;
	output.LightDirection = mLightPos - input.Position.xyz;
	return output;
}

sampler2D mTexture;

float4 pixelMain(VertexShaderOutput input) : COLOR0
{
	float NdotL = dot(normalize(input.Normal),normalize(input.LightDirection));
	NdotL = abs(NdotL)+0.2;
	float4 part1 = (NdotL*tex2D(mTexture,input.TexCoord));
	float4 part2 = ((1.0-NdotL)*0.002* (input.Position));
	return (0.5*part1 + 0.5*part2);
}