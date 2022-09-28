cbuffer MatrixBuffer{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer RelfectionBuffer{
    // �ݻ� ������� ������ ��� ���۸� �����Ѵ�. 
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

    // �ݻ�� ���� ���� ����� �����Ѵ�.
    reflectProjectWorld = mul(reflectionMatrix, projectionMatrix);
    reflectProjectWorld = mul(worldMatrix, reflectProjectWorld);

    // �ݻ�� ���� ������Ŀ� ���߾� �ݻ�Ǵ� ��ġ�� ����Ѵ�. 
    output.reflectionPosition = mul(input.position, reflectProjectWorld);
    
    return output;
}