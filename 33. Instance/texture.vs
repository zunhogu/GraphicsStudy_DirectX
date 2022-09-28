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

	// ��� ������ ���� ��ġ������ ���Ҹ� 4���� �ٲ۴�. 
    input.position.w = 1.0f;
    
    // �Է¹��� �ν��Ͻ� ������ �̿��Ͽ� �ﰢ���� �׷�����ġ�� �����Ѵ�.
    input.position.x += input.instancePosition.x;
    input.position.y += input.instancePosition.y;
    input.position.z += input.instancePosition.z;

	// ������ȯ
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// �ؽ�ó uv��ǥ ����
	output.tex = input.tex;
    
    return output;
}