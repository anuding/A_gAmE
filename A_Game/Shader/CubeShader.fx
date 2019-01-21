cbuffer ConstantBuffer : register(b0)
{
	matrix WVP; // matrix������float4x4���������row_major������£�����Ĭ��Ϊ��������
	matrix World;  // ������ǰ�����row_major��ʾ��������
	//matrix gProj;  // �ý̳�����ʹ��Ĭ�ϵ��������󣬵���Ҫ��C++�����Ԥ�Ƚ��������ת�á�

	float4 difColor;
	bool hasTexture;
	bool hasNormMap;
}
Texture2D ObjTexture;
SamplerState Sampler     //���������
{
    Filter = MIN_MAG_MIP_LINEAR; //�������Թ���
    AddressU = WRAP; //ѰַģʽΪClamp
    AddressV = MIRROR; //ѰַģʽΪClamp
};


struct VertexIn
{
	float3 posL : POSITION;
	float2 in_tex_coord : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
};

struct VertexOut
{
	float4 posH : SV_POSITION;
	float2 tex_coord : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
};



VertexOut VS(VertexIn vIn)
{
	VertexOut vOut;
	vOut.posH = mul(float4(vIn.posL, 1.0f), WVP);  // mul ���Ǿ���˷�, �����*Ҫ���������Ϊ

    vOut.tex_coord = vIn.in_tex_coord;
    vOut.normal = mul(vIn.normal, World);
    vOut.tangent = mul(vIn.tangent,World);

	return vOut;
}
// ������ɫ��
float4 PS(VertexOut pIn) : SV_Target
{
    pIn.normal = normalize(pIn.normal);
    float4 diffuse = ObjTexture.Sample(Sampler, pIn.tex_coord);
    return diffuse;
}
