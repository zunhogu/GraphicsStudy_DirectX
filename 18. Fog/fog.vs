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
    
    // ������ ��ġ�� ������� �������� ī�޶���� ������ ��� ��ġ�� �ִ��� �˾ƾ��ϱ� �����̴�.
    dist = mul(input.position, worldMatrix);
    dist = mul(dist, viewMatrix);

    // �Ȱ����� ���
    // dist.z �� �Ÿ��� �ǹ��Ѵ�.
    output.fogFactor = saturate((fogEnd - dist.z) / (fogEnd - fogStart));

    return output;
}