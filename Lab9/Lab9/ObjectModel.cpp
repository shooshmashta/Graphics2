#include "ObjectModel.h"

ObjectModel::~ObjectModel()
{

	
		SAFE_RELEASE(textureResource);
		SAFE_RELEASE(ObjTexture);
		SAFE_RELEASE(ObjTextureSamplerState);

		if (hasTrans)
		{
		
			SAFE_RELEASE(Transparency);
			//blenders
			SAFE_RELEASE(CCWcullMode);
			SAFE_RELEASE(CWcullMode);
		
		}

	if (!hasLight)
	{
		SAFE_RELEASE(matrixLocationBuffer[0]);
		
	}

	SAFE_RELEASE(matrixLocationBuffer[1]);
	SAFE_RELEASE(VertBuff);             // Models vertex buffer
	SAFE_RELEASE(IndexBuff);            // Models index buffer
	SAFE_RELEASE(layout);
	SAFE_RELEASE(pixelShader);
	SAFE_RELEASE(vertexShader);

	



}

bool ObjectModel::loadOBJ(
	const char * path)
{
	std::vector< XMFLOAT3 > temp_vertices;
	std::vector< XMFLOAT2 > temp_uvs;
	std::vector< XMFLOAT3 > temp_normals;

	FILE * file = fopen(path, "r");
	if (file == NULL)
	{
		printf("Impossible to open the file !\n");
		return false;
	}

	while (true)
	{
		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader
		if (strcmp(lineHeader, "v") == 0)
		{
			XMFLOAT3 vertex;
			fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0)
		{
			XMFLOAT2 uv;
			fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
			XMFLOAT3 normal;
			fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
				&vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1],
				&uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2],
				&normalIndex[2]);
			if (matches != 9)
			{
				printf("File can't be read by our simple parser : ( Try exporting with other options\n");
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
		else
		{
			// kill line
			char commentBuffer[1000];
			fgets(commentBuffer, 1000, file);
		}
	}

	Simple_Vert v;
	v.m_color = XMFLOAT4(1, 1, 1, 0.5f);

	for (unsigned int i = 0; i < vertexIndices.size(); i++)
	{
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normIndex = vertexIndices[i];

		v.m_vect = XMFLOAT4(temp_vertices[vertexIndex - 1].x, temp_vertices[vertexIndex - 1].y, temp_vertices[vertexIndex - 1].z, 1.0f);
		XMFLOAT2 uv = temp_uvs[uvIndex - 1];
		XMFLOAT3 norm = temp_normals[normIndex - 1];

		StrideStruct _ss;
		_ss.m_vect = v.m_vect;
		uv.y = 1-uv.y;
		_ss.v_uvs = uv;
		
		_ss.v_normals = norm;
		m_stride.push_back(_ss);

		v_vertices.push_back(v);

		vertexIndices[i] = i;
	}

	return true;
}

bool ObjectModel::Init(XMFLOAT3 pos,
	const wchar_t* path,
	ID3D11Device* dev,
	ID3D11DeviceContext* devCon,
	ProjViewMatricies* _viewproj,
	bool _texture, bool _light)
{
	ProjView = _viewproj;
	hasTexture = _texture;
	hasLight = _light;

	world.r[3] = XMVectorSet(pos.x, pos.y, pos.z, 1.0f);

	ProjView->world = world;

	HRESULT tester = 0;
#pragma region Obj Subresource

	if (hasTexture || hasLight)
	{
		D3D11_BUFFER_DESC assigner;
		assigner.Usage = D3D11_USAGE_IMMUTABLE;
		assigner.CPUAccessFlags = NULL;
		assigner.MiscFlags = NULL;
		assigner.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		assigner.ByteWidth = sizeof(StrideStruct) * m_stride.size();
		assigner.StructureByteStride = sizeof(StrideStruct);

		D3D11_SUBRESOURCE_DATA obj_verts;
		obj_verts.pSysMem = &m_stride[0];
		obj_verts.SysMemPitch = 0;
		obj_verts.SysMemSlicePitch = 0;

		tester = dev->CreateBuffer(&assigner, &obj_verts, &VertBuff);

	}
	else
	{
		D3D11_BUFFER_DESC assigner;
		D3D11_SUBRESOURCE_DATA obj_verts;
		assigner.Usage = D3D11_USAGE_IMMUTABLE;
		assigner.CPUAccessFlags = NULL;
		assigner.MiscFlags = NULL;
		assigner.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		assigner.ByteWidth = sizeof(Simple_Vert) * vertexIndices.size();
		assigner.StructureByteStride = sizeof(Simple_Vert);

		obj_verts.pSysMem = &v_vertices[0];
		obj_verts.SysMemPitch = 0;
		obj_verts.SysMemSlicePitch = 0;

		tester = dev->CreateBuffer(&assigner, &obj_verts, &VertBuff);
	}
#pragma endregion



#pragma region Obj Shader

		tester = dev->CreatePixelShader(Trivial_PS,
			sizeof(Trivial_PS), NULL, &pixelShader);
		tester = dev->CreateVertexShader(Trivial_VS,
			sizeof(Trivial_VS), NULL, &vertexShader);

		D3D11_INPUT_ELEMENT_DESC vLayout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};


		tester = dev->CreateInputLayout(vLayout, 2, Trivial_VS,
			sizeof(Trivial_VS), &layout);
	

	
#pragma endregion


#pragma region ObjTextures
	if (hasTexture || hasLight)
	{

		tester = CreateDDSTextureFromFile(dev, path, &textureResource, &ObjTexture);

		if (tester)
		{
			tester;
		}

		D3D11_SAMPLER_DESC sampleDec;
		sampleDec.Filter = D3D11_FILTER_ANISOTROPIC;
		sampleDec.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampleDec.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampleDec.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampleDec.MinLOD = (-FLT_MAX);
		sampleDec.MaxLOD = (FLT_MAX);
		sampleDec.MipLODBias = 0.0f;
		sampleDec.MaxAnisotropy = 1;
		sampleDec.ComparisonFunc = D3D11_COMPARISON_NEVER;

		tester = dev->CreateSamplerState(&sampleDec, &ObjTextureSamplerState);

		devCon->PSSetSamplers(0, 1, &ObjTextureSamplerState);
	}

#pragma endregion

#pragma region Obj IndexBuffer
	//index buffer settings
	D3D11_BUFFER_DESC indexBuffer;
	indexBuffer.Usage = D3D11_USAGE_IMMUTABLE;
	indexBuffer.MiscFlags = NULL;
	indexBuffer.CPUAccessFlags = NULL;
	indexBuffer.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBuffer.ByteWidth = sizeof(unsigned int) * vertexIndices.size();
	indexBuffer.StructureByteStride = sizeof(unsigned int);

	D3D11_SUBRESOURCE_DATA indexSub;
	indexSub.pSysMem = &vertexIndices[0];
	indexSub.SysMemPitch = 0;
	indexSub.SysMemSlicePitch = 0;

	tester = dev->CreateBuffer(&indexBuffer, &indexSub, &IndexBuff);


#pragma endregion

#pragma region Locations / constantBuffers
	if (!hasLight)
	{
		D3D11_BUFFER_DESC locationBuffer;
		locationBuffer.ByteWidth = sizeof(XMMATRIX);
		locationBuffer.Usage = D3D11_USAGE_DYNAMIC;
		locationBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		locationBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		locationBuffer.MiscFlags = NULL;
		tester = dev->CreateBuffer(&locationBuffer, NULL, &matrixLocationBuffer[0]);

	}


	D3D11_BUFFER_DESC othermatrix;
	othermatrix.ByteWidth = sizeof(ProjViewMatricies);
	othermatrix.Usage = D3D11_USAGE_DYNAMIC;
	othermatrix.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	othermatrix.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	othermatrix.MiscFlags = NULL;

	tester = dev->CreateBuffer(&othermatrix, NULL, &matrixLocationBuffer[1]);

	


#pragma endregion
	return true;
}

bool ObjectModel::Run(
	ID3D11Device* dev,
	ID3D11DeviceContext* devCon)
{
	HRESULT tester = 0;
#pragma region place on map

	ProjView->world = world;// *XMMatrixRotationX(180);


	ProjView->view = XMMatrixInverse(nullptr, ProjView->view);

	if (!hasLight)
	{
		D3D11_MAPPED_SUBRESOURCE mapRes;
		ZeroMemory(&mapRes, sizeof(D3D11_MAPPED_SUBRESOURCE));
		tester = devCon->Map(matrixLocationBuffer[0], NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapRes);
		memcpy(mapRes.pData, &world, sizeof(XMMATRIX));

		devCon->Unmap(matrixLocationBuffer[0], NULL);

		ZeroMemory(&mapRes, sizeof(D3D11_MAPPED_SUBRESOURCE));
		tester = devCon->Map(matrixLocationBuffer[1], NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapRes);
		memcpy(mapRes.pData, ProjView, sizeof(ProjViewMatricies));
		devCon->Unmap(matrixLocationBuffer[1], NULL);

		devCon->VSSetConstantBuffers(0, 2, matrixLocationBuffer);
	}
	else if (hasLight)
	{
		D3D11_MAPPED_SUBRESOURCE mapRes;
		ZeroMemory(&mapRes, sizeof(D3D11_MAPPED_SUBRESOURCE));
		tester = devCon->Map(matrixLocationBuffer[1], NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapRes);
		memcpy(mapRes.pData, ProjView, sizeof(ProjViewMatricies));
		devCon->Unmap(matrixLocationBuffer[1], NULL);


		devCon->VSSetConstantBuffers(0, 1, &matrixLocationBuffer[1]);

	}

	ProjView->view = XMMatrixInverse(nullptr, ProjView->view);

	ProjView->world = XMMatrixIdentity();
#pragma endregion



#pragma region VS and PS
	UINT strides;
	UINT offsets;
	if (!hasTexture && !hasLight)
	{
		strides = sizeof(Simple_Vert);
		offsets = 0;
	}
	else
	{
		strides = sizeof(StrideStruct);
		offsets = 0;

		devCon->PSSetShaderResources(0, 1, &ObjTexture);
	}


	devCon->IASetVertexBuffers(0, 1, &VertBuff, &strides, &offsets);

	devCon->IASetIndexBuffer(IndexBuff, DXGI_FORMAT_R32_UINT, offsets);

	devCon->VSSetShader(vertexShader, 0, 0);
	devCon->PSSetShader(pixelShader, 0, 0);

	devCon->IASetInputLayout(layout);

	devCon->DrawIndexed(vertexIndices.size(), 0, 0);


#pragma endregion


	return true;
}

bool ObjectModel::SkyInit(const wchar_t* path,
	ID3D11Device * dev,
	ID3D11DeviceContext *devCon,
	ProjViewMatricies* _viewproj)
{
	ProjView = _viewproj;

	ProjView->world.r[3].m128_f32[0] = ProjView->view.r[3].m128_f32[0];
	ProjView->world.r[3].m128_f32[1] = ProjView->view.r[3].m128_f32[1];
	ProjView->world.r[3].m128_f32[2] = ProjView->view.r[3].m128_f32[2];

	hasTexture = true;
	hasLight = true;

	HRESULT tester = 0;

	D3D11_BUFFER_DESC assigner;
	D3D11_SUBRESOURCE_DATA obj_verts;
	assigner.Usage = D3D11_USAGE_IMMUTABLE;
	assigner.CPUAccessFlags = NULL;
	assigner.MiscFlags = NULL;
	assigner.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	assigner.ByteWidth = sizeof(Simple_Vert) * vertexIndices.size();
	assigner.StructureByteStride = sizeof(Simple_Vert);

	obj_verts.pSysMem = &v_vertices[0];
	obj_verts.SysMemPitch = 0;
	obj_verts.SysMemSlicePitch = 0;

	tester = dev->CreateBuffer(&assigner, &obj_verts, &VertBuff);

	tester = dev->CreatePixelShader(Sky_PS,
		sizeof(Sky_PS), NULL, &pixelShader);
	tester = dev->CreateVertexShader(Sky_VS,
		sizeof(Sky_VS), NULL, &vertexShader);

	D3D11_INPUT_ELEMENT_DESC vLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UVS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	tester = dev->CreateInputLayout(vLayout, 2, Sky_VS,
		sizeof(Sky_VS), &layout);

	tester = CreateDDSTextureFromFile(dev, path, &textureResource, &ObjTexture);

	if (tester)
	{
		tester;
	}

	D3D11_SAMPLER_DESC sampleDec;
	sampleDec.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampleDec.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampleDec.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampleDec.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampleDec.MinLOD = (-FLT_MAX);
	sampleDec.MaxLOD = (FLT_MAX);
	sampleDec.MipLODBias = 0.0f;
	sampleDec.MaxAnisotropy = 1;
	sampleDec.ComparisonFunc = D3D11_COMPARISON_NEVER;

	tester = dev->CreateSamplerState(&sampleDec, &ObjTextureSamplerState);

	devCon->PSSetSamplers(0, 1, &ObjTextureSamplerState);

	D3D11_BUFFER_DESC indexBuffer;
	indexBuffer.Usage = D3D11_USAGE_IMMUTABLE;
	indexBuffer.MiscFlags = NULL;
	indexBuffer.CPUAccessFlags = NULL;
	indexBuffer.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBuffer.ByteWidth = sizeof(unsigned int) * vertexIndices.size();
	indexBuffer.StructureByteStride = sizeof(unsigned int);

	D3D11_SUBRESOURCE_DATA indexSub;
	indexSub.pSysMem = &vertexIndices[0];
	indexSub.SysMemPitch = 0;
	indexSub.SysMemSlicePitch = 0;

	tester = dev->CreateBuffer(&indexBuffer, &indexSub, &IndexBuff);


	D3D11_BUFFER_DESC othermatrix;
	othermatrix.ByteWidth = sizeof(ProjViewMatricies);
	othermatrix.Usage = D3D11_USAGE_DYNAMIC;
	othermatrix.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	othermatrix.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	othermatrix.MiscFlags = NULL;

	tester = dev->CreateBuffer(&othermatrix, NULL, &matrixLocationBuffer[1]);



	return true;
}

bool ObjectModel::SkyRun(
	ID3D11Device* dev,
	ID3D11DeviceContext* devCon, 
	ID3D11DepthStencilView * pDSV)
{

	UINT strides;
	UINT offsets;

	strides = sizeof(Simple_Vert);
	offsets = 0;

	HRESULT tester;
	ProjView->world = XMMatrixIdentity();
	ProjView->world.r[3] = ProjView->view.r[3];


	ProjView->view = XMMatrixInverse(nullptr, ProjView->view);
	D3D11_MAPPED_SUBRESOURCE mapRes;
	ZeroMemory(&mapRes, sizeof(D3D11_MAPPED_SUBRESOURCE));
	tester = devCon->Map(matrixLocationBuffer[1], NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapRes);
	memcpy(mapRes.pData, ProjView, sizeof(ProjViewMatricies));
	devCon->Unmap(matrixLocationBuffer[1], NULL);
	ProjView->view = XMMatrixInverse(nullptr, ProjView->view);

	devCon->VSSetConstantBuffers(0, 1, &matrixLocationBuffer[1]);
	devCon->PSSetShaderResources(0, 1, &ObjTexture);
	devCon->IASetVertexBuffers(0, 1, &VertBuff, &strides, &offsets);
	devCon->IASetIndexBuffer(IndexBuff, DXGI_FORMAT_R32_UINT, offsets);

	devCon->VSSetShader(vertexShader, 0, 0);
	devCon->PSSetShader(pixelShader, 0, 0);

	devCon->IASetInputLayout(layout);

	devCon->DrawIndexed(vertexIndices.size(), 0, 0);
	devCon->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1, 0);
	return true;
}



bool ObjectModel::LightsInit(
	XMFLOAT3 pos,
	const wchar_t* path,
	ID3D11Device * dev,
	ID3D11DeviceContext *devCon,
	ProjViewMatricies* _viewproj, bool _hasTrans)
{
	hasTrans = _hasTrans;
	ProjView = _viewproj;
	HRESULT tester = 0;
	//hasTexture = t;
	//hasLight = _light;
	hasLight = true;
	world.r[3] = XMVectorSet(pos.x, pos.y, pos.z, 1.0f);

	ProjView->world = world;

#pragma region Obj Subresource


	D3D11_BUFFER_DESC assigner;
	assigner.Usage = D3D11_USAGE_IMMUTABLE;
	assigner.CPUAccessFlags = NULL;
	assigner.MiscFlags = NULL;
	assigner.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	assigner.ByteWidth = sizeof(StrideStruct) * m_stride.size();
	assigner.StructureByteStride = sizeof(StrideStruct);

	D3D11_SUBRESOURCE_DATA obj_verts;
	obj_verts.pSysMem = &m_stride[0];
	obj_verts.SysMemPitch = 0;
	obj_verts.SysMemSlicePitch = 0;

	tester = dev->CreateBuffer(&assigner, &obj_verts, &VertBuff);



#pragma endregion



#pragma region Obj Shader
	tester = dev->CreatePixelShader(Combined_PS,
		sizeof(Combined_PS), NULL, &pixelShader);

	tester = dev->CreateVertexShader(Combined_VS,
		sizeof(Combined_VS), NULL, &vertexShader);

	D3D11_INPUT_ELEMENT_DESC vLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		{ "UVS", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		{ "NORMS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};


	tester = dev->CreateInputLayout(vLayout, 3, Combined_VS,
		sizeof(Combined_VS), &layout);


#pragma endregion


#pragma region ObjTextures


	tester = CreateDDSTextureFromFile(dev, path, &textureResource, &ObjTexture);

	if (tester)
	{
		tester;
	}

	D3D11_SAMPLER_DESC sampleDec;
	sampleDec.Filter = D3D11_FILTER_ANISOTROPIC;
	sampleDec.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDec.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDec.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDec.MinLOD = (-FLT_MAX);
	sampleDec.MaxLOD = (FLT_MAX);
	sampleDec.MipLODBias = 0.0f;
	sampleDec.MaxAnisotropy = 1;
	sampleDec.ComparisonFunc = D3D11_COMPARISON_NEVER;

	tester = dev->CreateSamplerState(&sampleDec, &ObjTextureSamplerState);

	devCon->PSSetSamplers(0, 1, &ObjTextureSamplerState);


#pragma endregion

#pragma region Obj IndexBuffer
	//index buffer settings
	D3D11_BUFFER_DESC indexBuffer;
	indexBuffer.Usage = D3D11_USAGE_IMMUTABLE;
	indexBuffer.MiscFlags = NULL;
	indexBuffer.CPUAccessFlags = NULL;
	indexBuffer.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBuffer.ByteWidth = sizeof(unsigned int) * vertexIndices.size();
	indexBuffer.StructureByteStride = sizeof(unsigned int);

	D3D11_SUBRESOURCE_DATA indexSub;
	indexSub.pSysMem = &vertexIndices[0];
	indexSub.SysMemPitch = 0;
	indexSub.SysMemSlicePitch = 0;

	tester = dev->CreateBuffer(&indexBuffer, &indexSub, &IndexBuff);


#pragma endregion

#pragma region Locations / constantBuffers

	D3D11_BUFFER_DESC othermatrix;
	othermatrix.ByteWidth = sizeof(ProjViewMatricies);
	othermatrix.Usage = D3D11_USAGE_DYNAMIC;
	othermatrix.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	othermatrix.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	othermatrix.MiscFlags = NULL;

	tester = dev->CreateBuffer(&othermatrix, NULL, &matrixLocationBuffer[1]);


#pragma endregion


	if (hasTrans)
	{
		//blenders
		D3D11_BLEND_DESC blendDesc;
		ZeroMemory(&blendDesc, sizeof(blendDesc));

		D3D11_RENDER_TARGET_BLEND_DESC rtbd;
		ZeroMemory(&rtbd, sizeof(rtbd));

		rtbd.BlendEnable = true;
		rtbd.SrcBlend = D3D11_BLEND_SRC_COLOR;
		rtbd.DestBlend = D3D11_BLEND_BLEND_FACTOR;
		rtbd.BlendOp = D3D11_BLEND_OP_ADD;
		rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
		rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
		rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

		blendDesc.AlphaToCoverageEnable = false;
		blendDesc.RenderTarget[0] = rtbd;


		dev->CreateBlendState(&blendDesc, &Transparency);


		D3D11_RASTERIZER_DESC cmdesc;
		ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));

		cmdesc.FillMode = D3D11_FILL_SOLID;
		cmdesc.CullMode = D3D11_CULL_BACK;
		cmdesc.DepthClipEnable = true;
		cmdesc.AntialiasedLineEnable = true;
		cmdesc.MultisampleEnable = true;

		cmdesc.FrontCounterClockwise = true;
		tester = dev->CreateRasterizerState(&cmdesc, &CCWcullMode);

		cmdesc.FrontCounterClockwise = false;
		tester = dev->CreateRasterizerState(&cmdesc, &CWcullMode);

	}

	return true;
}

bool ObjectModel::LightsRun(
	ID3D11Device* dev,
	ID3D11DeviceContext* devCon)
{
	HRESULT tester = 0;
#pragma region place on map

	ProjView->world = world;// *XMMatrixRotationX(180);


	ProjView->view = XMMatrixInverse(nullptr, ProjView->view);


	D3D11_MAPPED_SUBRESOURCE mapRes;


	ZeroMemory(&mapRes, sizeof(D3D11_MAPPED_SUBRESOURCE));
	tester = devCon->Map(matrixLocationBuffer[1], NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapRes);
	memcpy(mapRes.pData, ProjView, sizeof(ProjViewMatricies));
	devCon->Unmap(matrixLocationBuffer[1], NULL);

	devCon->VSSetConstantBuffers(0, 1, &matrixLocationBuffer[1]);

	ProjView->view = XMMatrixInverse(nullptr, ProjView->view);

	ProjView->world = XMMatrixIdentity();
#pragma endregion




#pragma region VS and PS
	UINT strides;
	UINT offsets;

	strides = sizeof(StrideStruct);
	offsets = 0;

	devCon->PSSetShaderResources(0, 1, &ObjTexture);
	devCon->IASetVertexBuffers(0, 1, &VertBuff, &strides, &offsets);
	devCon->IASetIndexBuffer(IndexBuff, DXGI_FORMAT_R32_UINT, offsets);

	devCon->VSSetShader(vertexShader, 0, 0);
	devCon->PSSetShader(pixelShader, 0, 0);

	devCon->IASetInputLayout(layout);

	if (!hasTrans)
	{
		devCon->OMSetBlendState(0, 0, 0xffffffff);
		devCon->DrawIndexed(vertexIndices.size(), 0, 0);

	}
	else
	{
		float blendFactor[4] = { 0.90f, 0.90f, 0.90f, 1.0f };
		devCon->OMSetBlendState(Transparency, blendFactor, 0xffffffff);
		devCon->RSSetState(CCWcullMode);

		devCon->DrawIndexed(vertexIndices.size(), 0, 0);
		
		devCon->PSSetShaderResources(0, 1, &ObjTexture);
		devCon->IASetVertexBuffers(0, 1, &VertBuff, &strides, &offsets);
		devCon->IASetIndexBuffer(IndexBuff, DXGI_FORMAT_R32_UINT, offsets);

		devCon->VSSetShader(vertexShader, 0, 0);
		devCon->PSSetShader(pixelShader, 0, 0);

		devCon->IASetInputLayout(layout);
		devCon->RSSetState(CWcullMode);
		devCon->DrawIndexed(vertexIndices.size(), 0, 0);

		devCon->OMSetBlendState(0, 0, 0xffffffff);
	}
	




#pragma endregion
	return true;
}










bool ObjectModel::RunNewPos(XMFLOAT3 pos,
	ID3D11Device* dev,
	ID3D11DeviceContext* devCon)
{
	XMMATRIX _world;
	_world = world + XMMatrixTranslation(pos.x, pos.y, pos.z);

	HRESULT tester = 0;
#pragma region place on map
	ProjView->view = XMMatrixInverse(nullptr, ProjView->view);

	if (!hasLight)
	{
		D3D11_MAPPED_SUBRESOURCE mapRes;
		ZeroMemory(&mapRes, sizeof(D3D11_MAPPED_SUBRESOURCE));
		tester = devCon->Map(matrixLocationBuffer[0], NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapRes);
		memcpy(mapRes.pData, &world, sizeof(XMMATRIX));

		devCon->Unmap(matrixLocationBuffer[0], NULL);

		ZeroMemory(&mapRes, sizeof(D3D11_MAPPED_SUBRESOURCE));
		tester = devCon->Map(matrixLocationBuffer[1], NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapRes);
		memcpy(mapRes.pData, &ProjView, sizeof(ProjViewMatricies));
		devCon->Unmap(matrixLocationBuffer[1], NULL);

		devCon->VSSetConstantBuffers(0, 2, matrixLocationBuffer);
	}
	else if (hasLight)
	{
		D3D11_MAPPED_SUBRESOURCE mapRes;
		ZeroMemory(&mapRes, sizeof(D3D11_MAPPED_SUBRESOURCE));
		tester = devCon->Map(matrixLocationBuffer[1], NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapRes);
		memcpy(mapRes.pData, &ProjView, sizeof(ProjViewMatricies));
		devCon->Unmap(matrixLocationBuffer[1], NULL);

		devCon->VSSetConstantBuffers(0, 1, &matrixLocationBuffer[1]);

	}

	ProjView->view = XMMatrixInverse(nullptr, ProjView->view);
#pragma endregion



#pragma region VS and PS
	UINT strides;
	UINT offsets;
	if (!hasTexture && !hasLight)
	{
		strides = sizeof(Simple_Vert);
		offsets = 0;
	}
	else
	{
		strides = sizeof(StrideStruct);
		offsets = 0;
		devCon->PSSetShaderResources(0, 1, &ObjTexture);
	}

	ProjView->world = world;
	devCon->IASetVertexBuffers(0, 1, &VertBuff, &strides, &offsets);
	devCon->IASetIndexBuffer(IndexBuff, DXGI_FORMAT_R32_UINT, offsets);


	// TODO: PART 2 STEP 9b
	devCon->VSSetShader(vertexShader, 0, 0);
	devCon->PSSetShader(pixelShader, 0, 0);

	// TODO: PART 2 STEP 9c
	devCon->IASetInputLayout(layout);

	// TODO: PART 2 STEP 9d
	//devCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// TODO: PART 2 STEP 10
	devCon->DrawIndexed(vertexIndices.size(), 0, 0);


#pragma endregion


	return true;
}


void ObjectModel::Cleanup()
{
	/*delete[] p_vertInd;
	delete[] p_verts;
	delete[] p_norms;
	delete[] p_uvs	;*/

	SAFE_RELEASE(VertBuff);             // Models vertex buffer
	SAFE_RELEASE(IndexBuff);            // Models index buffer
	SAFE_RELEASE(matrixLocationBuffer[0]);
	SAFE_RELEASE(matrixLocationBuffer[1]);
	SAFE_RELEASE(layout);
	SAFE_RELEASE(pixelShader);
	SAFE_RELEASE(vertexShader);
}