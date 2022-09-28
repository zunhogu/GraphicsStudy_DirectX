#define NUM_LIGHTS 4

cbuffer MatrixBuffer{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer LightPositionBuffer{
    float4 lightPosition[NUM_LIGHTS];
}

struct VS_INPUT{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct VS_OUTPUT{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 lightDir1 : TEXCOORD1;
    float3 lightDir2 : TEXCOORD2;
    float3 lightDir3 : TEXCOORD3;
    float3 lightDir4 : TEXCOORD4;
};

VS_OUTPUT vs_main(VS_INPUT input){
    VS_OUTPUT output;
    float4 worldPosition;

    input.position.w = 1.0f;

    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	output.tex = input.tex;

    output.normal = mul(input.normal, (float3x3)worldMatrix);
    output.normal = normalize(output.normal);
    
    worldPosition = mul(input.position, worldMatrix);

    output.lightDir1.xyz = lightPosition[0].xyz - worldPosition.xyz;
    output.lightDir2.xyz = lightPosition[1].xyz - worldPosition.xyz;
    output.lightDir3.xyz = lightPosition[2].xyz - worldPosition.xyz;
    output.lightDir4.xyz = lightPosition[3].xyz - worldPosition.xyz;

    output.lightDir1 = normalize(output.lightDir1);
    output.lightDir2 = normalize(output.lightDir2); 
    output.lightDir3 = normalize(output.lightDir3);
    output.lightDir4 = normalize(output.lightDir4);
    
    return output;
}