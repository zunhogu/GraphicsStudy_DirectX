cbuffer MatrixBuffer{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

// 이 셰이더는 텍셀의 실제 UV 위치를 계싼하기 위해 화면의 너비 or RTT의 너비가 필요하다
// 아래의 상수버퍼로 너비값을 받아온다.
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
	float2 texCoord1 : TEXCOORD1;   // 왼쪽 픽셀4
	float2 texCoord2 : TEXCOORD2;   // 왼쪽 픽셀3
	float2 texCoord3 : TEXCOORD3;    // 왼쪽 픽셀2
	float2 texCoord4 : TEXCOORD4;   // 왼쪽 픽셀1
	float2 texCoord5 : TEXCOORD5;   // 가운데 픽셀
	float2 texCoord6 : TEXCOORD6;   // 오른쪽 픽셀1
	float2 texCoord7 : TEXCOORD7;   // 오른쪽 픽셀2
	float2 texCoord8 : TEXCOORD8;   // 오른쪽 픽셀3
	float2 texCoord9 : TEXCOORD9;   // 오른쪽 픽셀4
};

VS_OUTPUT vs_main(VS_INPUT input){
    VS_OUTPUT output;
	float texelSize;

    input.position.w = 1.0f;

    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	output.tex = input.tex;
    
	// 1을 화면의 너비로 나누어 텍셀의 크기(offset)를 구한다.이 값을 이용하여 인접 픽셀의 UV좌표를 구할수 있다.
	texelSize = 1.0f / screenWidth;

	// 가운데 픽셀과 좌우로 인접한 각 4개의 픽셀에 대한 UV좌표를 계산한다.
	// 현재 텍스처의 좌표를 가지고 수평 offset값을 더해감으로써 9개의 좌표를 모두 구한다.
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