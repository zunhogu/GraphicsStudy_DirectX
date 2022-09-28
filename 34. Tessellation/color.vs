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

    // 정점을 아직 공간변환 시키지 않고 헐 셰이더로 전달한다.
    output.position = input.position;
    
    // 색정보 설정
    output.color = input.color;
    
    return output;
}