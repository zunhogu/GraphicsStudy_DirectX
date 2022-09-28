cbuffer MatrixBuffer{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
    matrix lightViewMatrix; 
    matrix lightProjectionMatrix;
    matrix lightViewMatrix2; 
    matrix lightProjectionMatrix2;
};

cbuffer LightBuffer {
    float3 lightPosition; 
    float padding;
    float3 lightPosition2; 
    float padding2;
};

struct VS_INPUT{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct VS_OUTPUT{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 worldNormal : NORMAL;
    float4 lightViewPosition : TEXCOORD1;   // 광원 시점의 정점 위치
    float3 lightDir : TEXCOORD2;   // 월드공간 속 광원 벡터
    float4 lightViewPosition2 : TEXCOORD3;
    float3 lightDir2 : TEXCOORD4;
};

VS_OUTPUT vs_main(VS_INPUT input){
    VS_OUTPUT output;
    float4 worldPosition;

    // 동차좌표가 위치벡터임을 표시
    input.position.w = 1.0f;

    // 일반 카메라 시점에서 백버퍼에 투영할 정점위치 계산 
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // 광원1 시점에서의 텍스처에 투영할 정점위치 계산
    output.lightViewPosition = mul(input.position, worldMatrix); 
    output.lightViewPosition = mul(output.lightViewPosition, lightViewMatrix); 
    output.lightViewPosition = mul(output.lightViewPosition, lightProjectionMatrix);

    // 광원2 시점에서 텍스처에서 백퍼에 투영할 정점위치 계산
    output.lightViewPosition2 = mul(input.position, worldMatrix); 
    output.lightViewPosition2 = mul(output.lightViewPosition2, lightViewMatrix2); 
    output.lightViewPosition2 = mul(output.lightViewPosition2, lightProjectionMatrix2);

    // 텍스처좌표 저장
	output.tex = input.tex;

    // 월드 법선 벡터 계산
    output.worldNormal = normalize(mul(input.normal, (float3x3)worldMatrix));

    // 월드 공간에서의 정점의 위치만 따로 계산
    worldPosition = mul(input.position, worldMatrix);
    
    // 월드 정점 위치를 가지고 광원1 벡터 계산
    output.lightDir = normalize(lightPosition.xyz - worldPosition.xyz);

    // 광원2 벡터 계산
    output.lightDir2 = normalize(lightPosition2.xyz - worldPosition.xyz);

    return output;
}