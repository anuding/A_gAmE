cbuffer ConstantBuffer : register(b0)
{
	matrix WVP; // matrix可以用float4x4替代。不加row_major的情况下，矩阵默认为列主矩阵，
	matrix World;  // 可以在前面添加row_major表示行主矩阵
	//matrix gProj;  // 该教程往后将使用默认的列主矩阵，但需要在C++代码端预先将矩阵进行转置。

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
	vOut.posH = mul(float4(vIn.posL, 1.0f), WVP);  // mul 才是矩阵乘法, 运算符*要求操作对象为
	//vOut.posH = mul(vOut.posH, gView);               // 行列数相等的两个矩阵，结果为
	//vOut.posH = mul(vOut.posH, gProj);               // Cij = Aij * Bij
	vOut.color = vIn.color;                         // 这里alpha通道的值默认为1.0
	return vOut;
}
// 像素着色器
float4 PS(VertexOut pIn) : SV_Target
{
	return pIn.color;
}
