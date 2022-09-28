cbuffer MatrixBuffer{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer CameraBuffer{
    float3 cameraPosition;
	float padding;
};

struct VS_INPUT{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;   // 법선
    float3 tangent : TANGENT;   // 접선 
    float3 binormal : BINORMAL;   // 종법선
};

struct VS_OUTPUT{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL; 
    float3 tangent : TANGENT; 
    float3 binormal : BINORMAL;
    float3 viewDirection : TEXCOORD1;
    float3 worldPosition : TEXCOORD2;
};

VS_OUTPUT vs_main(VS_INPUT input){
    VS_OUTPUT output;

    input.position.w = 1.0f;

    output.position = mul(input.position, worldMatrix);
    output.viewDirection = cameraPosition - output.position;
    output.worldPosition = output.position;

    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	output.tex = input.tex;

   float3 worldNormal = mul(input.normal, (float3x3)worldMatrix);
   output.normal = normalize(worldNormal);

   float3 worldTangent = mul(input.tangent, (float3x3)worldMatrix);
   output.tangent = normalize(worldTangent);

   float3 worldBinormal = mul(input.binormal, (float3x3)worldMatrix);
   output.binormal = normalize(worldBinormal);


    return output;
}