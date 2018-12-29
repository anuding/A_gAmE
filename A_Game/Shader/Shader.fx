////////////////////////////////////////////////////////////////////////////
//// ���峣����������
////////////////////////////////////////////////////////////////////////////
//
//cbuffer cbPerObject
//{
//	float4x4 World;
//	float4x4 View;
//	float4x4 Proj;
//	float4 difColor;
//	bool hasTexture;
//	bool hasNormMap;
//};
//
//
//
////////////////////////////////////////////////////////////////////////////
//// ��������ṹ
////////////////////////////////////////////////////////////////////////////
//
////������ɫ��������ṹ
//struct VS_INPUT
//{
//	float4 Pos : POSITION;   //��������
//	float3 Norm : NORMAL;    //������
//	float2 Tex : TEXCOORD0;  //����
//};
//
////������ɫ��������ṹ
//struct PS_INPUT
//{
//	float4 Pos : SV_POSITION;           //��������
//};
//
////////////////////////////////////////////////////////////////////////////
//// ���������ɫ��
////////////////////////////////////////////////////////////////////////////
//
////������ɫ��
////���������Ƕ����VS_INPUT��Ϊ��������PS_INPUT��Ϊ����ֵ
////��������Żص���һ���ṹ�����Բ���Ҫָ������ֵ������
////�������ж�������ļ������㣬Ӧ��������������ļ�������
//PS_INPUT VS(VS_INPUT input)
//{
//	PS_INPUT output = (PS_INPUT)0;              //����һ��PS_INPUT����
//	output.Pos = mul(input.Pos, World);       //��input�����Ͻ�������任
//	output.Pos = mul(output.Pos, View);       //���й۲�任
//	output.Pos = mul(output.Pos, Proj); //����ͶӰ�任
//
//	output.Pos = float4(0.0f, 0.0f, 0.0f, 0.0f);
//	return output;
//}
//
////������ɫ��
////���ڱ��������������ɫ���ɹ��պͲ����໥���ò���
////���Բ��������������ɫ
//float4 PS(PS_INPUT input) : SV_Target
//{
//	float4 finalColor = 0;  //���ڲ�������ɫ����finalColorΪ0
//	return float4(1.0f, 1.0f, 1.0f, 1.0f);
//}
//float4 VS(float4 Pos:POSITION) :SV_POSITION
//{
//	return Pos;
//}
//
//float4 PS(float4 Pos:POSITION) : SV_TARGET
//{
//	return float4(0.5f,0.7f,0.7f,1.0f);
//}

cbuffer ConstantBuffer : register(b0)
{
	matrix gWorld; // matrix������float4x4���������row_major������£�����Ĭ��Ϊ��������
	matrix gView;  // ������ǰ�����row_major��ʾ��������
	matrix gProj;  // �ý̳�����ʹ��Ĭ�ϵ��������󣬵���Ҫ��C++�����Ԥ�Ƚ��������ת�á�
}


struct VertexIn
{
	float3 posL : POSITION;
	float4 color : COLOR;
};

struct VertexOut
{
	float4 posH : SV_POSITION;
	float4 color : COLOR;
};



VertexOut VS(VertexIn vIn)
{
	VertexOut vOut;
	vOut.posH = mul(float4(vIn.posL, 1.0f), gWorld);  // mul ���Ǿ���˷�, �����*Ҫ���������Ϊ
	vOut.posH = mul(vOut.posH, gView);               // ��������ȵ��������󣬽��Ϊ
	vOut.posH = mul(vOut.posH, gProj);               // Cij = Aij * Bij
	vOut.color = vIn.color;                         // ����alphaͨ����ֵĬ��Ϊ1.0
	return vOut;
}
// ������ɫ��
float4 PS(VertexOut pIn) : SV_Target
{
	return pIn.color;
}
