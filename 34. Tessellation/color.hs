cbuffer TessellationBuffer
{
    float tessellationAmount;
    float3 padding;
};

struct HS_INPUT{
    float3 position : POSITION;
    float4 color : COLOR;
};

// ConstantOutputType�� ��� ��� ���� ���̴����� ������� ���� ����ü��.
struct ConstantOutputType{
	float edges[3] : SV_TessFactor;  // �� �׼����̼� ��� -> ���� ���� ������?
	float inside : SV_InsideTessFactor;   // ���� �׼����̼� ��� -> ���ο� ���� ��� �Ѱų�?
};

struct HS_OUTPUT{
    float3 position : POSITION;
    float4 color : COLOR;
};


//////////////////////////////////////////////////////////////////////////////// 
// Patch Constant Function 
////////////////////////////////////////////////////////////////////////////////
// ��� �� ���̴��� ��ġ�� �׼����̼� �� ��ġ�� id�� �Է����� �޴´�.
// ��� �� ���̴��� ������ ���̴��� ���� ��ȯ�Ѵ�.
// ��ġ����Լ��� ColorShaderClass ��ü���� �����ϴ� ��������� tessellationAmount���� �����Ѵ�.
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
// ���� �������� �״�� �����Ű�� ������ ��ġ ������ �� ���̴��� �������� �ʴ´�. 

////////////////////////////////////////////////////////////////////////////////
// Hull Shader 
////////////////////////////////////////////////////////////////////////////////
// �� ���̴��� ��µǴ� �������� ������ ȣ��ǰ�, �� ����ŭ ��ġ ��� �Լ��� ȣ���Ѵ�.
// �� ���̴������� �Ӽ����� �����ϴ� �Ʒ��� ���� ����� �����Ѵ�. 

// ��ġ ������ � ���̳�? 
//tri : �ﰢ��, quad : �簢��, isoline : ??
[domain("tri")] 
 
// � �׼����̼� ���и�带 ����� ���̳�?
// integer : �׼����̼� ����� �Ҽ��� ���� �κ��� ���� �̶����� �׼����̼� ������ ���Ҷ� ���� ������ ���� ��Եȴ�.
// fractional_even, fractional_odd : �׼����̼� ����� �Ҽ��� ���� �κе� �׼����̼ǵǾ� �����ϰ� �Ų����� ����� ����
[partitioning("integer")]

// ���п� ���� ��������� �ﰢ���� ���� ���� ����
// triangle_cw : �ð����, triangle_ccw : �ݽð� ����, line : �� �׼����̼ǿ� ���
[outputtopology("triangle_cw")]

// ���� ���̴��� ����� �������� ����, �̴� �ϳ��� �Է� ���̾ƿ��� ���� ������ ���� ���̴��� ����Ƚ����. 
[outputcontrolpoints(3)]

// ���̴��� ����� �׼����̼� ����� �ִ밪�� �����⿡�� �Ͷ����ִ� ����
// �ϵ��� �� �ִ밪�� �̸� �˰� ������ �׼����̼ǿ� �󸶳� ���� �ڿ��� �ʿ����� ������ �� �ְ� �̴� ����ȭ�� ������ ����������.
// Direct3D11 ���� �ϵ������ �׼����̼� ��� �ִ밪�� 64�̴�.
[maxtessfactor(64.0f)]

// ��ġ ����Լ��� �̸� 
[patchconstantfunc("ColorPatchConstantFunction")]

// �� ���̴��� �Է°��� ��ġ, ���� ����� ������ Id, ��ġ ID��.
// ���� �� ���̴��� ������ ����� ���������� ������ ���̴��� ���������� ���⿡�� �������鿡 �� ���� ���� �������� ���� �� �ִ�.
HS_OUTPUT hs_main(InputPatch<HS_INPUT, 3> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID){
	HS_OUTPUT output;

	// ���� �������� ��ġ�� ��� ��ġ�� ����Ѵ�.
	output.position = patch[pointId].position;

	// �Է� ������ ��� �������� ����Ѵ�.
	output.color = patch[pointId].color;

	return output;
}