//--------------------------------------------------------------------------------------
// File: DX11 Framework.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------

Texture2D txDiffuse : register( t0 );
SamplerState samLinear : register( s0 );

cbuffer ConstantBuffer : register( b0 )
{
	matrix World;
	matrix View;
	matrix Projection;
	float4 DiffuseMtrl;
	float4 DiffuseLight;
	float4 AmbientMtrl;
	float4 AmbientLight;
	float4 SpecularMtrl;
	float4 SpecularLight;
	float SpecularPower;
	float3 pad = { 0.0f, 0.0f, 0.0f };
	float3 EyePosW;
	float pad3 = 0.0f;
	float3 LightVecW;
	float gTime;
	float gFogStart;
	float gFogRange;
	float2 pad2 = { 0.0f, 0.0f };
	float4 gFogColor;
	float gFogEnabled;
}

//--------------------------------------------------------------------------------------

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;

	float3 Norm : NORMAL;
	float3 PosW : POSITION;

	float2 Tex : TEXCOORD0;
	float S : FOG;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS( float4 Pos : POSITION, float3 NormalL : NORMAL, float2 Tex : TEXCOORD0)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

	//Pos.xy += 0.5f * sin(Pos.x) * sin(3.0f * gTime);
	//Pos.z *= 0.6f + 0.4f * sin(2.0f * gTime);

    output.Pos = mul( Pos, World );
	float3 toEye = normalize(EyePosW - output.Pos.xyz);

    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );

	float3 normalW = mul(float4(NormalL, 0.0f), World).xyz;
		normalW = normalize(normalW);

	
	output.S = saturate((distance(output.Pos.xyz, toEye) - gFogStart) / gFogRange);
	output.Norm = NormalL;
	output.PosW = Pos;
	output.Tex = Tex;
	//output.fogFactor = saturate((gFogRange - toEye.z) / (gFogRange - gFogStart));

    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input) : SV_Target
{
	input.Norm = normalize(input.Norm);
	float3 toEye = normalize(EyePosW - input.PosW.xyz);

		float distToEye = length(toEye);
	toEye /= distToEye;

	float3 normalW = mul(float4(input.Norm, 0.0f), World).xyz;
	normalW = normalize(normalW);

	float s = input.S;
	

	float4 texColor = float4(1, 1, 1, 1);
		texColor = txDiffuse.Sample(samLinear, input.Tex);

		clip(texColor.a - 0.1f);


	float4 litColor = texColor;

	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	ambient = AmbientMtrl * AmbientLight;
	
	float diffuseAmount = max(dot(LightVecW, normalW), 0.0f);
	

	[flatten]
	if (diffuseAmount > 0.0f)
	{
		float3 r = reflect(-LightVecW, normalW);
		float specularAmount = pow(max(dot(r, toEye), 0.0f), SpecularPower);


		specular = specularAmount * SpecularMtrl * SpecularLight;
		diffuse = diffuseAmount* (DiffuseMtrl * DiffuseLight);
	}

		litColor = texColor  * (ambient + diffuse) + specular;

	
		if (gFogEnabled == 1.0f)
		{
				litColor = litColor + s *(gFogColor - litColor);
		}

	litColor.a = DiffuseMtrl.a * texColor.a;
	return litColor;
}
