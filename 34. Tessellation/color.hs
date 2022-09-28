cbuffer TessellationBuffer
{
    float tessellationAmount;
    float3 padding;
};

struct HS_INPUT{
    float3 position : POSITION;
    float4 color : COLOR;
};

// ConstantOutputType의 경우 상수 덮개 셰이더에서 출력으로 사용될 구조체다.
struct ConstantOutputType{
	float edges[3] : SV_TessFactor;  // 변 테셀레이션 계수 -> 변을 몇등분 낼꺼냐?
	float inside : SV_InsideTessFactor;   // 내부 테셀레이션 계수 -> 내부에 점을 몇개나 둘거냐?
};

struct HS_OUTPUT{
    float3 position : POSITION;
    float4 color : COLOR;
};


//////////////////////////////////////////////////////////////////////////////// 
// Patch Constant Function 
////////////////////////////////////////////////////////////////////////////////
// 상수 헐 셰이더는 패치와 테셀레이션 될 패치의 id를 입력으로 받는다.
// 상수 헐 셰이더는 도메인 셰이더로 값을 반환한다.
// 패치상수함수는 ColorShaderClass 객체에서 설정하는 상수버퍼의 tessellationAmount으로 지정한다.
ConstantOutputType ColorPatchConstantFunction(InputPatch<HS_INPUT, 3> inputPatch, uint patchId : SV_PrimitiveID){
	
	ConstantOutputType output;

    output.edges[0] = tessellationAmount;
    output.edges[1] = tessellationAmount;
    output.edges[2] = tessellationAmount;

    output.inside = tessellationAmount;

	return output;
}

//////////////////////////////////////////////////////////////////////////////// 
// Patch Point Function 
////////////////////////////////////////////////////////////////////////////////
// 현재 제어점을 그대로 통과시키기 때문에 패치 제어점 헐 셰이더는 정의하지 않는다. 

////////////////////////////////////////////////////////////////////////////////
// Hull Shader 
////////////////////////////////////////////////////////////////////////////////
// 헐 셰이더는 출력되는 제어점의 수마다 호출되고, 그 수만큼 패치 상수 함수를 호출한다.
// 헐 셰이더에서는 속성들을 정의하는 아래와 같은 헤더가 존재한다. 

// 패치 종류가 어떤 것이냐? 
//tri : 삼각형, quad : 사각형, isoline : ??
[domain("tri")] 
 
// 어떤 테셀레이션 세분모드를 사용할 것이냐?
// integer : 테셀레이션 계수의 소수점 이하 부분은 무시 이때문에 테셀레이션 수준이 변할때 파핑 현상이 눈에 띄게된다.
// fractional_even, fractional_odd : 테셀레이션 계수의 소수점 이하 부분도 테셀레이션되어 조밀하고 매끄럽게 만들기 가능
[partitioning("integer")]

// 세분에 의해 만들어지는 삼각형의 정점 감김 순서
// triangle_cw : 시계방향, triangle_ccw : 반시계 방향, line : 선 테셀레이션에 사용
[outputtopology("triangle_cw")]

// 덮개 셰이더가 출력할 제어점의 개수, 이는 하나의 입력 레이아웃에 대한 제어점 덮개 셰이더의 실행횟수다. 
[outputcontrolpoints(3)]

// 셰이더가 사용할 테셀레이션 계수의 최대값을 구동기에게 귀띔해주는 역할
// 하드웨어가 이 최대값을 미리 알고 잇으면 테셀레이션에 얼마나 많은 자원이 필요할지 추정할 수 있고 이는 최적화의 여지가 가능해진다.
// Direct3D11 대응 하드웨어의 테셀레이션 계수 최대값은 64이다.
[maxtessfactor(64.0f)]

// 패치 상수함수의 이름 
[patchconstantfunc("ColorPatchConstantFunction")]

// 헐 셰이더의 입력값은 패치, 현재 출력하 제어점 Id, 패치 ID다.
// 현재 헐 셰이더는 간단히 색상과 제어점들을 도메인 셰이더로 전달하지만 여기에서 제어점들에 더 많은 계산과 수정들을 가할 수 있다.
HS_OUTPUT hs_main(InputPatch<HS_INPUT, 3> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID){
	HS_OUTPUT output;

	// 현재 제어점의 위치를 출력 위치로 사용한다.
	output.position = patch[pointId].position;

	// 입력 색상을 출력 색상으로 사용한다.
	output.color = patch[pointId].color;

	return output;
}