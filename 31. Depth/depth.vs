cbuffer MatrixBuffer{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};


struct VS_INPUT{
    float4 position : POSITION;
};

struct VS_OUTPUT{
    float4 position : SV_POSITION;
    float4 depthPosition : TEXTURE0;
};


VS_OUTPUT vs_main(VS_INPUT input){
    VS_OUTPUT output;
   
    
    input.position.w = 1.0f;

    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

	output.depthPosition = output.position;
	
	return output;
}