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
    
    // ���� ���
    matrix refractProjectWorld;

    input.position.w = 1.0f;

    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	output.tex = input.tex;

    // ������� ������ ���� ��������� ����� ȭ�鿡 �׷��� ��ġ�� ����Ѵ�.
    refractProjectWorld = mul(viewMatrix, projectionMatrix);
    refractProjectWorld = mul(worldMatrix, refractProjectWorld);

    output.refractionPosition = mul(input.position, refractProjectWorld);

    return output;
}