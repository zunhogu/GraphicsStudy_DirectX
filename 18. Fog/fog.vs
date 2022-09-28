cbuffer MatrixBuffer{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer FogBuffer{
    float fogStart;
    float fogEnd;
}

struct VS_INPUT{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct VS_OUTPUT{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float fogFactor : FOG;
};

VS_OUTPUT vs_main(VS_INPUT input){
    VS_OUTPUT output;
    float4 dist;
    
	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;
    
    // 정점의 위치를 뷰공간에 둔이유는 카메라기준 정점이 어느 위치에 있는지 알아야하기 때문이다.
    dist = mul(input.position, worldMatrix);
    dist = mul(dist, viewMatrix);

    // 안개인자 계산
    // dist.z 는 거리를 의미한다.
    output.fogFactor = saturate((fogEnd - dist.z) / (fogEnd - fogStart));

    return output;
}