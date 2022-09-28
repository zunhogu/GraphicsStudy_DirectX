cbuffer MatrixBuffer{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer RelfectionBuffer{
    // 반사 뷰행렬을 저장할 상수 버퍼를 설정한다. 
	matrix reflectionMatrix;
};

struct VS_INPUT{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct VS_OUTPUT{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float4 reflectionPosition : TEXCOORD1;
};

VS_OUTPUT vs_main(VS_INPUT input){
    VS_OUTPUT output;
	matrix reflectProjectWorld;

	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;

    // 반사뷰 투영 월드 행렬을 생성한다.
    reflectProjectWorld = mul(reflectionMatrix, projectionMatrix);
    reflectProjectWorld = mul(worldMatrix, reflectProjectWorld);

    // 반사뷰 투영 월드행렬에 비추어 반사되는 위치를 계산한다. 
    output.reflectionPosition = mul(input.position, reflectProjectWorld);
    
    return output;
}