cbuffer MatrixBuffer{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer LightBuffer{
    float4 diffuseColor;
    float3 lightDirection;
    float4 ambientColor;
    float padding;
};

struct VertexInputType{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct PixelInputType{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float diffuseLight : TEXCOORD1;
    float4 diffuseColor : TEXCOORD2;
    float4 ambientColor : TEXCOORD3;
};

PixelInputType LightVertexShader(VertexInputType input){
    PixelInputType output;
    float3 worldNormal;
	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;

    worldNormal = mul(input.normal, worldMatrix);
    worldNormal = normalize(worldNormal);

    output.diffuseLight = saturate(dot(worldNormal, -lightDirection));
    output.diffuseColor = diffuseColor;
    output.ambientColor = ambientColor;
    
    return output;
}