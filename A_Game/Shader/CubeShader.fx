cbuffer ConstantBuffer : register(b0)
{
	matrix WVP; // matrix可以用float4x4替代。不加row_major的情况下，矩阵默认为列主矩阵，
	matrix World;  // 可以在前面添加row_major表示行主矩阵
	//matrix gProj;  // 该教程往后将使用默认的列主矩阵，但需要在C++代码端预先将矩阵进行转置。

	float4 difColor;
	bool hasTexture;
	bool hasNormMap;
}
Texture2D ObjTexture;
SamplerState Sampler     //定义采样器
{
    Filter = MIN_MAG_MIP_LINEAR; //采用线性过滤
    AddressU = WRAP; //寻址模式为Clamp
    AddressV = MIRROR; //寻址模式为Clamp
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
	vOut.posH = mul(float4(vIn.posL, 1.0f), WVP);  // mul 才是矩阵乘法, 运算符*要求操作对象为

    vOut.tex_coord = vIn.in_tex_coord;
    vOut.normal = mul(vIn.normal, World);
    vOut.tangent = mul(vIn.tangent,World);

	return vOut;
}
// 像素着色器
float4 PS(VertexOut pIn) : SV_Target
{
    pIn.normal = normalize(pIn.normal);
    float4 diffuse = ObjTexture.Sample(Sampler, pIn.tex_coord);
    return diffuse;
}
