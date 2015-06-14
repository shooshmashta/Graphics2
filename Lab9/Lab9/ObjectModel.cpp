#include "ObjectModel.h"

ObjectModel::ObjectModel()
{}

bool ObjectModel::loadOBJ(
	const char * path	)
{
	
	std::vector< XMFLOAT3 > temp_vertices;
	std::vector< XMFLOAT2 > temp_uvs;
	std::vector< XMFLOAT3 > temp_normals;

	FILE * file = fopen( path, "r");
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
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}
	}


	for (unsigned int i = 0; i < vertexIndices.size(); i++)
	{
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normIndex = vertexIndices[i];
		
		XMFLOAT3 vertex = temp_vertices[vertexIndex - 1];
		XMFLOAT2 uv = temp_uvs[uvIndex - 1];
		XMFLOAT3 norm = temp_normals[normIndex - 1];
	
		v_vertices.push_back(vertex);
		v_uvs.push_back(uv);
		v_normals.push_back(norm);
	}
	return true;
}

bool ObjectModel::Init(XMFLOAT3 pos,
	ID3D11Device* dev, 
	ID3D11DeviceContext* devCon, 
	ProjViewMatricies* _viewproj,
	bool texture)
{
	world.r[3] = XMVectorSet(pos.x, pos.y, pos.z, 1.f);

	ProjView = _viewproj;

	hasTexture = texture;
#pragma region Obj Subresource
	HRESULT tester = 0;
	D3D11_BUFFER_DESC assigner;
	assigner.Usage = D3D11_USAGE_IMMUTABLE;
	assigner.CPUAccessFlags = NULL;
	assigner.MiscFlags = NULL;
	assigner.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	assigner.ByteWidth = sizeof(vector<XMFLOAT3>) * v_vertices.size();
	assigner.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA obj_verts;
	obj_verts.pSysMem = &v_vertices;
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
	// TODO: PART 2 STEP 8a


	/*D3D11_INPUT_ELEMENT_DESC vLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		
		{ "UVS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		
		{ "NORMS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	D3D11_INPUT_ELEMENT_DESC pLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		
		{ "UVS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};*/

	D3D11_INPUT_ELEMENT_DESC vLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};


	tester = dev->CreateInputLayout(vLayout, 2, Trivial_VS,
		sizeof(Trivial_VS), &layout);
#pragma endregion


#pragma region ObjTextures
	if (hasTexture)
	{
		////TextureBuffer
		//D3D11_TEXTURE2D_DESC descText;
		//descText.MipLevels = Tron_numlevels;
		//descText.ArraySize = 1;
		//descText.MiscFlags = NULL;
		//descText.CPUAccessFlags = NULL;
		//descText.SampleDesc.Quality = 0;
		//descText.SampleDesc.Count = 1;
		//descText.Width = Tron_width;
		//descText.Height = Tron_height;
		//descText.Usage = D3D11_USAGE_DEFAULT;
		//descText.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		//descText.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		//D3D11_SUBRESOURCE_DATA textSub[Tron_numlevels];
		//for (int i = 0; i < Tron_numlevels; i++)
		//{
		//	ZeroMemory(&textSub[i], sizeof(textSub[i]));
		//	textSub[i].pSysMem = Tron_pixels + Tron_leveloffsets[i];
		//	textSub[i].SysMemPitch = (Tron_width >> i) * sizeof(unsigned int);
		//}

		//tester = dev->CreateTexture2D(&descText, textSub, &pTexture2D);

		//D3D11_SAMPLER_DESC sampleDec;
		//sampleDec.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		//sampleDec.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		//sampleDec.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		//sampleDec.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		//sampleDec.MinLOD = (-FLT_MAX);
		//sampleDec.MaxLOD = (FLT_MAX);
		//sampleDec.MipLODBias = 0.0f;
		//sampleDec.MaxAnisotropy = 1;
		//sampleDec.ComparisonFunc = D3D11_COMPARISON_NEVER;


		//tester = dev->CreateSamplerState(&sampleDec, &samplerState);
		//
		//D3D11_SHADER_RESOURCE_VIEW_DESC pDesc;
		//pDesc.Texture2D.MipLevels = Tron_numlevels;
		//pDesc.Texture2D.MostDetailedMip = 0;
		//pDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		//pDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;


		//D3D11_BUFFEREX_SRV pShaderBuf;
		//pShaderBuf.FirstElement = 0;
		//pShaderBuf.NumElements = Tron_numpixels;
		//pShaderBuf.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
		//pDesc.BufferEx = pShaderBuf;


		//tester = dev->CreateShaderResourceView(pTexture2D, NULL, &shaderRes);
		//devCon->PSSetShaderResources(0, 1, &shaderRes);
	}

#pragma endregion

#pragma region Obj IndexBuffer
	//index buffer settings
	D3D11_BUFFER_DESC indexBuffer;
	indexBuffer.Usage = D3D11_USAGE_IMMUTABLE;
	indexBuffer.MiscFlags = NULL;
	indexBuffer.CPUAccessFlags = NULL;
	indexBuffer.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBuffer.ByteWidth = sizeof(vector<XMFLOAT3>) * vertexIndices.size() ;
	indexBuffer.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexSub;
	indexSub.pSysMem = &vertexIndices;
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
	memcpy(mapRes.pData, &world, sizeof(world));

	devCon->Unmap(matrixLocationBuffer[0], NULL);

	ZeroMemory(&mapRes, sizeof(D3D11_MAPPED_SUBRESOURCE));
	tester = devCon->Map(matrixLocationBuffer[1], NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapRes);
	memcpy(mapRes.pData, ProjView, sizeof(ProjViewMatricies));
	devCon->Unmap(matrixLocationBuffer[1], NULL);
	ProjView->view = XMMatrixInverse(nullptr, ProjView->view);

	devCon->VSSetConstantBuffers(0, 2, matrixLocationBuffer);
#pragma endregion



#pragma region VS and PS
	UINT strides = sizeof(vector < XMFLOAT3 >);
	UINT offsets = 0;


	devCon->IASetVertexBuffers(0, 1, &VertBuff, &strides, &offsets);

	devCon->IASetIndexBuffer(IndexBuff, DXGI_FORMAT_R16_UINT, offsets);
	// TODO: PART 2 STEP 9b
	devCon->VSSetShader(vertexShader, 0, 0);
	devCon->PSSetShader(pixelShader, 0, 0);

	// TODO: PART 2 STEP 9c
	devCon->IASetInputLayout(layout);

	// TODO: PART 2 STEP 9d
	devCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// TODO: PART 2 STEP 10
	devCon->DrawIndexed(vertexIndices.size(), 0, 0);
	

#pragma endregion


	return true;
}


void ObjectModel::Cleanup()
{
	SAFE_RELEASE(VertBuff);             // Models vertex buffer
	SAFE_RELEASE(IndexBuff);            // Models index buffer
	SAFE_RELEASE(matrixLocationBuffer[0]);
	SAFE_RELEASE(matrixLocationBuffer[1]);
	SAFE_RELEASE(layout);
	SAFE_RELEASE(pixelShader);
	SAFE_RELEASE(vertexShader);
}