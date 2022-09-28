////////////////////////////////////////////////////////////////////////////////
cbuffer MatrixBuffer{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VS_INPUT{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 instancePosition : TEXCOORD1;
};

struct PS_INPUT{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};


PS_INPUT vs_main(VS_INPUT input){
    PS_INPUT output;

	// 행렬 연산을 위해 위치벡터의 원소를 4개로 바꾼다. 
    input.position.w = 1.0f;
    
    // 입력받은 인스턴스 정보를 이용하여 삼각형이 그려질위치를 수정한다.
    input.position.x += input.instancePosition.x;
    input.position.y += input.instancePosition.y;
    input.position.z += input.instancePosition.z;

	// 공간변환
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// 텍스처 uv좌표 설정
	output.tex = input.tex;
    
    return output;
}