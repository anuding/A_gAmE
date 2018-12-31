#include "GameObject.h"
#include <DDSTextureLoader.h>


GameObject::GameObject(D3DUtility* app)
{
	//CUBE 构造函数
	mapp = app;
	dev = app->device.Get();
	con = app->immediateContext.Get();
	mType = CUBE;

	InitCube();
}

GameObject::GameObject(D3DUtility * app, TYPE modelType, std::wstring filename,char *tag)
{
	//CUBE 构造函数
	mapp = app;
	dev = app->device.Get();
	con = app->immediateContext.Get();
	mType = modelType;
	strcpy_s(this->tag, tag);
	if (modelType == MD5)
	{
		LoadMD5Model(filename, NewMD5Model, meshSRV, textureNameArray);
		if (strcmp(this->tag, "player") == 0)
		{
			LoadMD5Anim(L"Models/Anim/player/fists_idle.md5anim", NewMD5Model); 
			LoadMD5Anim(L"Models/Anim/player/run.md5anim", NewMD5Model);
			LoadMD5Anim(L"Models/Anim/player/fists_punch1.md5anim", NewMD5Model);
		}
		if (strcmp(this->tag, "boss") == 0)
		{
			LoadMD5Anim(L"Models/Anim/boss/walk3.md5anim", NewMD5Model);
			LoadMD5Anim(L"Models/Anim/boss/oneshot.md5anim", NewMD5Model);
		}
		InitMd5();
	}
	if (modelType == OBJ)
	{
		LoadObjModel(L"ground.obj",
			&meshVertBuff,
			&meshIndexBuff,
			meshSubsetIndexStart,
			meshSubsetTexture,
			material,
			meshSubsets, true, true);
		InitMd5();
		// Describe the Sample State
		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(sampDesc));
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;


		//Create the Sample State
		dev->CreateSamplerState(&sampDesc, &CubesTexSamplerState);


		D3D11_BLEND_DESC blendDesc;
		ZeroMemory(&blendDesc, sizeof(blendDesc));

		D3D11_RENDER_TARGET_BLEND_DESC rtbd;

		ZeroMemory(&rtbd, sizeof(rtbd));

		rtbd.BlendEnable = true;
		rtbd.SrcBlend = D3D11_BLEND_INV_SRC_ALPHA;
		rtbd.DestBlend = D3D11_BLEND_SRC_ALPHA;
		rtbd.BlendOp = D3D11_BLEND_OP_ADD;
		rtbd.SrcBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		rtbd.DestBlendAlpha = D3D11_BLEND_SRC_ALPHA;
		rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

		blendDesc.AlphaToCoverageEnable = false;
		blendDesc.RenderTarget[0] = rtbd;

		dev->CreateBlendState(&blendDesc, &Transparency);
		D3D11_RASTERIZER_DESC cmdesc;

		ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
		cmdesc.FillMode = D3D11_FILL_SOLID;
		cmdesc.CullMode = D3D11_CULL_BACK;
		cmdesc.FrontCounterClockwise = true;
		dev->CreateRasterizerState(&cmdesc, &CCWcullMode);

		cmdesc.FrontCounterClockwise = false;

		dev->CreateRasterizerState(&cmdesc, &CWcullMode);

		cmdesc.CullMode = D3D11_CULL_NONE;
		//cmdesc.FillMode = D3D11_FILL_WIREFRAME;
		dev->CreateRasterizerState(&cmdesc, &RSCullNone);
	}
}

GameObject::~GameObject()
{

}

void GameObject::SetWorldMatrix(XMMATRIX mworld)
{
	World = mworld;
	XMVECTOR tmp = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	pos = XMVector4Transform(tmp, mworld);
}

void GameObject::SetViewMatrix(XMMATRIX mview)
{
	camView = mview;
}

void GameObject::SetProjMatrix(XMMATRIX mproj)
{
	camProjection = mproj;
}

XMVECTOR GameObject::GetPos()
{
	return pos;
}

bool GameObject::LoadObjModel(std::wstring filename,
	ID3D11Buffer** vertBuff,
	ID3D11Buffer** indexBuff,
	std::vector<int>& subsetIndexStart,
	std::vector<int>& subsetMaterialArray,
	std::vector<SurfaceMaterial>& material,
	int& subsetCount,
	bool isRHCoordSys,
	bool computeNormals)
{
	HRESULT hr = 0;

	std::wifstream fileIn(filename.c_str());	//Open file
	std::wstring meshMatLib;					//String to hold our obj material library filename

	//Arrays to store our model's information
	std::vector<DWORD> indices;
	std::vector<XMFLOAT3> vertPos;
	std::vector<XMFLOAT3> vertNorm;
	std::vector<XMFLOAT2> vertTexCoord;
	std::vector<std::wstring> meshMaterials;

	//Vertex definition indices
	std::vector<int> vertPosIndex;
	std::vector<int> vertNormIndex;
	std::vector<int> vertTCIndex;

	//Make sure we have a default if no tex coords or normals are defined
	bool hasTexCoord = false;
	bool hasNorm = false;

	//Temp variables to store into vectors
	std::wstring meshMaterialsTemp;
	int vertPosIndexTemp;
	int vertNormIndexTemp;
	int vertTCIndexTemp;

	wchar_t checkChar;		//The variable we will use to store one char from file at a time
	std::wstring face;		//Holds the string containing our face vertices
	int vIndex = 0;			//Keep track of our vertex index count
	int triangleCount = 0;	//Total Triangles
	int totalVerts = 0;
	int meshTriangles = 0;

	//Check to see if the file was opened
	
		while (fileIn)
		{
			checkChar = fileIn.get();	//Get next char

			switch (checkChar)
			{
			case '#':
				checkChar = fileIn.get();
				while (checkChar != '\n')
					checkChar = fileIn.get();
				break;
			case 'v':	//Get Vertex Descriptions
				checkChar = fileIn.get();
				if (checkChar == ' ')	//v - vert position
				{
					float vz, vy, vx;
					fileIn >> vx >> vy >> vz;	//Store the next three types

					if (isRHCoordSys)	//If model is from an RH Coord System
						vertPos.push_back(XMFLOAT3(vx, vy, vz * -1.0f));	//Invert the Z axis
					else
						vertPos.push_back(XMFLOAT3(vx, vy, vz));
				}
				if (checkChar == 't')	//vt - vert tex coords
				{
					float vtcu, vtcv;
					fileIn >> vtcu >> vtcv;		//Store next two types

					if (isRHCoordSys)	//If model is from an RH Coord System
						vertTexCoord.push_back(XMFLOAT2(vtcu, 1.0f - vtcv));	//Reverse the "v" axis
					else
						vertTexCoord.push_back(XMFLOAT2(vtcu, vtcv));

					hasTexCoord = true;	//We know the model uses texture coords
				}
				//Since we compute the normals later, we don't need to check for normals
				//In the file, but i'll do it here anyway
				if (checkChar == 'n')	//vn - vert normal
				{
					float vnx, vny, vnz;
					fileIn >> vnx >> vny >> vnz;	//Store next three types

					if (isRHCoordSys)	//If model is from an RH Coord System
						vertNorm.push_back(XMFLOAT3(vnx, vny, vnz * -1.0f));	//Invert the Z axis
					else
						vertNorm.push_back(XMFLOAT3(vnx, vny, vnz));

					hasNorm = true;	//We know the model defines normals
				}
				break;

				//New group (Subset)
			case 'g':	//g - defines a group
				checkChar = fileIn.get();
				if (checkChar == ' ')
				{
					subsetIndexStart.push_back(vIndex);		//Start index for this subset
					subsetCount++;
				}
				break;

				//Get Face Index
			case 'f':	//f - defines the faces
				checkChar = fileIn.get();
				if (checkChar == ' ')
				{
					face = L"";
					std::wstring VertDef;	//Holds one vertex definition at a time
					triangleCount = 0;

					checkChar = fileIn.get();
					while (checkChar != '\n')
					{
						face += checkChar;			//Add the char to our face string
						checkChar = fileIn.get();	//Get the next Character
						if (checkChar == ' ')		//If its a space...
							triangleCount++;		//Increase our triangle count
					}

					//Check for space at the end of our face string
					if (face[face.length() - 1] == ' ')
						triangleCount--;	//Each space adds to our triangle count

					triangleCount -= 1;		//Ever vertex in the face AFTER the first two are new faces

					std::wstringstream ss(face);

					if (face.length() > 0)
					{
						int firstVIndex, lastVIndex;	//Holds the first and last vertice's index

						for (int i = 0; i < 3; ++i)		//First three vertices (first triangle)
						{
							ss >> VertDef;	//Get vertex definition (vPos/vTexCoord/vNorm)

							std::wstring vertPart;
							int whichPart = 0;		//(vPos, vTexCoord, or vNorm)

							//Parse this string
							for (int j = 0; j < VertDef.length(); ++j)
							{
								if (VertDef[j] != '/')	//If there is no divider "/", add a char to our vertPart
									vertPart += VertDef[j];

								//If the current char is a divider "/", or its the last character in the string
								if (VertDef[j] == '/' || j == VertDef.length() - 1)
								{
									std::wistringstream wstringToInt(vertPart);	//Used to convert wstring to int

									if (whichPart == 0)	//If vPos
									{
										wstringToInt >> vertPosIndexTemp;
										vertPosIndexTemp -= 1;		//subtract one since c++ arrays start with 0, and obj start with 1

										//Check to see if the vert pos was the only thing specified
										if (j == VertDef.length() - 1)
										{
											vertNormIndexTemp = 0;
											vertTCIndexTemp = 0;
										}
									}

									else if (whichPart == 1)	//If vTexCoord
									{
										if (vertPart != L"")	//Check to see if there even is a tex coord
										{
											wstringToInt >> vertTCIndexTemp;
											vertTCIndexTemp -= 1;	//subtract one since c++ arrays start with 0, and obj start with 1
										}
										else	//If there is no tex coord, make a default
											vertTCIndexTemp = 0;

										//If the cur. char is the second to last in the string, then
										//there must be no normal, so set a default normal
										if (j == VertDef.length() - 1)
											vertNormIndexTemp = 0;

									}
									else if (whichPart == 2)	//If vNorm
									{
										std::wistringstream wstringToInt(vertPart);

										wstringToInt >> vertNormIndexTemp;
										vertNormIndexTemp -= 1;		//subtract one since c++ arrays start with 0, and obj start with 1
									}

									vertPart = L"";	//Get ready for next vertex part
									whichPart++;	//Move on to next vertex part					
								}
							}

							//Check to make sure there is at least one subset
							if (subsetCount == 0)
							{
								subsetIndexStart.push_back(vIndex);		//Start index for this subset
								subsetCount++;
							}

							//Avoid duplicate vertices
							bool vertAlreadyExists = false;
							if (totalVerts >= 3)	//Make sure we at least have one triangle to check
							{
								//Loop through all the vertices
								for (int iCheck = 0; iCheck < totalVerts; ++iCheck)
								{
									//If the vertex position and texture coordinate in memory are the same
									//As the vertex position and texture coordinate we just now got out
									//of the obj file, we will set this faces vertex index to the vertex's
									//index value in memory. This makes sure we don't create duplicate vertices
									if (vertPosIndexTemp == vertPosIndex[iCheck] && !vertAlreadyExists)
									{
										if (vertTCIndexTemp == vertTCIndex[iCheck])
										{
											indices.push_back(iCheck);		//Set index for this vertex
											vertAlreadyExists = true;		//If we've made it here, the vertex already exists
										}
									}
								}
							}

							//If this vertex is not already in our vertex arrays, put it there
							if (!vertAlreadyExists)
							{
								vertPosIndex.push_back(vertPosIndexTemp);
								vertTCIndex.push_back(vertTCIndexTemp);
								vertNormIndex.push_back(vertNormIndexTemp);
								totalVerts++;	//We created a new vertex
								indices.push_back(totalVerts - 1);	//Set index for this vertex
							}

							//If this is the very first vertex in the face, we need to
							//make sure the rest of the triangles use this vertex
							if (i == 0)
							{
								firstVIndex = indices[vIndex];	//The first vertex index of this FACE

							}

							//If this was the last vertex in the first triangle, we will make sure
							//the next triangle uses this one (eg. tri1(1,2,3) tri2(1,3,4) tri3(1,4,5))
							if (i == 2)
							{
								lastVIndex = indices[vIndex];	//The last vertex index of this TRIANGLE
							}
							vIndex++;	//Increment index count
						}

						meshTriangles++;	//One triangle down

						//If there are more than three vertices in the face definition, we need to make sure
						//we convert the face to triangles. We created our first triangle above, now we will
						//create a new triangle for every new vertex in the face, using the very first vertex
						//of the face, and the last vertex from the triangle before the current triangle
						for (int l = 0; l < triangleCount - 1; ++l)	//Loop through the next vertices to create new triangles
						{
							//First vertex of this triangle (the very first vertex of the face too)
							indices.push_back(firstVIndex);			//Set index for this vertex
							vIndex++;

							//Second Vertex of this triangle (the last vertex used in the tri before this one)
							indices.push_back(lastVIndex);			//Set index for this vertex
							vIndex++;

							//Get the third vertex for this triangle
							ss >> VertDef;

							std::wstring vertPart;
							int whichPart = 0;

							//Parse this string (same as above)
							for (int j = 0; j < VertDef.length(); ++j)
							{
								if (VertDef[j] != '/')
									vertPart += VertDef[j];
								if (VertDef[j] == '/' || j == VertDef.length() - 1)
								{
									std::wistringstream wstringToInt(vertPart);

									if (whichPart == 0)
									{
										wstringToInt >> vertPosIndexTemp;
										vertPosIndexTemp -= 1;

										//Check to see if the vert pos was the only thing specified
										if (j == VertDef.length() - 1)
										{
											vertTCIndexTemp = 0;
											vertNormIndexTemp = 0;
										}
									}
									else if (whichPart == 1)
									{
										if (vertPart != L"")
										{
											wstringToInt >> vertTCIndexTemp;
											vertTCIndexTemp -= 1;
										}
										else
											vertTCIndexTemp = 0;
										if (j == VertDef.length() - 1)
											vertNormIndexTemp = 0;

									}
									else if (whichPart == 2)
									{
										std::wistringstream wstringToInt(vertPart);

										wstringToInt >> vertNormIndexTemp;
										vertNormIndexTemp -= 1;
									}

									vertPart = L"";
									whichPart++;
								}
							}

							//Check for duplicate vertices
							bool vertAlreadyExists = false;
							if (totalVerts >= 3)	//Make sure we at least have one triangle to check
							{
								for (int iCheck = 0; iCheck < totalVerts; ++iCheck)
								{
									if (vertPosIndexTemp == vertPosIndex[iCheck] && !vertAlreadyExists)
									{
										if (vertTCIndexTemp == vertTCIndex[iCheck])
										{
											indices.push_back(iCheck);			//Set index for this vertex
											vertAlreadyExists = true;		//If we've made it here, the vertex already exists
										}
									}
								}
							}

							if (!vertAlreadyExists)
							{
								vertPosIndex.push_back(vertPosIndexTemp);
								vertTCIndex.push_back(vertTCIndexTemp);
								vertNormIndex.push_back(vertNormIndexTemp);
								totalVerts++;					//New vertex created, add to total verts
								indices.push_back(totalVerts - 1);		//Set index for this vertex
							}

							//Set the second vertex for the next triangle to the last vertex we got		
							lastVIndex = indices[vIndex];	//The last vertex index of this TRIANGLE

							meshTriangles++;	//New triangle defined
							vIndex++;
						}
					}
				}
				break;

			case 'm':	//mtllib - material library filename
				checkChar = fileIn.get();
				if (checkChar == 't')
				{
					checkChar = fileIn.get();
					if (checkChar == 'l')
					{
						checkChar = fileIn.get();
						if (checkChar == 'l')
						{
							checkChar = fileIn.get();
							if (checkChar == 'i')
							{
								checkChar = fileIn.get();
								if (checkChar == 'b')
								{
									checkChar = fileIn.get();
									if (checkChar == ' ')
									{
										//Store the material libraries file name
										fileIn >> meshMatLib;
									}
								}
							}
						}
					}
				}

				break;

			case 'u':	//usemtl - which material to use
				checkChar = fileIn.get();
				if (checkChar == 's')
				{
					checkChar = fileIn.get();
					if (checkChar == 'e')
					{
						checkChar = fileIn.get();
						if (checkChar == 'm')
						{
							checkChar = fileIn.get();
							if (checkChar == 't')
							{
								checkChar = fileIn.get();
								if (checkChar == 'l')
								{
									checkChar = fileIn.get();
									if (checkChar == ' ')
									{
										meshMaterialsTemp = L"";	//Make sure this is cleared

										fileIn >> meshMaterialsTemp; //Get next type (string)

										meshMaterials.push_back(meshMaterialsTemp);
									}
								}
							}
						}
					}
				}
				break;

			default:
				break;
			}
		}
	
	

	subsetIndexStart.push_back(vIndex); //There won't be another index start after our last subset, so set it here

	//sometimes "g" is defined at the very top of the file, then again before the first group of faces.
	//This makes sure the first subset does not conatain "0" indices.
	if (subsetIndexStart[1] == 0)
	{
		subsetIndexStart.erase(subsetIndexStart.begin() + 1);
		meshSubsets--;
	}

	//Make sure we have a default for the tex coord and normal
	//if one or both are not specified
	if (!hasNorm)
		vertNorm.push_back(XMFLOAT3(0.0f, 0.0f, 0.0f));
	if (!hasTexCoord)
		vertTexCoord.push_back(XMFLOAT2(0.0f, 0.0f));

	//Close the obj file, and open the mtl file
	fileIn.close();
	fileIn.open(meshMatLib.c_str());

	std::wstring lastStringRead;
	int matCount = material.size();	//total materials

	//kdset - If our diffuse color was not set, we can use the ambient color (which is usually the same)
	//If the diffuse color WAS set, then we don't need to set our diffuse color to ambient
	bool kdset = false;


		while (fileIn)
		{
			checkChar = fileIn.get();	//Get next char

			switch (checkChar)
			{
				//Check for comment
			case '#':
				checkChar = fileIn.get();
				while (checkChar != '\n')
					checkChar = fileIn.get();
				break;

				//Set diffuse color
			case 'K':
				checkChar = fileIn.get();
				if (checkChar == 'd')	//Diffuse Color
				{
					checkChar = fileIn.get();	//remove space

					fileIn >> material[matCount - 1].difColor.x;
					fileIn >> material[matCount - 1].difColor.y;
					fileIn >> material[matCount - 1].difColor.z;

					kdset = true;
				}

				//Ambient Color (We'll store it in diffuse if there isn't a diffuse already)
				if (checkChar == 'a')
				{
					checkChar = fileIn.get();	//remove space
					if (!kdset)
					{
						fileIn >> material[matCount - 1].difColor.x;
						fileIn >> material[matCount - 1].difColor.y;
						fileIn >> material[matCount - 1].difColor.z;
					}
				}
				break;

				//Check for transparency
			case 'T':
				checkChar = fileIn.get();
				if (checkChar == 'r')
				{
					checkChar = fileIn.get();	//remove space
					float Transparency;
					fileIn >> Transparency;

					material[matCount - 1].difColor.w = Transparency;

					if (Transparency > 0.0f)
						material[matCount - 1].transparent = true;
				}
				break;

				//Some obj files specify d for transparency
			case 'd':
				checkChar = fileIn.get();
				if (checkChar == ' ')
				{
					float Transparency;
					fileIn >> Transparency;

					//'d' - 0 being most transparent, and 1 being opaque, opposite of Tr
					Transparency = 1.0f - Transparency;

					material[matCount - 1].difColor.w = Transparency;

					if (Transparency > 0.0f)
						material[matCount - 1].transparent = true;
				}
				break;

				//Get the diffuse map (texture)
			case 'm':
				checkChar = fileIn.get();
				if (checkChar == 'a')
				{
					checkChar = fileIn.get();
					if (checkChar == 'p')
					{
						checkChar = fileIn.get();
						if (checkChar == '_')
						{
							//map_Kd - Diffuse map
							checkChar = fileIn.get();
							if (checkChar == 'K')
							{
								checkChar = fileIn.get();
								if (checkChar == 'd')
								{
									std::wstring fileNamePath;

									fileIn.get();	//Remove whitespace between map_Kd and file

									//Get the file path - We read the pathname char by char since
									//pathnames can sometimes contain spaces, so we will read until
									//we find the file extension
									bool texFilePathEnd = false;
									while (!texFilePathEnd)
									{
										checkChar = fileIn.get();

										fileNamePath += checkChar;

										if (checkChar == '.')
										{
											for (int i = 0; i < 3; ++i)
												fileNamePath += fileIn.get();

											texFilePathEnd = true;
										}
									}

									//check if this texture has already been loaded
									bool alreadyLoaded = false;
									for (int i = 0; i < textureNameArray.size(); ++i)
									{
										if (fileNamePath == textureNameArray[i])
										{
											alreadyLoaded = true;
											material[matCount - 1].texArrayIndex = i;
											material[matCount - 1].hasTexture = true;
										}
									}

									//if the texture is not already loaded, load it now
									if (!alreadyLoaded)
									{
										ID3D11ShaderResourceView* tempMeshSRV;
										/*hr = D3DX11CreateShaderResourceViewFromFile(d3d11Device, fileNamePath.c_str(),
											NULL, NULL, &tempMeshSRV, NULL);*/
										DirectX::CreateDDSTextureFromFile(dev,
											fileNamePath.c_str(), nullptr, &tempMeshSRV);
										if (SUCCEEDED(hr))
										{
											textureNameArray.push_back(fileNamePath.c_str());
											material[matCount - 1].texArrayIndex = meshSRV.size();
											meshSRV.push_back(tempMeshSRV);
											material[matCount - 1].hasTexture = true;
										}
									}
								}
							}
							//map_d - alpha map
							else if (checkChar == 'd')
							{
								//Alpha maps are usually the same as the diffuse map
								//So we will assume that for now by only enabling
								//transparency for this material, as we will already
								//be using the alpha channel in the diffuse map
								material[matCount - 1].transparent = true;
							}

							//map_bump - bump map (we're usinga normal map though)
							else if (checkChar == 'b')
							{
								checkChar = fileIn.get();
								if (checkChar == 'u')
								{
									checkChar = fileIn.get();
									if (checkChar == 'm')
									{
										checkChar = fileIn.get();
										if (checkChar == 'p')
										{
											std::wstring fileNamePath;

											fileIn.get();	//Remove whitespace between map_bump and file

											//Get the file path - We read the pathname char by char since
											//pathnames can sometimes contain spaces, so we will read until
											//we find the file extension
											bool texFilePathEnd = false;
											while (!texFilePathEnd)
											{
												checkChar = fileIn.get();

												fileNamePath += checkChar;

												if (checkChar == '.')
												{
													for (int i = 0; i < 3; ++i)
														fileNamePath += fileIn.get();

													texFilePathEnd = true;
												}
											}

											//check if this texture has already been loaded
											bool alreadyLoaded = false;
											for (int i = 0; i < textureNameArray.size(); ++i)
											{
												if (fileNamePath == textureNameArray[i])
												{
													alreadyLoaded = true;
													material[matCount - 1].normMapTexArrayIndex = i;
													material[matCount - 1].hasNormMap = true;
												}
											}

											//if the texture is not already loaded, load it now
											if (!alreadyLoaded)
											{
												ID3D11ShaderResourceView* tempMeshSRV;
												//hr = D3DX11CreateShaderResourceViewFromFile(d3d11Device, fileNamePath.c_str(),
												//	NULL, NULL, &tempMeshSRV, NULL);
												DirectX::CreateDDSTextureFromFile(dev,
													fileNamePath.c_str(), nullptr, &tempMeshSRV);
												if (SUCCEEDED(hr))
												{
													textureNameArray.push_back(fileNamePath.c_str());
													material[matCount - 1].normMapTexArrayIndex = meshSRV.size();
													meshSRV.push_back(tempMeshSRV);
													material[matCount - 1].hasNormMap = true;
												}
											}
										}
									}
								}
							}
						}
					}
				}
				break;

			case 'n':	//newmtl - Declare new material
				checkChar = fileIn.get();
				if (checkChar == 'e')
				{
					checkChar = fileIn.get();
					if (checkChar == 'w')
					{
						checkChar = fileIn.get();
						if (checkChar == 'm')
						{
							checkChar = fileIn.get();
							if (checkChar == 't')
							{
								checkChar = fileIn.get();
								if (checkChar == 'l')
								{
									checkChar = fileIn.get();
									if (checkChar == ' ')
									{
										//New material, set its defaults
										SurfaceMaterial tempMat;
										material.push_back(tempMat);
										fileIn >> material[matCount].matName;
										material[matCount].transparent = false;
										material[matCount].hasTexture = false;
										material[matCount].hasNormMap = false;
										material[matCount].normMapTexArrayIndex = 0;
										material[matCount].texArrayIndex = 0;
										matCount++;
										kdset = false;
									}
								}
							}
						}
					}
				}
				break;

			default:
				break;
			}
		}
	


	//Set the subsets material to the index value
	//of the its material in our material array
	for (int i = 0; i < meshSubsets; ++i)
	{
		bool hasMat = false;
		for (int j = 0; j < material.size(); ++j)
		{
			if (meshMaterials[i] == material[j].matName)
			{
				subsetMaterialArray.push_back(j);
				hasMat = true;
			}
		}
		if (!hasMat)
			subsetMaterialArray.push_back(0); //Use first material in array
	}

	std::vector<Vertex> vertices;
	Vertex tempVert;

	//Create our vertices using the information we got 
	//from the file and store them in a vector
	for (int j = 0; j < totalVerts; ++j)
	{
		tempVert.pos = vertPos[vertPosIndex[j]];
		tempVert.normal = vertNorm[vertNormIndex[j]];
		tempVert.texCoord = vertTexCoord[vertTCIndex[j]];

		vertices.push_back(tempVert);
	}

	//////////////////////Compute Normals///////////////////////////
	//If computeNormals was set to true then we will create our own
	//normals, if it was set to false we will use the obj files normals
	if (computeNormals)
	{
		std::vector<XMFLOAT3> tempNormal;

		//normalized and unnormalized normals
		XMFLOAT3 unnormalized = XMFLOAT3(0.0f, 0.0f, 0.0f);

		//tangent stuff
		std::vector<XMFLOAT3> tempTangent;
		XMFLOAT3 tangent = XMFLOAT3(0.0f, 0.0f, 0.0f);
		float tcU1, tcV1, tcU2, tcV2;

		//Used to get vectors (sides) from the position of the verts
		float vecX, vecY, vecZ;

		//Two edges of our triangle
		XMVECTOR edge1 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR edge2 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		//Compute face normals
		//And Tangents
		for (int i = 0; i < meshTriangles; ++i)
		{
			//Get the vector describing one edge of our triangle (edge 0,2)
			vecX = vertices[indices[(i * 3)]].pos.x - vertices[indices[(i * 3) + 2]].pos.x;
			vecY = vertices[indices[(i * 3)]].pos.y - vertices[indices[(i * 3) + 2]].pos.y;
			vecZ = vertices[indices[(i * 3)]].pos.z - vertices[indices[(i * 3) + 2]].pos.z;
			edge1 = XMVectorSet(vecX, vecY, vecZ, 0.0f);	//Create our first edge

			//Get the vector describing another edge of our triangle (edge 2,1)
			vecX = vertices[indices[(i * 3) + 2]].pos.x - vertices[indices[(i * 3) + 1]].pos.x;
			vecY = vertices[indices[(i * 3) + 2]].pos.y - vertices[indices[(i * 3) + 1]].pos.y;
			vecZ = vertices[indices[(i * 3) + 2]].pos.z - vertices[indices[(i * 3) + 1]].pos.z;
			edge2 = XMVectorSet(vecX, vecY, vecZ, 0.0f);	//Create our second edge

			//Cross multiply the two edge vectors to get the un-normalized face normal
			XMStoreFloat3(&unnormalized, XMVector3Cross(edge1, edge2));

			tempNormal.push_back(unnormalized);

			//Find first texture coordinate edge 2d vector
			tcU1 = vertices[indices[(i * 3)]].texCoord.x - vertices[indices[(i * 3) + 2]].texCoord.x;
			tcV1 = vertices[indices[(i * 3)]].texCoord.y - vertices[indices[(i * 3) + 2]].texCoord.y;

			//Find second texture coordinate edge 2d vector
			tcU2 = vertices[indices[(i * 3) + 2]].texCoord.x - vertices[indices[(i * 3) + 1]].texCoord.x;
			tcV2 = vertices[indices[(i * 3) + 2]].texCoord.y - vertices[indices[(i * 3) + 1]].texCoord.y;

			//Find tangent using both tex coord edges and position edges
			tangent.x = (tcV1 * XMVectorGetX(edge1) - tcV2 * XMVectorGetX(edge2)) * (1.0f / (tcU1 * tcV2 - tcU2 * tcV1));
			tangent.y = (tcV1 * XMVectorGetY(edge1) - tcV2 * XMVectorGetY(edge2)) * (1.0f / (tcU1 * tcV2 - tcU2 * tcV1));
			tangent.z = (tcV1 * XMVectorGetZ(edge1) - tcV2 * XMVectorGetZ(edge2)) * (1.0f / (tcU1 * tcV2 - tcU2 * tcV1));

			tempTangent.push_back(tangent);
		}

		//Compute vertex normals (normal Averaging)
		XMVECTOR normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR tangentSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		int facesUsing = 0;
		float tX, tY, tZ;	//temp axis variables

		//Go through each vertex
		for (int i = 0; i < totalVerts; ++i)
		{
			//Check which triangles use this vertex
			for (int j = 0; j < meshTriangles; ++j)
			{
				if (indices[j * 3] == i ||
					indices[(j * 3) + 1] == i ||
					indices[(j * 3) + 2] == i)
				{
					tX = XMVectorGetX(normalSum) + tempNormal[j].x;
					tY = XMVectorGetY(normalSum) + tempNormal[j].y;
					tZ = XMVectorGetZ(normalSum) + tempNormal[j].z;

					normalSum = XMVectorSet(tX, tY, tZ, 0.0f);	//If a face is using the vertex, add the unormalized face normal to the normalSum

					//We can reuse tX, tY, tZ to sum up tangents
					tX = XMVectorGetX(tangentSum) + tempTangent[j].x;
					tY = XMVectorGetY(tangentSum) + tempTangent[j].y;
					tZ = XMVectorGetZ(tangentSum) + tempTangent[j].z;

					tangentSum = XMVectorSet(tX, tY, tZ, 0.0f); //sum up face tangents using this vertex

					facesUsing++;
				}
			}

			//Get the actual normal by dividing the normalSum by the number of faces sharing the vertex
			normalSum = normalSum / facesUsing;
			tangentSum = tangentSum / facesUsing;

			//Normalize the normalSum vector and tangent
			normalSum = XMVector3Normalize(normalSum);
			tangentSum = XMVector3Normalize(tangentSum);

			//Store the normal and tangent in our current vertex
			vertices[i].normal.x = XMVectorGetX(normalSum);
			vertices[i].normal.y = XMVectorGetY(normalSum);
			vertices[i].normal.z = XMVectorGetZ(normalSum);

			vertices[i].tangent.x = XMVectorGetX(tangentSum);
			vertices[i].tangent.y = XMVectorGetY(tangentSum);
			vertices[i].tangent.z = XMVectorGetZ(tangentSum);

			//Clear normalSum, tangentSum and facesUsing for next vertex
			normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
			tangentSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
			facesUsing = 0;

		}
	}

	//Create index buffer
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * meshTriangles * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData;

	iinitData.pSysMem = &indices[0];
	dev->CreateBuffer(&indexBufferDesc, &iinitData, indexBuff);

	//Create Vertex Buffer
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * totalVerts;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;

	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = &vertices[0];
	hr = dev->CreateBuffer(&vertexBufferDesc, &vertexBufferData, vertBuff);

	return true;
}



bool GameObject::InitCube()
{
	ID3DBlob* compilationMsgs = nullptr;
	HRESULT hr = S_OK;

	D3DCompileFromFile(L"Shader/Shader.fx", 0, 0, "VS", "vs_5_0", 0, 0, &VS_Buffer, &compilationMsgs);
	D3DCompileFromFile(L"Shader/Shader.fx", 0, 0, "PS", "ps_5_0", 0, 0, &PS_Buffer, &compilationMsgs);
	//Create the Shader Objects
	hr = dev->CreateVertexShader(VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), NULL, &mVertexShader);
	hr = dev->CreatePixelShader(PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), NULL, &mPixelShader);

	//create input layout
	dev->CreateInputLayout(inputLayout, ARRAYSIZE(inputLayout),
		VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), mVertexLayout.GetAddressOf());

	VertexForCube vertices[] =
	{
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) }
	};
	// 设置顶点缓冲区描述
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(vertices);
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	// 新建顶点缓冲区
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;
	dev->CreateBuffer(&vbd, &InitData, mVertexBuffer.GetAddressOf());

	// ******************
	// 索引数组
	WORD indices[] = {
		// 正面
		0, 1, 2,
		2, 3, 0,
		// 左面
		4, 5, 1,
		1, 0, 4,
		// 顶面
		1, 5, 6,
		6, 2, 1,
		// 背面
		7, 6, 5,
		5, 4, 7,
		// 右面
		3, 2, 6,
		6, 7, 3,
		// 底面
		4, 0, 3,
		3, 7, 4
	};
	// 设置索引缓冲区描述
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(indices);
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	// 新建索引缓冲区
	InitData.pSysMem = indices;
	dev->CreateBuffer(&ibd, &InitData, mIndexBuffer.GetAddressOf());

	// ******************
	// 设置常量缓冲区描述
	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(cbd));
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.ByteWidth = sizeof(cbPerObject);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = 0;
	// 新建常量缓冲区，不使用初始数据
	dev->CreateBuffer(&cbd, nullptr, &cbPerObjectBuffer);

	return true;
}

void GameObject::DrawCube()
{
	cbPerObj.WVP = XMMatrixTranspose(smilesWorld*camView*camProjection);
	con->UpdateSubresource(cbPerObjectBuffer, 0, nullptr, &cbPerObj, 0, 0);
	UINT stride = sizeof(VertexForCube);	// 跨越字节数
	UINT offset = 0;						// 起始偏移量
	// 输入装配阶段的索引缓冲区设置
	con->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	con->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &stride, &offset);
	// 设置图元类型，设定输入布局
	con->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	con->IASetInputLayout(mVertexLayout.Get());
	// 将着色器绑定到渲染管线
	con->VSSetShader(mVertexShader.Get(), nullptr, 0);
	// 将更新好的常量缓冲区绑定到顶点着色器
	con->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
	con->PSSetShader(mPixelShader.Get(), nullptr, 0);

	// 绘制立方体
	con->DrawIndexed(36, 0, 0);
}

bool GameObject::LoadMD5Model(std::wstring filename,
	Model3D& MD5Model,
	std::vector<ID3D11ShaderResourceView*>& shaderResourceViewArray,
	std::vector<std::wstring> texFileNameArray)
{
	HRESULT hr = S_OK;
	std::wifstream fileIn(filename.c_str());		// Open file

	std::wstring checkString;						// Stores the next string from our file

	while (fileIn)								// Loop until the end of the file is reached
	{
		fileIn >> checkString;					// Get next string from file

		if (checkString == L"MD5Version")		// Get MD5 version (this function supports version 10)
		{
			/*fileIn >> checkString;
			MessageBox(0, checkString.c_str(),	//display message
			L"MD5Version", MB_OK);*/
		}
		else if (checkString == L"commandline")
		{
			std::getline(fileIn, checkString);	// Ignore the rest of this line
		}
		else if (checkString == L"numJoints")
		{
			fileIn >> MD5Model.numJoints;		// Store number of joints
		}
		else if (checkString == L"numMeshes")
		{
			fileIn >> MD5Model.numSubsets;		// Store number of meshes or subsets which we will call them
		}
		else if (checkString == L"joints")
		{
			Joint tempJoint;

			fileIn >> checkString;				// Skip the "{"

			for (int i = 0; i < MD5Model.numJoints; i++)
			{
				fileIn >> tempJoint.name;		// Store joints name
				// Sometimes the names might contain spaces. If that is the case, we need to continue
				// to read the name until we get to the closing " (quotation marks)
				if (tempJoint.name[tempJoint.name.size() - 1] != '"')
				{
					wchar_t checkChar;
					bool jointNameFound = false;
					while (!jointNameFound)
					{
						checkChar = fileIn.get();

						if (checkChar == '"')
							jointNameFound = true;

						tempJoint.name += checkChar;
					}
				}

				fileIn >> tempJoint.parentID;	// Store Parent joint's ID

				fileIn >> checkString;			// Skip the "("

				// Store position of this joint (swap y and z axis if model was made in RH Coord Sys)
				fileIn >> tempJoint.pos.x >> tempJoint.pos.z >> tempJoint.pos.y;

				fileIn >> checkString >> checkString;	// Skip the ")" and "("

				// Store orientation of this joint
				fileIn >> tempJoint.orientation.x >> tempJoint.orientation.z >> tempJoint.orientation.y;

				// Remove the quotation marks from joints name
				tempJoint.name.erase(0, 1);
				tempJoint.name.erase(tempJoint.name.size() - 1, 1);

				// Compute the w axis of the quaternion (The MD5 model uses a 3D vector to describe the
				// direction the bone is facing. However, we need to turn this into a quaternion, and the way
				// quaternions work, is the xyz values describe the axis of rotation, while the w is a value
				// between 0 and 1 which describes the angle of rotation)
				float t = 1.0f - (tempJoint.orientation.x * tempJoint.orientation.x)
					- (tempJoint.orientation.y * tempJoint.orientation.y)
					- (tempJoint.orientation.z * tempJoint.orientation.z);
				if (t < 0.0f)
				{
					tempJoint.orientation.w = 0.0f;
				}
				else
				{
					tempJoint.orientation.w = -sqrtf(t);
				}

				std::getline(fileIn, checkString);		// Skip rest of this line

				MD5Model.joints.push_back(tempJoint);	// Store the joint into this models joint vector
			}

			fileIn >> checkString;					// Skip the "}"
		}
		else if (checkString == L"mesh")
		{
			ModelSubset subset;
			int numVerts, numTris, numWeights;

			fileIn >> checkString;					// Skip the "{"

			fileIn >> checkString;
			while (checkString != L"}")			// Read until '}'
			{
				// In this lesson, for the sake of simplicity, we will assume a textures filename is givin here.
				// Usually though, the name of a material (stored in a material library. Think back to the lesson on
				// loading .obj files, where the material library was contained in the file .mtl) is givin. Let this
				// be an exercise to load the material from a material library such as obj's .mtl file, instead of
				// just the texture like we will do here.
				if (checkString == L"shader")		// Load the texture or material
				{
					std::wstring fileNamePath;
					fileIn >> fileNamePath;			// Get texture's filename

					// Take spaces into account if filename or material name has a space in it
					if (fileNamePath[fileNamePath.size() - 1] != '"')
					{
						wchar_t checkChar;
						bool fileNameFound = false;
						while (!fileNameFound)
						{
							checkChar = fileIn.get();

							if (checkChar == '"')
								fileNameFound = true;

							fileNamePath += checkChar;
						}
					}

					// Remove the quotation marks from texture path
					fileNamePath.erase(0, 1);
					fileNamePath.erase(fileNamePath.size() - 1, 1);

					//check if this texture has already been loaded
					bool alreadyLoaded = false;
					for (int i = 0; i < texFileNameArray.size(); ++i)
					{
						if (fileNamePath == texFileNameArray[i])
						{
							alreadyLoaded = true;
							subset.texArrayIndex = i;
						}
					}

					//if the texture is not already loaded, load it now
					if (!alreadyLoaded)
					{
						ID3D11ShaderResourceView* tempMeshSRV;
						//hr = D3DX11CreateShaderResourceViewFromFile( d3d11Device, fileNamePath.c_str(),
						//	NULL, NULL, &tempMeshSRV, NULL );
						hr = DirectX::CreateDDSTextureFromFile(dev,
							fileNamePath.c_str(), nullptr, &tempMeshSRV);
						//::MessageBox(NULL, fileNamePath.c_str(), L"ERROR", MB_OK);
						if (SUCCEEDED(hr))
						{
							texFileNameArray.push_back(fileNamePath.c_str());
							subset.texArrayIndex = shaderResourceViewArray.size();
							shaderResourceViewArray.push_back(tempMeshSRV);
						}
						else
						{
							MessageBox(0, fileNamePath.c_str(),		//display message
								L"Could Not Open:", MB_OK);
							return false;
						}
					}

					std::getline(fileIn, checkString);				// Skip rest of this line
				}
				else if (checkString == L"numverts")
				{
					fileIn >> numVerts;								// Store number of vertices

					std::getline(fileIn, checkString);				// Skip rest of this line

					for (int i = 0; i < numVerts; i++)
					{
						Vertex tempVert;

						fileIn >> checkString						// Skip "vert # ("
							>> checkString
							>> checkString;

						fileIn >> tempVert.texCoord.x				// Store tex coords
							>> tempVert.texCoord.y;

						fileIn >> checkString;						// Skip ")"

						fileIn >> tempVert.StartWeight;				// Index of first weight this vert will be weighted to

						fileIn >> tempVert.WeightCount;				// Number of weights for this vertex

						std::getline(fileIn, checkString);			// Skip rest of this line

						subset.vertices.push_back(tempVert);		// Push back this vertex into subsets vertex vector
					}
				}
				else if (checkString == L"numtris")
				{
					fileIn >> numTris;
					subset.numTriangles = numTris;

					std::getline(fileIn, checkString);				// Skip rest of this line

					for (int i = 0; i < numTris; i++)				// Loop through each triangle
					{
						DWORD tempIndex;
						fileIn >> checkString;						// Skip "tri"
						fileIn >> checkString;						// Skip tri counter

						for (int k = 0; k < 3; k++)					// Store the 3 indices
						{
							fileIn >> tempIndex;
							subset.indices.push_back(tempIndex);
						}

						std::getline(fileIn, checkString);			// Skip rest of this line
					}
				}
				else if (checkString == L"numweights")
				{
					fileIn >> numWeights;

					std::getline(fileIn, checkString);				// Skip rest of this line

					for (int i = 0; i < numWeights; i++)
					{
						Weight tempWeight;
						fileIn >> checkString >> checkString;		// Skip "weight #"

						fileIn >> tempWeight.jointID;				// Store weight's joint ID

						fileIn >> tempWeight.bias;					// Store weight's influence over a vertex

						fileIn >> checkString;						// Skip "("

						fileIn >> tempWeight.pos.x					// Store weight's pos in joint's local space
							>> tempWeight.pos.z
							>> tempWeight.pos.y;

						std::getline(fileIn, checkString);			// Skip rest of this line

						subset.weights.push_back(tempWeight);		// Push back tempWeight into subsets Weight array
					}

				}
				else
					std::getline(fileIn, checkString);				// Skip anything else

				fileIn >> checkString;								// Skip "}"
			}

			//*** find each vertex's position using the joints and weights ***//
			for (int i = 0; i < subset.vertices.size(); ++i)
			{
				Vertex tempVert = subset.vertices[i];
				tempVert.pos = XMFLOAT3(0, 0, 0);	// Make sure the vertex's pos is cleared first

				// Sum up the joints and weights information to get vertex's position
				for (int j = 0; j < tempVert.WeightCount; ++j)
				{
					Weight tempWeight = subset.weights[tempVert.StartWeight + j];
					Joint tempJoint = MD5Model.joints[tempWeight.jointID];

					// Convert joint orientation and weight pos to vectors for easier computation
					// When converting a 3d vector to a quaternion, you should put 0 for "w", and
					// When converting a quaternion to a 3d vector, you can just ignore the "w"
					XMVECTOR tempJointOrientation = XMVectorSet(tempJoint.orientation.x, tempJoint.orientation.y, tempJoint.orientation.z, tempJoint.orientation.w);
					XMVECTOR tempWeightPos = XMVectorSet(tempWeight.pos.x, tempWeight.pos.y, tempWeight.pos.z, 0.0f);

					// We will need to use the conjugate of the joint orientation quaternion
					// To get the conjugate of a quaternion, all you have to do is inverse the x, y, and z
					XMVECTOR tempJointOrientationConjugate = XMVectorSet(-tempJoint.orientation.x, -tempJoint.orientation.y, -tempJoint.orientation.z, tempJoint.orientation.w);

					// Calculate vertex position (in joint space, eg. rotate the point around (0,0,0)) for this weight using the joint orientation quaternion and its conjugate
					// We can rotate a point using a quaternion with the equation "rotatedPoint = quaternion * point * quaternionConjugate"
					XMFLOAT3 rotatedPoint;
					XMStoreFloat3(&rotatedPoint, XMQuaternionMultiply(XMQuaternionMultiply(tempJointOrientation, tempWeightPos), tempJointOrientationConjugate));

					// Now move the verices position from joint space (0,0,0) to the joints position in world space, taking the weights bias into account
					// The weight bias is used because multiple weights might have an effect on the vertices final position. Each weight is attached to one joint.
					tempVert.pos.x += (tempJoint.pos.x + rotatedPoint.x) * tempWeight.bias;
					tempVert.pos.y += (tempJoint.pos.y + rotatedPoint.y) * tempWeight.bias;
					tempVert.pos.z += (tempJoint.pos.z + rotatedPoint.z) * tempWeight.bias;

					// Basically what has happened above, is we have taken the weights position relative to the joints position
					// we then rotate the weights position (so that the weight is actually being rotated around (0, 0, 0) in world space) using
					// the quaternion describing the joints rotation. We have stored this rotated point in rotatedPoint, which we then add to
					// the joints position (because we rotated the weight's position around (0,0,0) in world space, and now need to translate it
					// so that it appears to have been rotated around the joints position). Finally we multiply the answer with the weights bias,
					// or how much control the weight has over the final vertices position. All weight's bias effecting a single vertex's position
					// must add up to 1.
				}

				subset.positions.push_back(tempVert.pos);			// Store the vertices position in the position vector instead of straight into the vertex vector
				// since we can use the positions vector for certain things like collision detection or picking
				// without having to work with the entire vertex structure.
			}

			// Put the positions into the vertices for this subset
			for (int i = 0; i < subset.vertices.size(); i++)
			{
				subset.vertices[i].pos = subset.positions[i];
			}

			//*** Calculate vertex normals using normal averaging ***///
			std::vector<XMFLOAT3> tempNormal;

			//normalized and unnormalized normals
			XMFLOAT3 unnormalized = XMFLOAT3(0.0f, 0.0f, 0.0f);

			//Used to get vectors (sides) from the position of the verts
			float vecX, vecY, vecZ;

			//Two edges of our triangle
			XMVECTOR edge1 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
			XMVECTOR edge2 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

			//Compute face normals
			for (int i = 0; i < subset.numTriangles; ++i)
			{
				//Get the vector describing one edge of our triangle (edge 0,2)
				vecX = subset.vertices[subset.indices[(i * 3)]].pos.x - subset.vertices[subset.indices[(i * 3) + 2]].pos.x;
				vecY = subset.vertices[subset.indices[(i * 3)]].pos.y - subset.vertices[subset.indices[(i * 3) + 2]].pos.y;
				vecZ = subset.vertices[subset.indices[(i * 3)]].pos.z - subset.vertices[subset.indices[(i * 3) + 2]].pos.z;
				edge1 = XMVectorSet(vecX, vecY, vecZ, 0.0f);	//Create our first edge

				//Get the vector describing another edge of our triangle (edge 2,1)
				vecX = subset.vertices[subset.indices[(i * 3) + 2]].pos.x - subset.vertices[subset.indices[(i * 3) + 1]].pos.x;
				vecY = subset.vertices[subset.indices[(i * 3) + 2]].pos.y - subset.vertices[subset.indices[(i * 3) + 1]].pos.y;
				vecZ = subset.vertices[subset.indices[(i * 3) + 2]].pos.z - subset.vertices[subset.indices[(i * 3) + 1]].pos.z;
				edge2 = XMVectorSet(vecX, vecY, vecZ, 0.0f);	//Create our second edge

				//Cross multiply the two edge vectors to get the un-normalized face normal
				XMStoreFloat3(&unnormalized, XMVector3Cross(edge1, edge2));

				tempNormal.push_back(unnormalized);
			}

			//Compute vertex normals (normal Averaging)
			XMVECTOR normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
			int facesUsing = 0;
			float tX, tY, tZ;	//temp axis variables

			//Go through each vertex
			for (int i = 0; i < subset.vertices.size(); ++i)
			{
				//Check which triangles use this vertex
				for (int j = 0; j < subset.numTriangles; ++j)
				{
					if (subset.indices[j * 3] == i ||
						subset.indices[(j * 3) + 1] == i ||
						subset.indices[(j * 3) + 2] == i)
					{
						tX = XMVectorGetX(normalSum) + tempNormal[j].x;
						tY = XMVectorGetY(normalSum) + tempNormal[j].y;
						tZ = XMVectorGetZ(normalSum) + tempNormal[j].z;

						normalSum = XMVectorSet(tX, tY, tZ, 0.0f);	//If a face is using the vertex, add the unormalized face normal to the normalSum

						facesUsing++;
					}
				}

				//Get the actual normal by dividing the normalSum by the number of faces sharing the vertex
				normalSum = normalSum / facesUsing;

				//Normalize the normalSum vector
				normalSum = XMVector3Normalize(normalSum);

				//Store the normal and tangent in our current vertex
				subset.vertices[i].normal.x = -XMVectorGetX(normalSum);
				subset.vertices[i].normal.y = -XMVectorGetY(normalSum);
				subset.vertices[i].normal.z = -XMVectorGetZ(normalSum);

				///////////////**************new**************////////////////////
				// Create the joint space normal for easy normal calculations in animation
				Vertex tempVert = subset.vertices[i];						// Get the current vertex
				subset.jointSpaceNormals.push_back(XMFLOAT3(0, 0, 0));		// Push back a blank normal
				XMVECTOR normal = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);		// Clear normal

				for (int k = 0; k < tempVert.WeightCount; k++)				// Loop through each of the vertices weights
				{
					Joint tempJoint = MD5Model.joints[subset.weights[tempVert.StartWeight + k].jointID];	// Get the joints orientation
					XMVECTOR jointOrientation = XMVectorSet(tempJoint.orientation.x, tempJoint.orientation.y, tempJoint.orientation.z, tempJoint.orientation.w);

					// Calculate normal based off joints orientation (turn into joint space)
					normal = XMQuaternionMultiply(XMQuaternionMultiply(XMQuaternionInverse(jointOrientation), normalSum), jointOrientation);

					XMStoreFloat3(&subset.weights[tempVert.StartWeight + k].normal, XMVector3Normalize(normal));			// Store the normalized quaternion into our weights normal
				}
				///////////////**************new**************////////////////////
				//Clear normalSum, facesUsing for next vertex
				normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
				facesUsing = 0;
			}

			// Create index buffer
			D3D11_BUFFER_DESC indexBufferDesc;
			ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

			indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			indexBufferDesc.ByteWidth = sizeof(DWORD) * subset.numTriangles * 3;
			indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			indexBufferDesc.CPUAccessFlags = 0;
			indexBufferDesc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA iinitData;
			iinitData.pSysMem = &subset.indices[0];
			dev->CreateBuffer(&indexBufferDesc, &iinitData, &subset.indexBuff);

			//Create Vertex Buffer
			D3D11_BUFFER_DESC vertexBufferDesc;
			ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

			vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;							// We will be updating this buffer, so we must set as dynamic
			vertexBufferDesc.ByteWidth = sizeof(Vertex) * subset.vertices.size();
			vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;				// Give CPU power to write to buffer
			vertexBufferDesc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA vertexBufferData;
			ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
			vertexBufferData.pSysMem = &subset.vertices[0];
			hr = dev->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &subset.vertBuff);

			// Push back the temp subset into the models subset vector
			MD5Model.subsets.push_back(subset);
		}
	}


	return true;
}


bool GameObject::InitMd5()
{
	//Compile Shaders from shader file
	ID3DBlob* compilationMsgs = nullptr;
	HRESULT hr = S_OK;

	D3DCompileFromFile(L"Effects.fx", 0, 0, "VS", "vs_5_0", 0, 0, &VS_Buffer, &compilationMsgs);
	D3DCompileFromFile(L"Effects.fx", 0, 0, "PS", "ps_5_0", 0, 0, &PS_Buffer, &compilationMsgs);

	//Create the Shader Objects
	hr = dev->CreateVertexShader(VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), NULL, &VS);
	hr = dev->CreatePixelShader(PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), NULL, &PS);


	light.pos = XMFLOAT3(0.0f, 7.0f, 0.0f);
	light.dir = XMFLOAT3(0.5f, 0.75f, -0.5f);
	light.range = 1000.0f;
	light.cone = 12.0f;
	light.att = XMFLOAT3(0.4f, 0.02f, 0.000f);
	light.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	light.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",	 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	UINT numElements = ARRAYSIZE(layout);
	//Create the Input Layout
	hr = dev->CreateInputLayout(layout, numElements, VS_Buffer->GetBufferPointer(),
		VS_Buffer->GetBufferSize(), &vertLayout);


	//Create the buffer to send to the cbuffer in effect file
	D3D11_BUFFER_DESC cbbd;
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof(cbPerObject);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	hr = dev->CreateBuffer(&cbbd, NULL, &cbPerObjectBuffer);

	//Create the buffer to send to the cbuffer per frame in effect file
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof(cbPerFrame);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	hr = dev->CreateBuffer(&cbbd, NULL, &cbPerFrameBuffer);

	return true;
}

bool GameObject::LoadMD5Anim(std::wstring filename, Model3D& MD5Model)
{
	ModelAnimation tempAnim;						// Temp animation to later store in our model's animation array

	std::wifstream fileIn(filename.c_str());		// Open file

	std::wstring checkString;						// Stores the next string from our file

	while (fileIn)								// Loop until the end of the file is reached
	{
		fileIn >> checkString;					// Get next string from file

		if (checkString == L"MD5Version")		// Get MD5 version (this function supports version 10)
		{
			fileIn >> checkString;
			/*MessageBox(0, checkString.c_str(),	//display message
			L"MD5Version", MB_OK);*/
		}
		else if (checkString == L"commandline")
		{
			std::getline(fileIn, checkString);	// Ignore the rest of this line
		}
		else if (checkString == L"numFrames")
		{
			fileIn >> tempAnim.numFrames;				// Store number of frames in this animation
		}
		else if (checkString == L"numJoints")
		{
			fileIn >> tempAnim.numJoints;				// Store number of joints (must match .md5mesh)
		}
		else if (checkString == L"frameRate")
		{
			fileIn >> tempAnim.frameRate;				// Store animation's frame rate (frames per second)
		}
		else if (checkString == L"numAnimatedComponents")
		{
			fileIn >> tempAnim.numAnimatedComponents;	// Number of components in each frame section
		}
		else if (checkString == L"hierarchy")
		{
			fileIn >> checkString;				// Skip opening bracket "{"

			for (int i = 0; i < tempAnim.numJoints; i++)	// Load in each joint
			{
				AnimJointInfo tempJoint;

				fileIn >> tempJoint.name;		// Get joints name
				// Sometimes the names might contain spaces. If that is the case, we need to continue
				// to read the name until we get to the closing " (quotation marks)
				if (tempJoint.name[tempJoint.name.size() - 1] != '"')
				{
					wchar_t checkChar;
					bool jointNameFound = false;
					while (!jointNameFound)
					{
						checkChar = fileIn.get();

						if (checkChar == '"')
							jointNameFound = true;

						tempJoint.name += checkChar;
					}
				}

				// Remove the quotation marks from joints name
				tempJoint.name.erase(0, 1);
				tempJoint.name.erase(tempJoint.name.size() - 1, 1);

				fileIn >> tempJoint.parentID;			// Get joints parent ID
				fileIn >> tempJoint.flags;				// Get flags
				fileIn >> tempJoint.startIndex;			// Get joints start index

				// Make sure the joint exists in the model, and the parent ID's match up
				// because the bind pose (md5mesh) joint hierarchy and the animations (md5anim)
				// joint hierarchy must match up
				bool jointMatchFound = false;
				for (int k = 0; k < MD5Model.numJoints; k++)
				{
					if (MD5Model.joints[k].name == tempJoint.name)
					{
						if (MD5Model.joints[k].parentID == tempJoint.parentID)
						{
							jointMatchFound = true;
							tempAnim.jointInfo.push_back(tempJoint);
						}
					}
				}
				if (!jointMatchFound)					// If the skeleton system does not match up, return false
					return false;						// You might want to add an error message here

				std::getline(fileIn, checkString);		// Skip rest of this line
			}
		}
		else if (checkString == L"bounds")			// Load in the AABB for each animation
		{
			fileIn >> checkString;						// Skip opening bracket "{"

			for (int i = 0; i < tempAnim.numFrames; i++)
			{
				BoundingBox tempBB;

				fileIn >> checkString;					// Skip "("
				fileIn >> tempBB.min.x >> tempBB.min.z >> tempBB.min.y;
				fileIn >> checkString >> checkString;	// Skip ") ("
				fileIn >> tempBB.max.x >> tempBB.max.z >> tempBB.max.y;
				fileIn >> checkString;					// Skip ")"

				tempAnim.frameBounds.push_back(tempBB);
			}
		}
		else if (checkString == L"baseframe")			// This is the default position for the animation
		{												// All frames will build their skeletons off this
			fileIn >> checkString;						// Skip opening bracket "{"

			for (int i = 0; i < tempAnim.numJoints; i++)
			{
				Joint tempBFJ;

				fileIn >> checkString;						// Skip "("
				fileIn >> tempBFJ.pos.x >> tempBFJ.pos.z >> tempBFJ.pos.y;
				fileIn >> checkString >> checkString;		// Skip ") ("
				fileIn >> tempBFJ.orientation.x >> tempBFJ.orientation.z >> tempBFJ.orientation.y;
				fileIn >> checkString;						// Skip ")"

				tempAnim.baseFrameJoints.push_back(tempBFJ);
			}
		}
		else if (checkString == L"frame")		// Load in each frames skeleton (the parts of each joint that changed from the base frame)
		{
			FrameData tempFrame;

			fileIn >> tempFrame.frameID;		// Get the frame ID

			fileIn >> checkString;				// Skip opening bracket "{"

			for (int i = 0; i < tempAnim.numAnimatedComponents; i++)
			{
				float tempData;
				fileIn >> tempData;				// Get the data

				tempFrame.frameData.push_back(tempData);
			}

			tempAnim.frameData.push_back(tempFrame);

			///*** build the frame skeleton ***///
			std::vector<Joint> tempSkeleton;

			for (int i = 0; i < tempAnim.jointInfo.size(); i++)
			{
				int k = 0;						// Keep track of position in frameData array

				// Start the frames joint with the base frame's joint
				Joint tempFrameJoint = tempAnim.baseFrameJoints[i];

				tempFrameJoint.parentID = tempAnim.jointInfo[i].parentID;

				// Notice how I have been flipping y and z. this is because some modeling programs such as
				// 3ds max (which is what I use) use a right handed coordinate system. Because of this, we
				// need to flip the y and z axes. If your having problems loading some models, it's possible
				// the model was created in a left hand coordinate system. in that case, just reflip all the
				// y and z axes in our md5 mesh and anim loader.
				if (tempAnim.jointInfo[i].flags & 1)		// pos.x	( 000001 )
					tempFrameJoint.pos.x = tempFrame.frameData[tempAnim.jointInfo[i].startIndex + k++];

				if (tempAnim.jointInfo[i].flags & 2)		// pos.y	( 000010 )
					tempFrameJoint.pos.z = tempFrame.frameData[tempAnim.jointInfo[i].startIndex + k++];

				if (tempAnim.jointInfo[i].flags & 4)		// pos.z	( 000100 )
					tempFrameJoint.pos.y = tempFrame.frameData[tempAnim.jointInfo[i].startIndex + k++];

				if (tempAnim.jointInfo[i].flags & 8)		// orientation.x	( 001000 )
					tempFrameJoint.orientation.x = tempFrame.frameData[tempAnim.jointInfo[i].startIndex + k++];

				if (tempAnim.jointInfo[i].flags & 16)	// orientation.y	( 010000 )
					tempFrameJoint.orientation.z = tempFrame.frameData[tempAnim.jointInfo[i].startIndex + k++];

				if (tempAnim.jointInfo[i].flags & 32)	// orientation.z	( 100000 )
					tempFrameJoint.orientation.y = tempFrame.frameData[tempAnim.jointInfo[i].startIndex + k++];


				// Compute the quaternions w
				float t = 1.0f - (tempFrameJoint.orientation.x * tempFrameJoint.orientation.x)
					- (tempFrameJoint.orientation.y * tempFrameJoint.orientation.y)
					- (tempFrameJoint.orientation.z * tempFrameJoint.orientation.z);
				if (t < 0.0f)
				{
					tempFrameJoint.orientation.w = 0.0f;
				}
				else
				{
					tempFrameJoint.orientation.w = -sqrtf(t);
				}

				// Now, if the upper arm of your skeleton moves, you need to also move the lower part of your arm, and then the hands, and then finally the fingers (possibly weapon or tool too)
				// This is where joint hierarchy comes in. We start at the top of the hierarchy, and move down to each joints child, rotating and translating them based on their parents rotation
				// and translation. We can assume that by the time we get to the child, the parent has already been rotated and transformed based of it's parent. We can assume this because
				// the child should never come before the parent in the files we loaded in.
				if (tempFrameJoint.parentID >= 0)
				{
					Joint parentJoint = tempSkeleton[tempFrameJoint.parentID];

					// Turn the XMFLOAT3 and 4's into vectors for easier computation
					XMVECTOR parentJointOrientation = XMVectorSet(parentJoint.orientation.x, parentJoint.orientation.y, parentJoint.orientation.z, parentJoint.orientation.w);
					XMVECTOR tempJointPos = XMVectorSet(tempFrameJoint.pos.x, tempFrameJoint.pos.y, tempFrameJoint.pos.z, 0.0f);
					XMVECTOR parentOrientationConjugate = XMVectorSet(-parentJoint.orientation.x, -parentJoint.orientation.y, -parentJoint.orientation.z, parentJoint.orientation.w);

					// Calculate current joints position relative to its parents position
					XMFLOAT3 rotatedPos;
					XMStoreFloat3(&rotatedPos, XMQuaternionMultiply(XMQuaternionMultiply(parentJointOrientation, tempJointPos), parentOrientationConjugate));

					// Translate the joint to model space by adding the parent joint's pos to it
					tempFrameJoint.pos.x = rotatedPos.x + parentJoint.pos.x;
					tempFrameJoint.pos.y = rotatedPos.y + parentJoint.pos.y;
					tempFrameJoint.pos.z = rotatedPos.z + parentJoint.pos.z;

					// Currently the joint is oriented in its parent joints space, we now need to orient it in
					// model space by multiplying the two orientations together (parentOrientation * childOrientation) <- In that order
					XMVECTOR tempJointOrient = XMVectorSet(tempFrameJoint.orientation.x, tempFrameJoint.orientation.y, tempFrameJoint.orientation.z, tempFrameJoint.orientation.w);
					tempJointOrient = XMQuaternionMultiply(parentJointOrientation, tempJointOrient);

					// Normalize the orienation quaternion
					tempJointOrient = XMQuaternionNormalize(tempJointOrient);

					XMStoreFloat4(&tempFrameJoint.orientation, tempJointOrient);
				}

				// Store the joint into our temporary frame skeleton
				tempSkeleton.push_back(tempFrameJoint);
			}

			// Push back our newly created frame skeleton into the animation's frameSkeleton array
			tempAnim.frameSkeleton.push_back(tempSkeleton);

			fileIn >> checkString;				// Skip closing bracket "}"
		}
	}

	// Calculate and store some usefull animation data
	tempAnim.frameTime = 1.0f / tempAnim.frameRate;						// Set the time per frame
	tempAnim.totalAnimTime = tempAnim.numFrames * tempAnim.frameTime;	// Set the total time the animation takes
	tempAnim.currAnimTime = 0.0f;										// Set the current time to zero

	MD5Model.animations.push_back(tempAnim);							// Push back the animation into our model object


	return true;
}

void GameObject::UpdateMD5Model(Model3D& MD5Model, float deltaTime, int animation)
{
	MD5Model.animations[animation].currAnimTime += deltaTime;			// Update the current animation time

	if (MD5Model.animations[animation].currAnimTime > MD5Model.animations[animation].totalAnimTime)
		MD5Model.animations[animation].currAnimTime = 0.0f;

	// Which frame are we on
	float currentFrame = MD5Model.animations[animation].currAnimTime * MD5Model.animations[animation].frameRate;
	int frame0 = floorf(currentFrame);
	int frame1 = frame0 + 1;

	// Make sure we don't go over the number of frames	
	if (frame0 == MD5Model.animations[animation].numFrames - 1)
		frame1 = 0;

	float interpolation = currentFrame - frame0;	// Get the remainder (in time) between frame0 and frame1 to use as interpolation factor

	std::vector<Joint> interpolatedSkeleton;		// Create a frame skeleton to store the interpolated skeletons in

	// Compute the interpolated skeleton
	for (int i = 0; i < MD5Model.animations[animation].numJoints; i++)
	{
		Joint tempJoint;
		Joint joint0 = MD5Model.animations[animation].frameSkeleton[frame0][i];		// Get the i'th joint of frame0's skeleton
		Joint joint1 = MD5Model.animations[animation].frameSkeleton[frame1][i];		// Get the i'th joint of frame1's skeleton

		tempJoint.parentID = joint0.parentID;											// Set the tempJoints parent id

		// Turn the two quaternions into XMVECTORs for easy computations
		XMVECTOR joint0Orient = XMVectorSet(joint0.orientation.x, joint0.orientation.y, joint0.orientation.z, joint0.orientation.w);
		XMVECTOR joint1Orient = XMVectorSet(joint1.orientation.x, joint1.orientation.y, joint1.orientation.z, joint1.orientation.w);

		// Interpolate positions
		tempJoint.pos.x = joint0.pos.x + (interpolation * (joint1.pos.x - joint0.pos.x));
		tempJoint.pos.y = joint0.pos.y + (interpolation * (joint1.pos.y - joint0.pos.y));
		tempJoint.pos.z = joint0.pos.z + (interpolation * (joint1.pos.z - joint0.pos.z));

		// Interpolate orientations using spherical interpolation (Slerp)
		XMStoreFloat4(&tempJoint.orientation, XMQuaternionSlerp(joint0Orient, joint1Orient, interpolation));

		interpolatedSkeleton.push_back(tempJoint);		// Push the joint back into our interpolated skeleton
	}

	for (int k = 0; k < MD5Model.numSubsets; k++)
	{
		for (int i = 0; i < MD5Model.subsets[k].vertices.size(); ++i)
		{
			Vertex tempVert = MD5Model.subsets[k].vertices[i];
			tempVert.pos = XMFLOAT3(0, 0, 0);	// Make sure the vertex's pos is cleared first
			tempVert.normal = XMFLOAT3(0, 0, 0);	// Clear vertices normal

			// Sum up the joints and weights information to get vertex's position and normal
			for (int j = 0; j < tempVert.WeightCount; ++j)
			{
				Weight tempWeight = MD5Model.subsets[k].weights[tempVert.StartWeight + j];
				Joint tempJoint = interpolatedSkeleton[tempWeight.jointID];

				// Convert joint orientation and weight pos to vectors for easier computation
				XMVECTOR tempJointOrientation = XMVectorSet(tempJoint.orientation.x, tempJoint.orientation.y, tempJoint.orientation.z, tempJoint.orientation.w);
				XMVECTOR tempWeightPos = XMVectorSet(tempWeight.pos.x, tempWeight.pos.y, tempWeight.pos.z, 0.0f);

				// We will need to use the conjugate of the joint orientation quaternion
				XMVECTOR tempJointOrientationConjugate = XMQuaternionInverse(tempJointOrientation);

				// Calculate vertex position (in joint space, eg. rotate the point around (0,0,0)) for this weight using the joint orientation quaternion and its conjugate
				// We can rotate a point using a quaternion with the equation "rotatedPoint = quaternion * point * quaternionConjugate"
				XMFLOAT3 rotatedPoint;
				XMStoreFloat3(&rotatedPoint, XMQuaternionMultiply(XMQuaternionMultiply(tempJointOrientation, tempWeightPos), tempJointOrientationConjugate));

				// Now move the verices position from joint space (0,0,0) to the joints position in world space, taking the weights bias into account
				tempVert.pos.x += (tempJoint.pos.x + rotatedPoint.x) * tempWeight.bias;
				tempVert.pos.y += (tempJoint.pos.y + rotatedPoint.y) * tempWeight.bias;
				tempVert.pos.z += (tempJoint.pos.z + rotatedPoint.z) * tempWeight.bias;

				// Compute the normals for this frames skeleton using the weight normals from before
				// We can comput the normals the same way we compute the vertices position, only we don't have to translate them (just rotate)
				XMVECTOR tempWeightNormal = XMVectorSet(tempWeight.normal.x, tempWeight.normal.y, tempWeight.normal.z, 0.0f);

				// Rotate the normal
				XMStoreFloat3(&rotatedPoint, XMQuaternionMultiply(XMQuaternionMultiply(tempJointOrientation, tempWeightNormal), tempJointOrientationConjugate));

				// Add to vertices normal and ake weight bias into account
				tempVert.normal.x -= rotatedPoint.x * tempWeight.bias;
				tempVert.normal.y -= rotatedPoint.y * tempWeight.bias;
				tempVert.normal.z -= rotatedPoint.z * tempWeight.bias;
			}

			MD5Model.subsets[k].positions[i] = tempVert.pos;				// Store the vertices position in the position vector instead of straight into the vertex vector
			MD5Model.subsets[k].vertices[i].normal = tempVert.normal;		// Store the vertices normal
			XMStoreFloat3(&MD5Model.subsets[k].vertices[i].normal, XMVector3Normalize(XMLoadFloat3(&MD5Model.subsets[k].vertices[i].normal)));
		}

		// Put the positions into the vertices for this subset
		for (int i = 0; i < MD5Model.subsets[k].vertices.size(); i++)
		{
			MD5Model.subsets[k].vertices[i].pos = MD5Model.subsets[k].positions[i];
		}

		// Update the subsets vertex buffer
		// First lock the buffer
		D3D11_MAPPED_SUBRESOURCE mappedVertBuff;
		con->Map(MD5Model.subsets[k].vertBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVertBuff);

		// Copy the data into the vertex buffer.
		memcpy(mappedVertBuff.pData, &MD5Model.subsets[k].vertices[0], (sizeof(Vertex) * MD5Model.subsets[k].vertices.size()));

		con->Unmap(MD5Model.subsets[k].vertBuff, 0);

		// The line below is another way to update a buffer. You will use this when you want to update a buffer less
		// than once per frame, since the GPU reads will be faster (the buffer was created as a DEFAULT buffer instead
		// of a DYNAMIC buffer), and the CPU writes will be slower. You can try both methods to find out which one is faster
		// for you. if you want to use the line below, you will have to create the buffer with D3D11_USAGE_DEFAULT instead
		// of D3D11_USAGE_DYNAMIC
		//d3d11DevCon->UpdateSubresource( MD5Model.subsets[k].vertBuff, 0, NULL, &MD5Model.subsets[k].vertices[0], 0, 0 );
	}
}

void GameObject::UpdateMatrix()
{
	if (mType == CUBE)
	{
		Scale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
		smilesWorld = Scale * World;
	}
	if (mType == MD5)
	{
		Scale = XMMatrixScaling(0.04f, 0.04f, 0.04f);			// The model is a bit too large for our scene, so make it smaller	
		smilesWorld = Scale * World;
	}
	if (mType == OBJ)
	{
		Scale = XMMatrixScaling(0.04f, 0.04f, 0.04f);
		smilesWorld = Scale * World;
	}

}

void GameObject::DrawMd5()
{
	//Set Vertex and Pixel Shaders
	con->VSSetShader(VS, 0, 0);
	con->PSSetShader(PS, 0, 0);
	//Set the Input Layout
	con->IASetInputLayout(vertLayout);

	//Set Primitive Topology
	con->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	constbuffPerFrame.light = light;
	con->UpdateSubresource(cbPerFrameBuffer, 0, NULL, &constbuffPerFrame, 0, 0);
	con->PSSetConstantBuffers(0, 1, &cbPerFrameBuffer);

	//Set Vertex and Pixel Shaders
	con->VSSetShader(VS, 0, 0);
	con->PSSetShader(PS, 0, 0);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	///***Draw MD5 Model***///
	for (int i = 0; i < NewMD5Model.numSubsets; i++)
	{
		//Set the grounds index buffer
		con->IASetIndexBuffer(NewMD5Model.subsets[i].indexBuff, DXGI_FORMAT_R32_UINT, 0);
		//Set the grounds vertex buffer
		con->IASetVertexBuffers(0, 1, &NewMD5Model.subsets[i].vertBuff, &stride, &offset);

		//Set the WVP matrix and send it to the constant buffer in effect file
		//WVP = 
		cbPerObj.WVP = XMMatrixTranspose(smilesWorld * camView * camProjection);
		cbPerObj.World = XMMatrixTranspose(smilesWorld);
		cbPerObj.hasTexture = true;		// We'll assume all md5 subsets have textures
		cbPerObj.hasNormMap = false;	// We'll also assume md5 models have no normal map (easy to change later though)
		con->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
		con->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
		con->PSSetConstantBuffers(1, 1, &cbPerObjectBuffer);
		con->PSSetShaderResources(0, 1, &meshSRV[NewMD5Model.subsets[i].texArrayIndex]);

		con->DrawIndexed(NewMD5Model.subsets[i].indices.size(), 0, 0);
	}
}

void GameObject::DrawObj()
{	//Set Vertex and Pixel Shaders
	con->VSSetShader(VS, 0, 0);
	con->PSSetShader(PS, 0, 0);
	con->IASetInputLayout(vertLayout);

	//Set Primitive Topology
	con->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	constbuffPerFrame.light = light;
	con->UpdateSubresource(cbPerFrameBuffer, 0, NULL, &constbuffPerFrame, 0, 0);
	con->PSSetConstantBuffers(0, 1, &cbPerFrameBuffer);
	
	
	con->OMSetBlendState(0, 0, 0xffffffff);
	//Set Vertex and Pixel Shaders
	con->VSSetShader(VS, 0, 0);
	con->PSSetShader(PS, 0, 0);


	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	/////Draw our model's NON-transparent subsets/////
	for (int i = 0; i < meshSubsets; ++i)
	{
		//Set the grounds index buffer
		con->IASetIndexBuffer(meshIndexBuff, DXGI_FORMAT_R32_UINT, 0);
		//Set the grounds vertex buffer
		con->IASetVertexBuffers(0, 1, &meshVertBuff, &stride, &offset);

		//Set the WVP matrix and send it to the constant buffer in effect file
		/*WVP = meshWorld * camView * camProjection;*/
		cbPerObj.WVP = XMMatrixTranspose(smilesWorld * camView * camProjection);
		cbPerObj.World = XMMatrixTranspose(meshWorld);
		cbPerObj.difColor = material[meshSubsetTexture[i]].difColor;
		cbPerObj.hasTexture = material[meshSubsetTexture[i]].hasTexture;
		cbPerObj.hasNormMap = material[meshSubsetTexture[i]].hasNormMap;
		con->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
		con->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
		con->PSSetConstantBuffers(1, 1, &cbPerObjectBuffer);
		if (material[meshSubsetTexture[i]].hasTexture)
			con->PSSetShaderResources(0, 1, &meshSRV[material[meshSubsetTexture[i]].texArrayIndex]);
		if (material[meshSubsetTexture[i]].hasNormMap)
			con->PSSetShaderResources(1, 1, &meshSRV[material[meshSubsetTexture[i]].normMapTexArrayIndex]);
		//con->PSSetSamplers(0, 1, &CubesTexSamplerState);

		con->RSSetState(RSCullNone);
		int indexStart = meshSubsetIndexStart[i];
		int indexDrawAmount = meshSubsetIndexStart[i + 1] - meshSubsetIndexStart[i];
		if (!material[meshSubsetTexture[i]].transparent)
			con->DrawIndexed(indexDrawAmount, indexStart, 0);
	}

}

void GameObject::Draw()
{
	UpdateMatrix();
	if (mType == CUBE)
	{
		DrawCube();
	}
	if (mType == MD5)
	{
		DrawMd5();
	}
	if (mType == OBJ)
	{
		DrawObj();
	}
}
