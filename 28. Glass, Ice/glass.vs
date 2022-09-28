cbuffer MatrixBuffer{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};


struct VS_INPUT{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct VS_OUTPUT{
    float4 position : SV_POSITION; 
    float2 tex : TEXCOORD0; 
    float4 refractionPosition : TEXCOORD1;
};

VS_OUTPUT vs_main(VS_INPUT input){

    VS_OUTPUT output;
    
    // 굴절 행렬
    matrix refractProjectWorld;

    input.position.w = 1.0f;

    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	output.tex = input.tex;

    // 뷰행렬을 가지고 굴절 투영행렬을 만든뒤 화면에 그려질 위치를 계산한다.
    refractProjectWorld = mul(viewMatrix, projectionMatrix);
    refractProjectWorld = mul(worldMatrix, refractProjectWorld);

    output.refractionPosition = mul(input.position, refractProjectWorld);

    return output;
}