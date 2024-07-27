float4x4 mWorldViewProj;
float4 mOffset;
float3 mLightPos;

struct VertexShaderInput
{
	float4 Position : POSITION0;
	float3 Normal : NORMAL0;
};

struct VertexShaderOutput
{
	float4 PositionProj : POSITION0;
	float3 Normal : TEXCOORD0;
	float3 LightDirection: TEXCOORD1;
	float4 Position: TEXCOORD2;
};

VertexShaderOutput vertexMain(VertexShaderInput input)
{
	VertexShaderOutput output;	
	output.PositionProj = mul(input.Position, mWorldViewProj);
	output.Position = abs(input.Position - mOffset);
	output.Normal = input.Normal;
	output.LightDirection = mLightPos - input.Position.xyz;
	return output;
}

float4 pixelMain(VertexShaderOutput input) : COLOR0
{
	float NdotL = dot(normalize(input.Normal),normalize(input.LightDirection));
	return ((1.0-NdotL)*0.002* (input.Position));
}