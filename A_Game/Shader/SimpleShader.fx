cbuffer ConstantBuffer : register(b0)
{
	matrix World;             //世界变换矩阵
	matrix View;              //观察变换矩阵
	matrix Projection;    //投影变换矩阵
}

struct VS_INPUT
{
	float4 Pos : SV_POSITION;  //float4是类型，SV_POSITION为语义表示位置
	float4 Color : COLOR0;    //COLOR0表示颜色，COLOR后的数字为0到资源支持
							 //最大数之间的整数
};




VS_INPUT VS(float4 Pos : POSITION, float4 Color : COLOR)
{
	VS_INPUT input = (VS_INPUT)0;       //定义一个顶点结构的对象
	input.Pos = mul(Pos, World);           //世界坐标变换，mul为矩阵乘法的函数
	input.Pos = mul(input.Pos, View);       //观察坐标变换，mul第一个参数为向量
	input.Pos = mul(input.Pos, Projection);   //投影坐标变换，mul第二个
										//参数为矩阵
	input.Color = Color;                    //设置VS_INPUT对象的颜色
	return input;                          //返回VS_INPUT的对象
}


float4 PS(VS_INPUT input) : SV_Target
{
   return input.Color;
}


technique11 TexTech
{
	//Technique通过Pass来应用不同的效果，每个Technique可以有多个Pass
   //本例中只有一个Pass
	pass P0
	{
		//设置顶点着色器
		//CompileShader包含两个版本，一个是目标着色器，这里使用的是SM5.0
		//另一个参数为我们自己顶一个着色器函数
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);   //设置为空	       
		SetPixelShader(CompileShader(ps_5_0, PS())); //设置像素着色器
	}
}
