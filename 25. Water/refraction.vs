cbuffer MatrixBuffer{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer ClipPlaneBuffer{
    float4 clipPlane;
};

cbuffer LightBuffer{
    float4 diffuseColor;
    float4 ambientColor;
    float3 lightDirection;
    float padding;
};

struct VS_INPUT{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct VS_OUTPUT{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float diffuseLight : TEXCOORD1;
    float4 diffuseColor : TEXCOORD2;
    float4 ambientColor : TEXCOORD3;
    float clip : SV_ClipDistance0;
};

VS_OUTPUT vs_main(VS_INPUT input){
    VS_OUTPUT output;
    float3 worldNormal;

    input.position.w = 1.0f;

    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	output.tex = input.tex;

    worldNormal = mul(input.normal, worldMatrix);
    worldNormal = normalize(worldNormal);

    output.diffuseLight = saturate(dot(worldNormal, -lightDirection));
    output.diffuseColor = diffuseColor;
    output.ambientColor = ambientColor;
    
    // 클리핑 평면을 설정한다.
    output.clip = dot(mul(input.position, worldMatrix), clipPlane);

    return output;
}