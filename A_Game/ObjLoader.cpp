#include "ObjLoader.h"

//#pragma warning(disable : 4127)

//材质类的构造函数
ObjMaterial::ObjMaterial()
{
	memset( strName , 0 , sizeof(char) * 1024 );
	vAmbient = XMFLOAT4( 1.0f , 1.0f , 1.0f, 1.0f);
	vDiffuse = XMFLOAT4( 1.0f , 1.0f , 1.0f, 1.0f);
	vSpecular = XMFLOAT4( 1.0f , 1.0f , 1.0f, 1.0f);
	nShininess = 0;
	pTextureRV = NULL;
}
//材质类的释放函数，主要释放纹理对象
void ObjMaterial::Release()
{
	if(pTextureRV)
	{
		(pTextureRV)->Release();
		(pTextureRV)=NULL;
	}
}

//网格类的构造函数
ObjSubMesh::ObjSubMesh()
{
	pTextureRV = 0;
}

//网格类的释放函数，主要释放纹理对象
void ObjSubMesh::Release()
{
	if(pTextureRV)
	{
		pTextureRV->Release();
		pTextureRV = 0;
	}
}

//ObjLoader的默认构造函数
ObjLoader::ObjLoader()
{
	m_meshVertexBuffer = 0;
	m_meshIndexBuffer = 0;
	effect = 0;
}

//ObjLoader的构造函数，以ID3DX11Effect对象作为参数
ObjLoader::ObjLoader(ID3DX11Effect* objeffect)
{
	m_meshVertexBuffer = 0;
	m_meshIndexBuffer = 0;
	effect = objeffect;
}

//从OBJ文件中读入物体信息
bool ObjLoader::Load(ID3D11Device* device, char* resPath, char* objFilename, int nHandSystem)
{
	m_strResPath = resPath;

	std::string objFullFilename(resPath);  //取得文件路径
	objFullFilename += objFilename;        //将路径和文件名合并成完整文件名
	ParseVertices(device, (char*)objFullFilename.c_str(), nHandSystem); //根据文件中的顶点信息创建顶点
	ParseSubset((char*)objFullFilename.c_str(), nHandSystem);           //创建子集
	CreateBuffers(device);                                              //创建顶点缓存

	return true;
}

//根据文件中的顶点信息创建顶点
bool ObjLoader::ParseVertices(ID3D11Device* device, char* fileName, int nHandSystem)
{
	std::ifstream inFile;    //声明一个文件流输入对象
	inFile.open(fileName);   //打开文件
	if(inFile.fail())        //判断文件是否打开成功
	{
		return false;
	}

	while(true)              //通过一个循环读取文件，直到文件结束才退出循环
	{
		std::string input;   //创建一个字符串
		inFile>>input;       //对去文件流对象的一行传递给input字符串
		if(input == "v")     //读取input的值为v则表示之后的数字为顶点坐标
		{
			XMFLOAT3 pos;    //声明一个XMFLOAT3来存放文件中的顶点坐标
			inFile>>pos.x;   //设置x轴坐标
			inFile>>pos.y;   //设置y轴坐标
			inFile>>pos.z;   //设置z轴坐标

			if(Right_Hand_System == nHandSystem) //如果是右手坐标系需要进行z轴转向
			{
				pos.z = pos.z * -1.0f;
			}
			m_vertices.push_back( pos );  //将这个顶点坐标推入m_vertices成员中
		}
		else if(input == "vt") //读取input的值为vt则表示之后的数字为纹理坐标
		{
			XMFLOAT2 tc;
			inFile>>tc.x;      //设置纹理x轴坐标
			inFile>>tc.y;      //设置纹理y轴坐标

			if(Right_Hand_System == nHandSystem)
			{
				tc.y = 1.0f - tc.y;
			}
			m_textures.push_back( tc );  //将这个顶点的纹理坐标推入m_textures成员中
		}
		else if(input == "vn")  //读取input的值为vn则表示之后的数字为法向量
		{
			XMFLOAT3 nor;
			inFile>>nor.x;      
			inFile>>nor.y;
			inFile>>nor.z;

			if(Right_Hand_System == nHandSystem)
			{
				nor.z = nor.z * -1.0f;
			}
			m_normals.push_back( nor );  //将这个顶点的方向量推入给m_normals成员变量
		}
		else if(input == "mtllib")       //读取input的值为mtllib则表示之后的内容为材质文件名
		{
			// 解析材质文件
			std::string matFileName;
			inFile>>matFileName;

			// 材质文件中记录的纹理文件名是不包含路径信息的
			ParseMaterialFile(device, (char*)matFileName.c_str());
		}
		/// usemtl和f信息在ParseSubset函数中提取
		inFile.ignore( 1024 , '\n' );
		if( inFile.eof() )  //读到文件结束就退出循环
			break;
	}

	inFile.close();  //关键文件输入流对象

	return true;
}

//提取材质文件中材质信息
bool ObjLoader::ParseMaterialFile(ID3D11Device* device, char* fileName)
{
	std::string fullFileName = m_strResPath + fileName;  //获取文件全名
	std::ifstream infile;
	infile .open(fullFileName);
	int texcount = 0;
	if(infile.fail())                                    //判断文件打开是否成功
	{
		return false;
	}
	//记录纹理个数
	while(true)
	{
		std::string input;
		infile>>input;
		if(input == "newmtl")
		{
			texcount++;
		}
		infile.ignore( 1024 , '\n' );

		if( infile.eof() )  //文件结束则跳出循环
			break;
	}

	std::ifstream inFile;                                //声明一个文件输入流对象
	
	inFile.open(fullFileName);                           //打开文件
	if(inFile.fail())                                    //判断文件打开是否成功
	{
		return false;
	}
	//这里作了修改
	ObjMaterial** pMat = new ObjMaterial*[texcount];         //声明一个ObjMaterial材质对象
	for(int j =0;j<texcount;j++)
	{
		pMat[j] = new ObjMaterial;  //新建一个材质对象
	}
	int i = 0;
	while(true)                       //通过循环逐行读取材质文件中的内容
	{
		std::string input;
		inFile>>input;
		if(input == "newmtl")      //如果读入input的值为newmtl则创建一个新材质信息
		{
			if(pMat[i] != NULL&&pMat[i]->strName[0] != '\0')         //如果pMat里本来就存在材质信息（针对一个mtl文件定义多种材质的情况）
			{
				m_Material.push_back(*pMat[i]); //则将材质信息推入m_Material成员        
				i++;//增加一个成员
			}

			inFile>>pMat[i]->strName;   //将newmtl后的字符串设置为材质名称
		}
		else if(pMat[i] != NULL)        //如果材质对象pMat不为空
		{
			if(input == "Ka")        //如果读入input的值为Ka则后面的值为材质的环境光反射率
			{
				//设置材质的环境光反射率
				inFile>>pMat[i]->vAmbient.x;
				inFile>>pMat[i]->vAmbient.y;
				inFile>>pMat[i]->vAmbient.z;
			}
			else if(input == "Kd")  //如果读入input的值为Kd则后面的值为材质的散射光反射率
			{
				//设置材质的散射光反射率
				inFile>>pMat[i]->vDiffuse.x;
				inFile>>pMat[i]->vDiffuse.y;
				inFile>>pMat[i]->vDiffuse.z;
			}
			else if(input == "Ks")  //如果读入input的值为Ks则后面的值为材质的镜面光光反射率
			{
				//设置材质的镜面光反射率
				inFile>>pMat[i]->vSpecular.x;
				inFile>>pMat[i]->vSpecular.y;
				inFile>>pMat[i]->vSpecular.z;
			}
			else if(input == "Ns")  //如果读入input的值为Ns则后面的值为材质的镜面光光反射率
			{
				inFile>>pMat[i]->nShininess;  //设置镜面反射系数
			}
			else if(input == "map_Kd")  //如果读入input的值为map_Kd则后面的值为纹理文件名
			{
				// 首先判断文件名是否存在，如果不存在，那么下一个字符将是'\n'
				char ch;
				inFile.get(ch);
				if(ch == ' ' || ch == '\t')
				{
					inFile.get(ch);
				}

				if(ch == '\n')               //如果后面没有文件名
				{
					pMat[i]->pTextureRV = NULL;//则设置纹理为空
					if( inFile.eof() )      //如果是文件结尾则跳出循环
						break;
					else                    //否则继续循环
						continue;
				}

				std::string mapFileName;    //声明字符串来读取文件名
				inFile>>mapFileName;
				// 此处需要为纹理文件名添加上路径，因为OBJ文件是不包含路径信息的
				mapFileName = ch + mapFileName;
				mapFileName = m_strResPath + mapFileName;
				//从纹理文件中读取文件并将读取的信息存入材质对象的pTextureRV成员
				HRESULT result = D3DX11CreateShaderResourceViewFromFileA(
					device, mapFileName.c_str(), NULL, NULL,  &pMat[i]->pTextureRV, NULL);
				if(FAILED(result))
				{
					::MessageBoxA(NULL, "Create texture failed", "Notice", 0);
					return false;
				}
			}
		}
		inFile.ignore( 1024 , '\n' );

		if( inFile.eof() )  //文件结束则跳出循环
			break;
	}

	// 最后pMat还要添加一次，这是针对一个mtl文件只有一个材质信息的情况
	// 或者有多个材质信息的情况，将最后一个材质信息推入m_Material对象
	if(pMat[i] != NULL)
	{
		m_Material.push_back(*pMat[i]);
	}

	return true;
}

//根据读入的顶点信息创建子集
bool ObjLoader::ParseSubset(char* fileName,int nHandSystem)
{
	std::ifstream inFile;   //声明一个文件输入流对象
	inFile.open(fileName);  //打开文件
	if(inFile.fail())       //判断打开是否成功
	{
		return false;
	}

	ObjSubMesh* pSubMesh = NULL;    //创建一个ObjSubMesh对象
	unsigned long lStartIndex = 0;  //索引开始
	unsigned long lIndexCount = 0;  //索引数量

	while(true)             
	{
		std::string input;
		inFile>>input;
		if(input == "usemtl")            //如果读入input的值为usemtl则后面的值为材质的名称
		{
			if(pSubMesh != NULL)         //如果ObjSubMesh对象不为空
			{
				pSubMesh->lStartIndex = lStartIndex;              //设置ObjSubMesh对象的索引起始位置
				pSubMesh->lIndexCount = lIndexCount - lStartIndex;//设置ObjSubMesh对象的索引大小

				m_Mesh.push_back(*pSubMesh);   //将这个ObjSubMesh对象推入m_Mesh成员中
				delete pSubMesh;
			}

			pSubMesh = new ObjSubMesh;   //新建一个ObjSubMesh对象

			inFile>>pSubMesh->strMaterialName;  //设置ObjSubMesh对象的材质名
			// 根据材质名称获取材质视图对象
			std::vector<ObjMaterial>::iterator iter = m_Material.begin(); //声明一个迭代器
			while(iter != m_Material.end())   //从m_Material数组第一个遍历到最后一个
			{
				if(iter->strName == pSubMesh->strMaterialName) break; //逐个比较材质名，如有相同即退出
				iter++;
			}
			if(iter == m_Material.end())     //如果材质数字起始和结尾是在同一位置，则说明该数组为空
			{
				::MessageBoxA(NULL, "Material Error: can not find material", "Error", 0);
				return false;
			}
			else
				pSubMesh->pTextureRV = iter->pTextureRV;    //将遍历器的纹理赋值给pSubMesh的纹理

			lStartIndex = lIndexCount;                      //更新索引起始位置
		}
		else if(pSubMesh != NULL)  //如果读入input的值不是usemtl且pSubMesh不为空则表示
		{
			if(input == "f")      //如果读入input的值为f则之后的数字为坐标，纹理，法向量的索引
			{
				ObjVertex vertex[3];  //顶点对象
				int posIndex[3];      //位置索引
				int texIndex[3];      //纹理索引
				int normalIndex[3];   //法向量索引

				char ch;
				if(Left_Hand_System == nHandSystem) //左手坐标系
				{	//依次读入各个索引值			
					inFile >> posIndex[0] >> ch >> texIndex[0] >> ch >> normalIndex[0]
					>> posIndex[1] >> ch >> texIndex[1] >> ch >> normalIndex[1]
					>> posIndex[2] >> ch >> texIndex[2] >> ch >> normalIndex[2];
				}
				else    //右手坐标系
				{   //依次读入各个索引值
					inFile >> posIndex[2] >> ch >> texIndex[2] >> ch >> normalIndex[2]
					>> posIndex[1] >> ch >> texIndex[1] >> ch >> normalIndex[1]
					>> posIndex[0] >> ch >> texIndex[0] >> ch >> normalIndex[0];
				}
				//通过索引值创建顶点
				//所谓索引就是我们之前创建好的m_vertices，m_textures，m_normals对应下标
				for(int i = 0; i != 3; ++i)
				{
					vertex[i].vPosition = m_vertices[posIndex[i] - 1];  //位置
					vertex[i].vTexCoord = m_textures[texIndex[i] - 1];  //纹理
					vertex[i].vNormal = m_normals[normalIndex[i] - 1];  //法向量

					// 收集所有顶点
					this->m_meshVertices.push_back(vertex[i]);

					lIndexCount++;
				}
			}
		}
		inFile.ignore( 1024 , '\n' );

		if( inFile.eof() )
			break;
	}
	// pSubMesh还要添加一次
	if(pSubMesh != NULL)
	{
		pSubMesh->lStartIndex = lStartIndex;
		pSubMesh->lIndexCount = lIndexCount - lStartIndex;
		m_Mesh.push_back(*pSubMesh);
	}

	return true;
}

//创建顶点缓存
bool ObjLoader::CreateBuffers(ID3D11Device* device)
{
	// 创建mesh顶点和索引缓存
	ObjVertex* vertices;
	unsigned int* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc; //缓存描述
	D3D11_SUBRESOURCE_DATA vertexData, indexData;        //子资源对象
	HRESULT result;

	int vertexCount = m_meshVertices.size();

	vertices = new ObjVertex[vertexCount];
	if(!vertices)
	{
		return false;
	}

	indices = new unsigned int[vertexCount];
	if(!indices)
	{
		return false;
	}

	//创建顶点数组
	for(int i = 0; i != vertexCount; ++i)
	{
		vertices[i].vPosition = m_meshVertices[i].vPosition;
		vertices[i].vNormal = m_meshVertices[i].vNormal;
		vertices[i].vTexCoord = m_meshVertices[i].vTexCoord;
		indices[i] = i;
	}

	//填充顶点缓存描述结构
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(ObjVertex) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	//创建顶点缓存
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &(this->m_meshVertexBuffer));
	if(FAILED(result))
	{
		return false;
	}
	//填充索引缓存描述结构
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned int) * vertexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	//创建索引缓存
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &(this->m_meshIndexBuffer));
	if(FAILED(result))
	{
		return false;
	}

	// buffers创建成功以后vertex和index数组就可已删除了，因为数据已经被复制到buffers中了
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}

//绘制模型
void ObjLoader::RenderEffect(ID3D11DeviceContext* deviceContext, ID3DX11EffectTechnique* pTechnique)
{
	UINT stride = sizeof(ObjVertex);
	UINT offset = 0;
	bool textureOn;

	deviceContext->IASetVertexBuffers( 0, 1, &this->m_meshVertexBuffer, &stride, &offset );//设置顶点缓存
	deviceContext->IASetIndexBuffer( this->m_meshIndexBuffer, DXGI_FORMAT_R32_UINT, 0 );  //设置索引缓存
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);         //设置图元类型

	for(std::vector<ObjSubMesh>::iterator iter = m_Mesh.begin(); iter != m_Mesh.end(); ++iter)
	{
		// 获取材质数据
		std::vector<ObjMaterial>::iterator iter2;
		//根据材质和纹理不同绘制物体各个部分
		for(iter2 = m_Material.begin(); iter2 != m_Material.end(); ++iter2)
		{
			if(iter2->strName == iter->strMaterialName)
			{
				if(iter2->pTextureRV != NULL)
				{
					::ID3DX11EffectShaderResourceVariable* fxShaderResource = 
						effect->GetVariableByName("Texture")->AsShaderResource();
					fxShaderResource->SetResource(iter2->pTextureRV);
					textureOn = true;
				}
				else
					textureOn = false;
				break;
			}
		}


			//设置材质信息
			effect->GetVariableByName("MatAmbient")->AsVector()->SetFloatVector((float*)&(iter2->vAmbient));
			effect->GetVariableByName("MatDiffuse")->AsVector()->SetFloatVector((float*)&(iter2->vDiffuse));
			effect->GetVariableByName("MatSpecular")->AsVector()->SetFloatVector((float*)&(iter2->vSpecular));
			effect->GetVariableByName("MatPower")->AsScalar()->SetFloat(iter2->nShininess);

			D3DX11_TECHNIQUE_DESC techDesc;
			pTechnique->GetDesc( &techDesc );
			//根据pass的个数循环绘制物体，本例中fx文件中只有每个technique只有一个pass所以只循环一次
			for(UINT i = 0; i < techDesc.Passes; ++i)
			{
				pTechnique->GetPassByIndex(i)->Apply(0,deviceContext);
				deviceContext->DrawIndexed( iter->lIndexCount, iter->lStartIndex, 0 );
			}
		
	}
}

//释放函数
void ObjLoader::Release()
{
	if(m_meshIndexBuffer)
	{
		m_meshIndexBuffer->Release();
		m_meshIndexBuffer = 0;
	}

	if(m_meshVertexBuffer)
	{
		m_meshVertexBuffer->Release();
		m_meshVertexBuffer = 0;
	}

	for(std::vector<ObjSubMesh>::iterator iter = m_Mesh.begin(); iter != m_Mesh.end(); ++iter)
	{
		iter->Release();
	}

	for(std::vector<ObjMaterial>::iterator iter = m_Material.begin(); iter != m_Material.end(); ++iter)
	{
		iter->Release();
	}
}

std::vector<ObjVertex> ObjLoader::getVertexInfo()
{
	return m_meshVertices;
}


void ObjLoader::setMaterialAlpha(float alpha)
{
	for(std::vector<ObjSubMesh>::iterator iter = m_Mesh.begin(); iter != m_Mesh.end(); ++iter)
	{
		// 获取材质数据
		std::vector<ObjMaterial>::iterator iter2;
		//根据材质和纹理不同绘制物体各个部分
		for(iter2 = m_Material.begin(); iter2 != m_Material.end(); ++iter2)
		{
			if(iter2->strName == iter->strMaterialName)
			{
				iter2->vDiffuse.w = alpha;
			}
		}
	}
	
}