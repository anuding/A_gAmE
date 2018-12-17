#ifndef LIGHT_H_H
#define LIGHT_H_H

#include <DirectXMath.h>
using namespace DirectX;
//这是材质的对各种光反射率
struct Material
{
	XMFLOAT4 ambient;		//材质环境光反射率
	XMFLOAT4 diffuse;		//材质漫射光反射率
	XMFLOAT4 specular;		//材质镜面光反射率
	float    power;			//镜面光反射系数
};

//光源结构，这个结构包括了3种光源的所有属性
//但不是每种属性都会用到，比如方向光就不会用到
//光源位置，以及衰减因子等
struct Light
{
	int type;				//光源类型，方向光：0，点光源：1，聚光灯：2

	XMFLOAT4 position;		//光源位置
	XMFLOAT4 direction;		//方向向量

	XMFLOAT4 ambient;		//环境光强度
	XMFLOAT4 diffuse;		//漫射光强度
	XMFLOAT4 specular;		//镜面光强度

	float attenuation0;		//常量衰减因子
	float attenuation1;		//一次衰减因子
	float attenuation2;		//二次衰减因子

	float alpha;			//聚光灯内锥角度
	float beta;				//聚光灯外锥角度
	float fallOff;			//聚光灯衰减系数，一般取值为1.0

};

#endif

