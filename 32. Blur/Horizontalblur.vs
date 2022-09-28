cbuffer MatrixBuffer{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

// �� ���̴��� �ؼ��� ���� UV ��ġ�� ����ϱ� ���� ȭ���� �ʺ� or RTT�� �ʺ� �ʿ��ϴ�
// �Ʒ��� ������۷� �ʺ��� �޾ƿ´�.
cbuffer ScreenSizeBuffer{
	float screenWidth;
	float3 padding;
};

struct VS_INPUT{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct VS_OUTPUT{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float2 texCoord1 : TEXCOORD1;   // ���� �ȼ�4
	float2 texCoord2 : TEXCOORD2;   // ���� �ȼ�3
	float2 texCoord3 : TEXCOORD3;    // ���� �ȼ�2
	float2 texCoord4 : TEXCOORD4;   // ���� �ȼ�1
	float2 texCoord5 : TEXCOORD5;   // ��� �ȼ�
	float2 texCoord6 : TEXCOORD6;   // ������ �ȼ�1
	float2 texCoord7 : TEXCOORD7;   // ������ �ȼ�2
	float2 texCoord8 : TEXCOORD8;   // ������ �ȼ�3
	float2 texCoord9 : TEXCOORD9;   // ������ �ȼ�4
};

VS_OUTPUT vs_main(VS_INPUT input){
    VS_OUTPUT output;
	float texelSize;

    input.position.w = 1.0f;

    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	output.tex = input.tex;
    
	// 1�� ȭ���� �ʺ�� ������ �ؼ��� ũ��(offset)�� ���Ѵ�.�� ���� �̿��Ͽ� ���� �ȼ��� UV��ǥ�� ���Ҽ� �ִ�.
	texelSize = 1.0f / screenWidth;

	// ��� �ȼ��� �¿�� ������ �� 4���� �ȼ��� ���� UV��ǥ�� ����Ѵ�.
	// ���� �ؽ�ó�� ��ǥ�� ������ ���� offset���� ���ذ����ν� 9���� ��ǥ�� ��� ���Ѵ�.
	output.texCoord1 = input.tex + float2(texelSize * -4.0f, 0.0f);
	output.texCoord2 = input.tex + float2(texelSize * -3.0f, 0.0f);
	output.texCoord3 = input.tex + float2(texelSize * -2.0f, 0.0f);
	output.texCoord4 = input.tex + float2(texelSize * -1.0f, 0.0f);
	output.texCoord5 = input.tex + float2(texelSize *  0.0f, 0.0f);
	output.texCoord6 = input.tex + float2(texelSize *  1.0f, 0.0f);
	output.texCoord7 = input.tex + float2(texelSize *  2.0f, 0.0f);
	output.texCoord8 = input.tex + float2(texelSize *  3.0f, 0.0f);
	output.texCoord9 = input.tex + float2(texelSize *  4.0f, 0.0f);

    return output;
}