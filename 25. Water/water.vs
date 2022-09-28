cbuffer MatrixBuffer{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer ReflectionBuffer{
	matrix reflectionMatrix;
}

struct VS_INPUT{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct VS_OUTPUT{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float4 reflectionPosition : TEXCOORD1;
    float4 refractionPosition : TEXCOORD2;
};

VS_OUTPUT vs_main(VS_INPUT input){

    VS_OUTPUT output;

    // �ݻ� ���
    matrix reflectProjectWorld;
    // ���� ���
    matrix refractProjectWorld;

    input.position.w = 1.0f;

    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	output.tex = input.tex;
    
    // �ݻ� ������� ������ �ݻ� ��������� ����� ȭ�鿡 �׷��� ��ġ�� ����Ѵ�.
    reflectProjectWorld = mul(reflectionMatrix, projectionMatrix);
    reflectProjectWorld = mul(worldMatrix, reflectProjectWorld);

    output.reflectionPosition = mul(input.position, reflectProjectWorld);

    // ������� ������ ���� ��������� ����� ȭ�鿡 �׷��� ��ġ�� ����Ѵ�.
    refractProjectWorld = mul(viewMatrix, projectionMatrix);
    refractProjectWorld = mul(worldMatrix, refractProjectWorld);

    output.refractionPosition = mul(input.position, refractProjectWorld);

    return output;
}