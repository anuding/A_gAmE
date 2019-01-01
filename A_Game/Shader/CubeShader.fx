cbuffer ConstantBuffer : register(b0)
{
	matrix WVP; // matrix������float4x4���������row_major������£�����Ĭ��Ϊ��������
	matrix World;  // ������ǰ�����row_major��ʾ��������
	//matrix gProj;  // �ý̳�����ʹ��Ĭ�ϵ��������󣬵���Ҫ��C++�����Ԥ�Ƚ��������ת�á�

	float4 difColor;
	bool hasTexture;
	bool hasNormMap;
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
	vOut.posH = mul(float4(vIn.posL, 1.0f), WVP);  // mul ���Ǿ���˷�, �����*Ҫ���������Ϊ
	//vOut.posH = mul(vOut.posH, gView);               // ��������ȵ��������󣬽��Ϊ
	//vOut.posH = mul(vOut.posH, gProj);               // Cij = Aij * Bij
	vOut.color = vIn.color;                         // ����alphaͨ����ֵĬ��Ϊ1.0
	return vOut;
}
// ������ɫ��
float4 PS(VertexOut pIn) : SV_Target
{
	return pIn.color;
}
