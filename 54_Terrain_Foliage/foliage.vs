////////////////////////////////////////////////////////////////////////////////
// Filename: foliage.vs
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////

// Matrix Buffer에 World Matrix가 없는 것을 확인할 수 있는데 그 이유는 각 폴리지 조각에고유한 위치와 회전을 부여하기 위해 인스턴스 버퍼에서 인스턴스화 하고 읽어야하기 때문이다.
// 각 폴리지 조각은 고유한 worldMatrix를 가지고 있기때문에 상수 버퍼가 아닌 입력 조립기에서 정점과 같이 입력되어야한다.
cbuffer MatrixBuffer
{
	matrix viewMatrix;
	matrix projectionMatrix;
};


//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
	row_major matrix instanceWorld : WORLD;  // World Matrix에 대한 인스턴스 데이터를 입력데이터로 받는다.
	float3 instanceColor : TEXCOORD1;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 foliageColor : TEXCOORD1;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType FoliageVertexShader(VertexInputType input)
{
    PixelInputType output;
    

	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, input.instanceWorld);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;
    
	// Send the instanced foliage color into the pixel shader.
	output.foliageColor = input.instanceColor;

    return output;
}