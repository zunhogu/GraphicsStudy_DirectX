cbuffer MatrixBuffer{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer NoiseBuffer {
    float frameTime;   // �� �����Ӹ��� ���ŵǾ� ������ �ؽ�ó���� ��ũ���ϴ� ����
    float3 scrollSpeeds;   // ��ũ���� �ӵ��� �����ϴ� ����  
    float3 scales;    // �� ������ �ؽ�ó�� ������ ��(�Ǵ� ��Ÿ�� ��) �Ϲ������� x y z ���� 1 2 3 �� �����
    float padding; 
};



struct VS_INPUT{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct VS_OUTPUT{
    float4 position : SV_POSITION; 
    float2 tex : TEXCOORD0;
    float2 texCoords1 : TEXCOORD1; 
    float2 texCoords2 : TEXCOORD2;
    float2 texCoords3 : TEXCOORD3;
};


VS_OUTPUT vs_main(VS_INPUT input){
    VS_OUTPUT output;
    
    input.position.w = 1.0f;

    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	output.tex = input.tex;

    // input.tex�� sacle.x�� ������ ��Ÿ�긦 ����� y������ ��ũ�ѵǴ� ��ǥ�� ����Ѵ�.
    // �� ��ǥ�� �ȼ����̴����� ������ �ؽ�ó�� ���ø��ǰ� ���ø� �� ������ �ְ�� �ؽ�ó ��ǥ�� �������̴�.

    // ù��° ������ �ؽ����� ��ǥ�� ù��° ũ�� �� ������ ��ũ�� �ӵ� ���� �̿��Ͽ� ����Ѵ�.
    output.texCoords1 = (input.tex * scales.x);
    output.texCoords1.y = output.texCoords1.y + (frameTime * scrollSpeeds.x);

    // �ι�° ������ �ؽ����� ��ǥ�� �ι�° ũ�� �� ������ ��ũ�� �ӵ� ���� �̿��Ͽ� ����Ѵ�.
    output.texCoords2 = (input.tex * scales.y);
    output.texCoords2.y = output.texCoords2.y + (frameTime * scrollSpeeds.y);

    // ����° ������ �ؽ����� ��ǥ�� ����° ũ�� �� ������ ��ũ�� �ӵ� ���� �̿��Ͽ� ����Ѵ�.
    output.texCoords3 = (input.tex * scales.z);
    output.texCoords3.y = output.texCoords3.y + (frameTime * scrollSpeeds.z);

    return output;
}