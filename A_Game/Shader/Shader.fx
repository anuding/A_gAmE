////////////////////////////////////////////////////////////////////////////
//// 定义常量常量缓存
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
//// 定义输入结构
////////////////////////////////////////////////////////////////////////////
//
////顶点着色器的输入结构
//struct VS_INPUT
//{
//	float4 Pos : POSITION;   //顶点坐标
//	float3 Norm : NORMAL;    //法向量
//	float2 Tex : TEXCOORD0;  //纹理
//};
//
////像素着色器的输入结构
//struct PS_INPUT
//{
//	float4 Pos : SV_POSITION;           //顶点坐标
//};
//
////////////////////////////////////////////////////////////////////////////
//// 定义各类着色器
////////////////////////////////////////////////////////////////////////////
//
////顶点着色器
////这里以我们定义的VS_INPUT作为参数，以PS_INPUT作为返回值
////由于这里放回的是一个结构，所以不需要指定返回值的语义
////本例中有多次向量的减法运算，应该理解向量减法的几何意义
//PS_INPUT VS(VS_INPUT input)
//{
//	PS_INPUT output = (PS_INPUT)0;              //声明一个PS_INPUT对象
//	output.Pos = mul(input.Pos, World);       //在input坐标上进行世界变换
//	output.Pos = mul(output.Pos, View);       //进行观察变换
//	output.Pos = mul(output.Pos, Proj); //进行投影变换
//
//	output.Pos = float4(0.0f, 0.0f, 0.0f, 0.0f);
//	return output;
//}
//
////像素着色器
////由于本例中立方体的颜色是由光照和材质相互作用产生
////所以不对立方体进行着色
//float4 PS(PS_INPUT input) : SV_Target
//{
//	float4 finalColor = 0;  //由于不进行着色所以finalColor为0
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
	matrix gWorld; // matrix可以用float4x4替代。不加row_major的情况下，矩阵默认为列主矩阵，
	matrix gView;  // 可以在前面添加row_major表示行主矩阵
	matrix gProj;  // 该教程往后将使用默认的列主矩阵，但需要在C++代码端预先将矩阵进行转置。
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
	vOut.posH = mul(float4(vIn.posL, 1.0f), gWorld);  // mul 才是矩阵乘法, 运算符*要求操作对象为
	vOut.posH = mul(vOut.posH, gView);               // 行列数相等的两个矩阵，结果为
	vOut.posH = mul(vOut.posH, gProj);               // Cij = Aij * Bij
	vOut.color = vIn.color;                         // 这里alpha通道的值默认为1.0
	return vOut;
}
// 像素着色器
float4 PS(VertexOut pIn) : SV_Target
{
	return pIn.color;
}
