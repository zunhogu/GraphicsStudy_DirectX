struct VS_INPUT{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct VS_OUTPUT{
    float3 position : POSITION;
    float4 color : COLOR;
};

VS_OUTPUT vs_main(VS_INPUT input){
    VS_OUTPUT output;

    // ������ ���� ������ȯ ��Ű�� �ʰ� �� ���̴��� �����Ѵ�.
    output.position = input.position;
    
    // ������ ����
    output.color = input.color;
    
    return output;
}