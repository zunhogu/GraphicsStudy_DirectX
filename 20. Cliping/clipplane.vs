cbuffer MatrixBuffer{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer ClipPlaneBuffer { 
    float4 clipPlane; 
};


struct VS_INPUT{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct VS_OUTPUT{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float clip : SV_ClipDistance0;
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
    
    // 클리핑 평면을 설정한다. 
    // 클리핑 평면을 설정하기 위해서는 월드 공간의 정점들과 클리핑 평면에 내적을 수행한다.
    // 그렇게 하여 클리핑 평면이 활성화되고 픽셀셰이더는 클리핑 평면이 컬링한 것들을 그리지 않게된다.
    output.clip = dot(mul(input.position, worldMatrix), clipPlane);


    return output;
}