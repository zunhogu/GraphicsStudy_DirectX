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
    
    // Ŭ���� ����� �����Ѵ�. 
    // Ŭ���� ����� �����ϱ� ���ؼ��� ���� ������ ������� Ŭ���� ��鿡 ������ �����Ѵ�.
    // �׷��� �Ͽ� Ŭ���� ����� Ȱ��ȭ�ǰ� �ȼ����̴��� Ŭ���� ����� �ø��� �͵��� �׸��� �ʰԵȴ�.
    output.clip = dot(mul(input.position, worldMatrix), clipPlane);


    return output;
}