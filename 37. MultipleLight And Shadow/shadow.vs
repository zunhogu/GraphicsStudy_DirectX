cbuffer MatrixBuffer{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
    matrix lightViewMatrix; 
    matrix lightProjectionMatrix;
    matrix lightViewMatrix2; 
    matrix lightProjectionMatrix2;
};

cbuffer LightBuffer {
    float3 lightPosition; 
    float padding;
    float3 lightPosition2; 
    float padding2;
};

struct VS_INPUT{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct VS_OUTPUT{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 worldNormal : NORMAL;
    float4 lightViewPosition : TEXCOORD1;   // ���� ������ ���� ��ġ
    float3 lightDir : TEXCOORD2;   // ������� �� ���� ����
    float4 lightViewPosition2 : TEXCOORD3;
    float3 lightDir2 : TEXCOORD4;
};

VS_OUTPUT vs_main(VS_INPUT input){
    VS_OUTPUT output;
    float4 worldPosition;

    // ������ǥ�� ��ġ�������� ǥ��
    input.position.w = 1.0f;

    // �Ϲ� ī�޶� �������� ����ۿ� ������ ������ġ ��� 
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // ����1 ���������� �ؽ�ó�� ������ ������ġ ���
    output.lightViewPosition = mul(input.position, worldMatrix); 
    output.lightViewPosition = mul(output.lightViewPosition, lightViewMatrix); 
    output.lightViewPosition = mul(output.lightViewPosition, lightProjectionMatrix);

    // ����2 �������� �ؽ�ó���� ���ۿ� ������ ������ġ ���
    output.lightViewPosition2 = mul(input.position, worldMatrix); 
    output.lightViewPosition2 = mul(output.lightViewPosition2, lightViewMatrix2); 
    output.lightViewPosition2 = mul(output.lightViewPosition2, lightProjectionMatrix2);

    // �ؽ�ó��ǥ ����
	output.tex = input.tex;

    // ���� ���� ���� ���
    output.worldNormal = normalize(mul(input.normal, (float3x3)worldMatrix));

    // ���� ���������� ������ ��ġ�� ���� ���
    worldPosition = mul(input.position, worldMatrix);
    
    // ���� ���� ��ġ�� ������ ����1 ���� ���
    output.lightDir = normalize(lightPosition.xyz - worldPosition.xyz);

    // ����2 ���� ���
    output.lightDir2 = normalize(lightPosition2.xyz - worldPosition.xyz);

    return output;
}