//////////////////////////////////////////////////////////////////////////
// ���峣����������
//////////////////////////////////////////////////////////////////////////

//����任����ĳ�������
cbuffer MatrixBuffer
{
	matrix World;         //��������任����
	matrix View;          //�۲�����任����
	matrix Projection;    //ͶӰ����任����
	float4 EyePosition;   //�ӵ�λ��
};
Texture2D Texture;       //�������

SamplerState Sampler     //���������
{
	Filter = MIN_MAG_MIP_LINEAR;   //�������Թ���
	AddressU = WRAP;              //ѰַģʽΪClamp
	AddressV = MIRROR;              //ѰַģʽΪClamp
};

//������Ϣ�ĳ�������
cbuffer MaterialBuffer
{
	float4 MatAmbient;    //���ʶԻ�����ķ�����
	float4 MatDiffuse;    //���ʶ��������ķ�����
	float4 MatSpecular;   //���ʶԾ����ķ�����
	float  MatPower;      //���ʵľ���ⷴ��ϵ��
}

//��Դ�ĳ�������
cbuffer LightBuffer
{
	int    type;          //��Դ����
	float4 LightPosition; //��Դλ��
	float4 LightDirection;//��Դ����
	float4 LightAmbient;  //������ǿ��
	float4 LightDiffuse;  //�������ǿ��
	float4 LightSpecular; //�����ǿ��
	float  LightAtt0;     //����˥������
	float  LightAtt1;     //һ��˥������
	float  LightAtt2;     //����˥������
	float  LightAlpha;    //�۹����׶�Ƕ�
	float  LightBeta;     //�۹����׶�Ƕ�
	float  LightFallOff;  //�۹��˥��ϵ��
}

//////////////////////////////////////////////////////////////////////////
// ��������ṹ
//////////////////////////////////////////////////////////////////////////

//������ɫ��������ṹ
struct VS_INPUT
{
	float4 Pos : POSITION;   //��������
	float3 Norm : NORMAL;    //������
	float2 Tex : TEXCOORD0;  //����
};

//������ɫ��������ṹ
struct PS_INPUT
{
	float4 Pos : SV_POSITION;           //��������
	float3 Norm : TEXCOORD0;            //������
	float4 ViewDirection : TEXCOORD1;   //�ӵ㷽��
	float4 LightVector : TEXCOORD2;		//�Ե��Դ�;۹����Ч��
										//ǰ3��������¼������������
										//���һ��������¼���վ���

	float2 Tex : TEXCOORD3;      //����
};

//////////////////////////////////////////////////////////////////////////
// ���������ɫ��
//////////////////////////////////////////////////////////////////////////

//������ɫ��
//���������Ƕ����VS_INPUT��Ϊ��������PS_INPUT��Ϊ����ֵ
//��������Żص���һ���ṹ�����Բ���Ҫָ������ֵ������
//�������ж�������ļ������㣬Ӧ��������������ļ�������
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;              //����һ��PS_INPUT����
	output.Pos = mul(input.Pos, World);       //��input�����Ͻ�������任
	output.Pos = mul(output.Pos, View);       //���й۲�任
	output.Pos = mul(output.Pos, Projection); //����ͶӰ�任

	output.Norm = mul(input.Norm, (float3x3)World);  //���output�ķ�����
	output.Norm = normalize(output.Norm);              //�Է��������й�һ��

	float4 worldPosition = mul(input.Pos, World);          //��ȡ�������������
	output.ViewDirection = EyePosition - worldPosition;    //��ȡ�ӵ㷽��
	output.ViewDirection = normalize(output.ViewDirection);//���ӵ㷽���һ��

	output.LightVector = LightPosition - worldPosition;    //��ȡ���շ���
	output.LightVector = normalize(output.LightVector);    //�����շ����һ��
	output.LightVector.w = length(LightPosition - worldPosition);  //��ȡ���վ���

	output.Tex = input.Tex;       //��������
	return output;
}

//������ɫ��
//���ڱ��������������ɫ���ɹ��պͲ����໥���ò���
//���Բ��������������ɫ
float4 PS(PS_INPUT input) : SV_Target
{
	float4 finalColor = 0;  //���ڲ�������ɫ����finalColorΪ0
	return finalColor;
}

// ƽ�й�Դ������ɫ��
// ������PS_INPUT�Ķ�����Ϊ����������һ��float4�ı�����Ϊ��ɫ
// ע�⣺�������ɫ��������ɫ�����������ɹ��պͲ����໥���ò�������ɫ
float4 PSDirectionalLight(PS_INPUT input) : SV_Target
{
	float4 finalColor;           //������ɫ�����������ɫ�����յ���ɫ
	float4 ambientColor, diffuseColor, specularColor; //���������⣬�����䣬�������ɫ

	//���շ���,�͹������䷽���෴
	float3 lightVector = -LightDirection.xyz;  //������.xyz�ķ�ʽȥfloat4�����ǰ3λ����
	lightVector = normalize(lightVector);      //��һ����������
	//�ò��ʻ����ⷴ���ʺͻ�����ǿ����˵õ���������ɫ
	ambientColor = MatAmbient * LightAmbient;
	//�����㷨�����͹��շ�����е�˵õ������������
	float diffuseFactor = dot(lightVector, input.Norm);
	if (diffuseFactor > 0.0f)  //�����������>0��ʾ���Ǳ�����
	{
		//�ò��ʵ��������ķ����ʺ��������Ĺ���ǿ���Լ��������������˵õ����������ɫ
		diffuseColor = MatDiffuse * LightDiffuse * diffuseFactor;

		//���ݹ��շ���Ͷ��㷨�������㷴�䷽��
		float3 reflection = reflect(-lightVector, input.Norm);
		//���ݷ��䷽���ӵ㷽���Լ����ʵľ���ⷴ��ϵ�������㾵�淴������
		//pow(b, n)����b��n�η�
		float specularFactor = pow(max(dot(reflection, input.ViewDirection.xyz), 0.0f), MatPower);
		//���ʵľ��淴���ʣ������ǿ���Լ����淴��������˵õ��������ɫ
		specularColor = MatSpecular * LightSpecular * specularFactor;
	}
	//������ɫ�ɾ��⣬�����䣬�������ɫ������ӵõ�
	//saturate��ʾ���ʹ����������1�ͱ��1��С��0�ͱ��0���Ա�֤�����0-1֮��
	finalColor = saturate(ambientColor + diffuseColor + specularColor);
	
	return finalColor*Texture.Sample(Sampler, input.Tex);    //��������

}


//////////////////////////////////////////////////////////////////////////
// �������Technique
// ��ЩTechnique����������������ɫ����ͬ
//////////////////////////////////////////////////////////////////////////
technique11 T0
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}

//�����Technique
technique11 T_DirLight
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PSDirectionalLight()));
	}
}
