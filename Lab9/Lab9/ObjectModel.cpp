#include "ObjectModel.h"

ObjectModel::~ObjectModel()
{
	if (hasTexture)
	{
		SAFE_RELEASE(textureResource[0]);
		SAFE_RELEASE(ObjTexture[0]);
		if (!isSky)
		{
			SAFE_RELEASE(textureResource[1]);
			SAFE_RELEASE(ObjTexture[1]);
		}
		SAFE_RELEASE(ObjTextureSamplerState);
	}
	if (hasInstanced)
	{
		SAFE_RELEASE(InstanceBuff);
	}
	if (hasTrans)
	{
		//blenders
		SAFE_RELEASE(Transparency);
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
	vector< XMFLOAT3 > temp_vertices;
	vector< XMFLOAT2 > temp_uvs;
	vector< XMFLOAT3 > temp_normals;

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
			string vertex1, vertex2, vertex3;
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
		uv.y = 1 - uv.y;
		_ss.v_uvs = uv;

		_ss.v_normals = norm;
		m_stride.push_back(_ss);

		v_vertices.push_back(v);

		vertexIndices[i] = i;
	}
	ComputeTangents();
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
	isSky = true;
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

	tester = CreateDDSTextureFromFile(dev, path, &textureResource[0], &ObjTexture[0]);

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
	devCon->PSSetShaderResources(0, 1, &ObjTexture[0]);
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
	const wchar_t* norm,
	ID3D11Device * dev,
	ID3D11DeviceContext *devCon,
	ProjViewMatricies* _viewproj, bool _hasTrans)
{
	//ComputeTangents();
	hasTrans = _hasTrans;
	ProjView = _viewproj;
	HRESULT tester = 0;
	//hasTexture = t;
	//hasLight = _light;
	hasLight = true;
	hasTexture = true;
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

		{ "TANG", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		/*
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },*/
	};


	tester = dev->CreateInputLayout(vLayout, 4, Combined_VS,
		sizeof(Combined_VS), &layout);


#pragma endregion


#pragma region ObjTextures


	tester = CreateDDSTextureFromFile(dev, path, &textureResource[0], &ObjTexture[0]);
	tester = CreateDDSTextureFromFile(dev, norm, &textureResource[1], &ObjTexture[1]);


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

	devCon->PSSetShaderResources(0, 2, ObjTexture);
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

		devCon->PSSetShaderResources(0, 2, ObjTexture);
		devCon->IASetVertexBuffers(0, 1, &VertBuff, &strides, &offsets);
		devCon->IASetIndexBuffer(IndexBuff, DXGI_FORMAT_R32_UINT, offsets);

		devCon->VSSetShader(vertexShader, 0, 0);
		devCon->PSSetShader(pixelShader, 0, 0);

		devCon->IASetInputLayout(layout);
		devCon->RSSetState(CWcullMode);
		devCon->DrawIndexed(vertexIndices.size(), 0, 0);

		devCon->OMSetBlendState(0, 0, 0xffffffff);
	}
	ID3D11ShaderResourceView * nullShader = nullptr;
	devCon->PSSetShaderResources(0, 1, &nullShader);

#pragma endregion

	return true;
}





bool ObjectModel::InstancedInit(XMFLOAT3 pos,
	const wchar_t* path,
	const wchar_t* norm,
	ID3D11Device * dev,
	ID3D11DeviceContext *devCon,
	ProjViewMatricies* _viewproj)
{
	//ComputeTangents();

	ProjView = _viewproj;
	HRESULT tester = 0;

	hasLight = true;
	hasTexture = true;
	hasInstanced = true;
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
	tester = dev->CreatePixelShader(Trivial_PS,
		sizeof(Trivial_PS), NULL, &pixelShader);

	tester = dev->CreateVertexShader(Trivial_VS,
		sizeof(Trivial_VS), NULL, &vertexShader);

	D3D11_INPUT_ELEMENT_DESC vLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		{ "UVS", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		{ "NORMS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		{ "TANG", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		
		{ "OTHERPOS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};


	tester = dev->CreateInputLayout(vLayout, 5, Trivial_VS,
		sizeof(Trivial_VS), &layout);


#pragma endregion


#pragma region ObjTextures


	tester = CreateDDSTextureFromFile(dev, path, &textureResource[0], &ObjTexture[0]);
	tester = CreateDDSTextureFromFile(dev, norm, &textureResource[1], &ObjTexture[1]);


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
	devCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

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


#pragma region instanced stuff

	count = 4;

	instances.m_newPos[0] = XMFLOAT4(-1.5f, -1.5f, 5.0f, 1.0f);
	instances.m_newPos[1] = XMFLOAT4(-1.5f, 1.5f, 5.0f, 1.0f);
	instances.m_newPos[2] = XMFLOAT4(1.5f, -1.5f, 5.0f, 1.0f);
	instances.m_newPos[3] = XMFLOAT4(1.5f, 1.5f, 5.0f, 1.0f);

	D3D11_BUFFER_DESC instanceBufferDesc;

	instanceBufferDesc.ByteWidth = sizeof(InstancedObj) * count;
	instanceBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = 0;
	instanceBufferDesc.MiscFlags = 0;
	//instanceBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA instanceData;

	instanceData.pSysMem = &instances;
	instanceData.SysMemPitch = 0;
	instanceData.SysMemSlicePitch = 0;

	tester = dev->CreateBuffer(&instanceBufferDesc, &instanceData, &InstanceBuff);

#pragma endregion




	return true;
}





bool ObjectModel::InstancedRun(
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
	UINT strides[2];
	UINT offsets[2];
	ID3D11Buffer* bufferPointers[2];

	strides[0] = sizeof(StrideStruct);
	strides[1] = sizeof(InstancedObj);
	offsets[0] = 0;
	offsets[1] = 0;
	bufferPointers[0] = VertBuff;
	bufferPointers[1] = InstanceBuff;

	devCon->PSSetSamplers(0, 1, &ObjTextureSamplerState);
	devCon->PSSetShaderResources(0, 2, ObjTexture);

	devCon->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);

	devCon->IASetIndexBuffer(IndexBuff, DXGI_FORMAT_R32_UINT, offsets[0]);

	devCon->VSSetShader(vertexShader, 0, 0);
	devCon->PSSetShader(pixelShader, 0, 0);

	devCon->IASetInputLayout(layout);


	devCon->OMSetBlendState(0, 0, 0xffffffff);
	devCon->DrawIndexedInstanced(vertexIndices.size(), count, 0 ,0, 0);


	ID3D11ShaderResourceView * nullShader = nullptr;
	//devCon->PSSetShaderResources(0, 1, &nullShader);

#pragma endregion

	return true;
}



























bool ObjectModel::FlootInit(
	XMFLOAT3 pos,
	const wchar_t* path,
	ID3D11Device * dev,
	ID3D11DeviceContext *devCon,
	ProjViewMatricies* _viewproj)
{
	HRESULT tester = 0;

	hasTrans = false;
	hasLight = true;

	ProjView = _viewproj;
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



	return true;
}







bool ObjectModel::FloorRun(
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

	ID3D11ShaderResourceView *nullshader = NULL;


#pragma region VS and PS
	UINT strides;
	UINT offsets;

	strides = sizeof(StrideStruct);
	offsets = 0;

	devCon->IASetVertexBuffers(0, 1, &VertBuff, &strides, &offsets);
	devCon->IASetIndexBuffer(IndexBuff, DXGI_FORMAT_R32_UINT, offsets);

	devCon->PSSetShaderResources(0, 1, &ObjTexture[0]);
	devCon->VSSetShader(vertexShader, 0, 0);
	devCon->PSSetShader(pixelShader, 0, 0);

	devCon->IASetInputLayout(layout);

	devCon->OMSetBlendState(0, 0, 0xffffffff);
	devCon->DrawIndexed(vertexIndices.size(), 0, 0);

	devCon->PSSetShaderResources(0, 1, &nullshader);

#pragma endregion
	return true;
}

bool ObjectModel::Init(XMFLOAT3 pos,
	ID3D11Device* dev,
	ID3D11DeviceContext* devCon,
	ProjViewMatricies* _viewproj)
{

	hasTexture = false;
	hasLight = false;
	hasTrans = false;

	world.r[3] = XMVectorSet(pos.x, 0, pos.z, 1.0f);

	ProjView = _viewproj;


#pragma region Obj Subresource
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

#pragma endregion



#pragma region Obj Shader
	// TODO: PART 2 STEP 7

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
	D3D11_BUFFER_DESC locationBuffer;
	locationBuffer.ByteWidth = sizeof(XMMATRIX);
	locationBuffer.Usage = D3D11_USAGE_DYNAMIC;
	locationBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	locationBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	locationBuffer.MiscFlags = NULL;


	tester = dev->CreateBuffer(&locationBuffer, NULL, &matrixLocationBuffer[0]);

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
	ProjView->view = XMMatrixInverse(nullptr, ProjView->view);

	D3D11_MAPPED_SUBRESOURCE mapRes;
	ZeroMemory(&mapRes, sizeof(D3D11_MAPPED_SUBRESOURCE));
	tester = devCon->Map(matrixLocationBuffer[0], NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapRes);
	memcpy(mapRes.pData, &world, sizeof(XMMATRIX));

	devCon->Unmap(matrixLocationBuffer[0], NULL);

	ZeroMemory(&mapRes, sizeof(D3D11_MAPPED_SUBRESOURCE));
	tester = devCon->Map(matrixLocationBuffer[1], NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapRes);
	memcpy(mapRes.pData, ProjView, sizeof(ProjViewMatricies));
	devCon->Unmap(matrixLocationBuffer[1], NULL);
	ProjView->view = XMMatrixInverse(nullptr, ProjView->view);

	devCon->VSSetConstantBuffers(0, 2, matrixLocationBuffer);
#pragma endregion



#pragma region VS and PS
	UINT strides;
	UINT offsets;

	strides = sizeof(Simple_Vert);
	offsets = 0;


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

void ObjectModel::ComputeTangents()
{
	float tcU1, tcV1, tcU2, tcV2;
	float vecX, vecY, vecZ;
	XMVECTOR edge1 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR edge2 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMFLOAT3 tangent = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 unnormalized = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vector<XMFLOAT3> tempTangent;
	vector<XMFLOAT3> tempNormal;


	for (unsigned int i = 0; i < m_stride.size() / 3; i++)
	{

		//Get the vector describing one edge of our triangle (edge 0,2)
		vecX = v_vertices[vertexIndices[(i * 3)]].m_vect.x - v_vertices[vertexIndices[(i * 3) + 2]].m_vect.x;
		vecY = v_vertices[vertexIndices[(i * 3)]].m_vect.y - v_vertices[vertexIndices[(i * 3) + 2]].m_vect.y;
		vecZ = v_vertices[vertexIndices[(i * 3)]].m_vect.z - v_vertices[vertexIndices[(i * 3) + 2]].m_vect.z;
		edge1 = XMVectorSet(vecX, vecY, vecZ, 0.0f);	//Create our first edge

		//Get the vector describing another edge of our triangle (edge 2,1)
		vecX = v_vertices[vertexIndices[(i * 3) + 2]].m_vect.x - v_vertices[vertexIndices[(i * 3) + 1]].m_vect.x;
		vecY = v_vertices[vertexIndices[(i * 3) + 2]].m_vect.y - v_vertices[vertexIndices[(i * 3) + 1]].m_vect.y;
		vecZ = v_vertices[vertexIndices[(i * 3) + 2]].m_vect.z - v_vertices[vertexIndices[(i * 3) + 1]].m_vect.z;
		edge2 = XMVectorSet(vecX, vecY, vecZ, 0.0f);	//Create our second edge

		//Cross multiply the two edge vectors to get the un-normalized face normal
		XMStoreFloat3(&unnormalized, XMVector3Cross(edge1, edge2));

		tempNormal.push_back(unnormalized);

		//Find first texture coordinate edge 2d vector
		tcU1 = m_stride[vertexIndices[(i * 3)]].v_uvs.x - m_stride[vertexIndices[(i * 3) + 2]].v_uvs.x;
		tcV1 = m_stride[vertexIndices[(i * 3)]].v_uvs.y - m_stride[vertexIndices[(i * 3) + 2]].v_uvs.y;

		//Find second texture coordinate edge 2d vector
		tcU2 = m_stride[vertexIndices[(i * 3) + 2]].v_uvs.x - m_stride[vertexIndices[(i * 3) + 1]].v_uvs.x;
		tcV2 = m_stride[vertexIndices[(i * 3) + 2]].v_uvs.y - m_stride[vertexIndices[(i * 3) + 1]].v_uvs.y;

		//Find tangent using both tex coord edges and position edges
		tangent.x = (tcV1 * XMVectorGetX(edge1) - tcV2 * XMVectorGetX(edge2)) * (1.0f / (tcU1 * tcV2 - tcU2 * tcV1));
		tangent.y = (tcV1 * XMVectorGetY(edge1) - tcV2 * XMVectorGetY(edge2)) * (1.0f / (tcU1 * tcV2 - tcU2 * tcV1));
		tangent.z = (tcV1 * XMVectorGetZ(edge1) - tcV2 * XMVectorGetZ(edge2)) * (1.0f / (tcU1 * tcV2 - tcU2 * tcV1));

		tempTangent.push_back(tangent);

	}


	XMVECTOR normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR tangentSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	int facesUsing = 0;
	float tX, tY, tZ;	//temp axis variables

	//Go through each vertex
	for (unsigned int i = 0; i < m_stride.size(); ++i)
	{
		//Check which triangles use this vertex
		for (unsigned int j = 0; j < m_stride.size() / 3; ++j)
		{
			if (vertexIndices[j * 3] == i ||
				vertexIndices[(j * 3) + 1] == i ||
				vertexIndices[(j * 3) + 2] == i)
			{
				tX = XMVectorGetX(normalSum) + tempNormal[j].x;
				tY = XMVectorGetY(normalSum) + tempNormal[j].y;
				tZ = XMVectorGetZ(normalSum) + tempNormal[j].z;

				normalSum = XMVectorSet(tX, tY, tZ, 0.0f);	//If a face is using the vertex, add the unormalized face normal to the normalSum

				///////////////**************new**************////////////////////		
				//We can reuse tX, tY, tZ to sum up tangents
				tX = XMVectorGetX(tangentSum) + tempTangent[j].x;
				tY = XMVectorGetY(tangentSum) + tempTangent[j].y;
				tZ = XMVectorGetZ(tangentSum) + tempTangent[j].z;

				tangentSum = XMVectorSet(tX, tY, tZ, 0.0f); //sum up face tangents using this vertex
				///////////////**************new**************////////////////////

				facesUsing++;
			}
		}

		//Get the actual normal by dividing the normalSum by the number of faces sharing the vertex
		normalSum = normalSum / (float)facesUsing;
		///////////////**************new**************////////////////////
		tangentSum = tangentSum / (float)facesUsing;
		///////////////**************new**************////////////////////

		//Normalize the normalSum vector and tangent
		normalSum = XMVector3Normalize(normalSum);
		///////////////**************new**************////////////////////
		tangentSum = XMVector3Normalize(tangentSum);
		///////////////**************new**************////////////////////

		//Store the normal and tangent in our current vertex
		m_stride[i].v_normals.x = -XMVectorGetX(normalSum);
		m_stride[i].v_normals.y = -XMVectorGetY(normalSum);
		m_stride[i].v_normals.z = -XMVectorGetZ(normalSum);

		///////////////**************new**************////////////////////
		m_stride[i].v_tangents.x = -XMVectorGetX(tangentSum);
		m_stride[i].v_tangents.y = -XMVectorGetY(tangentSum);
		m_stride[i].v_tangents.z = -XMVectorGetZ(tangentSum);
		///////////////**************new**************////////////////////

		//Clear normalSum, tangentSum and facesUsing for next vertex
		normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		///////////////**************new**************////////////////////
		tangentSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		///////////////**************new**************////////////////////
		facesUsing = 0;

	}
}