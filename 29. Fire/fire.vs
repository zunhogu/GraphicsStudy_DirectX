cbuffer MatrixBuffer{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer NoiseBuffer {
    float frameTime;   // 매 프레임마다 갱신되어 노이즈 텍스처들을 스크롤하는 변수
    float3 scrollSpeeds;   // 스크롤의 속도를 지정하는 변수  
    float3 scales;    // 세 노이즈 텍스처의 스케일 값(또는 옥타브 값) 일반적으로 x y z 각각 1 2 3 이 저장됨
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

    // input.tex와 sacle.x를 가지고 옥타브를 만들고 y축으로 스크롤되는 좌표를 계산한다.
    // 이 좌표는 픽셀셰이더에서 노이즈 텍스처에 샘플링되고 샘플링 된 값으로 왜곡된 텍스처 좌표를 가질것이다.

    // 첫번째 노이즈 텍스쳐의 좌표를 첫번째 크기 및 윗방향 스크롤 속도 값을 이용하여 계산한다.
    output.texCoords1 = (input.tex * scales.x);
    output.texCoords1.y = output.texCoords1.y + (frameTime * scrollSpeeds.x);

    // 두번째 노이즈 텍스쳐의 좌표를 두번째 크기 및 윗방향 스크롤 속도 값을 이용하여 계산한다.
    output.texCoords2 = (input.tex * scales.y);
    output.texCoords2.y = output.texCoords2.y + (frameTime * scrollSpeeds.y);

    // 세번째 노이즈 텍스쳐의 좌표를 세번째 크기 및 윗방향 스크롤 속도 값을 이용하여 계산한다.
    output.texCoords3 = (input.tex * scales.z);
    output.texCoords3.y = output.texCoords3.y + (frameTime * scrollSpeeds.z);

    return output;
}