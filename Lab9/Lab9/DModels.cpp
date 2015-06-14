#include "DModels.h"


DModels::DModels()
{
}


DModels::~DModels()
{
	generalHolder.SwapChain->SetFullscreenState(false, NULL);
	//PostMessage(hwnd, WM_DESTROY, 0, 0);
	//Release the COM Objects we created
	generalHolder.SwapChain->Release();
	generalHolder.d3d11Device->Release();
	generalHolder.d3d11DevCon->Release();
	generalHolder.backBuffer->Release();
	m_VS->Release();
	m_PS->Release();
	m_vertLayout->Release();
	generalHolder.depthStencilView->Release();
	generalHolder.depthStencilBuffer->Release();
	m_cbPerObjectBuffer->Release();
	m_CCWcullMode->Release();
	m_CWcullMode->Release();
	//m_RSCullNone->Release();
	//m_Transparency->Release();

	m_cbPerFrameBuffer->Release();

	m_DIKeyboard->Unacquire();
	m_DIMouse->Unacquire();
	//DirectInput->Release();
	//VS_Buffer->Release();
	//PS_Buffer->Release();
}



void CameraMatrix::Init(XMMATRIX* _camView,
						XMMATRIX* _camProjection,
						XMMATRIX* _camRotationMatrix)
{
	p_camView = _camView;
	p_camProjection = _camProjection;
	p_camRotationMatrix = _camRotationMatrix;

	float yscale = 1 / tanf(.1f * FOV);
	float xscale = yscale * (BACKBUFFER_HEIGHT / BACKBUFFER_WIDTH);

	*p_camProjection = XMMatrixPerspectiveFovLH(yscale, xscale, zNear, zFar);

	// Camera information
	m_camPosition = XMVectorSet(0.0f, 5.0f, -8.0f, 0.0f);
	m_camTarget = XMVectorSet(0.0f, 0.5f, 0.0f, 0.0f);
	m_camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	// Set the View matrix
	*p_camView = XMMatrixLookAtLH(m_camPosition, m_camTarget, m_camUp);

}

void DModels::DrawScene()
{
	// Update the cbPerFrame
	m_cbPerFrame.light = m_light;
	generalHolder.d3d11DevCon->UpdateSubresource(m_cbPerFrameBuffer, 0, NULL, &m_cbPerFrame, 0, 0);
	generalHolder.d3d11DevCon->PSSetConstantBuffers(0, 1, &m_cbPerFrameBuffer);

	// Set our Render Target
	generalHolder.d3d11DevCon->OMSetRenderTargets(1, &generalHolder.backBuffer, generalHolder.depthStencilView);

	// Set the default blend state (no blending) for opaque objects
	generalHolder.d3d11DevCon->OMSetBlendState(0, 0, 0xffffffff);

	// Set Vertex and Pixel Shaders
	generalHolder.d3d11DevCon->VSSetShader(m_VS, 0, 0);
	generalHolder.d3d11DevCon->PSSetShader(m_PS, 0, 0);

	generalHolder.d3d11DevCon->PSSetSamplers(0, 1, &m_LinearSamplerState);
	//generalHolder.d3d11DevCon->RSSetState(m_RSCullNone);

	/////Draw our model's NON-transparent subsets/////
	// Only set the vert and index buffer once per model
	// Set the grounds index buffer
	generalHolder.d3d11DevCon->IASetIndexBuffer(m_objModel.IndexBuff, DXGI_FORMAT_R32_UINT, 0);
	// Set the grounds vertex buffer
	generalHolder.d3d11DevCon->IASetVertexBuffers(0, 1, &m_objModel.VertBuff, &stride, &offset);

	// Set the WVP matrix and send it to the constant buffer in effect file
	// This also only needs to be set once per model
	m_WVP = m_objModel.World * *cam->p_camView * *cam->p_camProjection;
	m_cbPerObj.WVP = XMMatrixTranspose(m_WVP);
	m_cbPerObj.World = XMMatrixTranspose(m_objModel.World);

	for (int i = 0; i < m_objModel.Subsets; ++i)
	{
		// Only draw the NON-transparent parts of the model. 
		//if (!material[m_objModel.SubsetMaterialID[i]].IsTransparent)
		{
			//m_cbPerObj.difColor = material[m_objModel.SubsetMaterialID[i]].Diffuse;      // Let shader know which color to draw the model 
			// (if no diffuse texture we defined)
			//m_cbPerObj.hasTexture = material[m_objModel.SubsetMaterialID[i]].HasDiffTexture; // Let shader know if we need to use a texture
			//m_cbPerObj.hasNormMap = material[m_objModel.SubsetMaterialID[i]].HasNormMap; // Let shader know if we need to do normal mapping
			generalHolder.d3d11DevCon->UpdateSubresource(m_cbPerObjectBuffer, 0, NULL, &m_cbPerObj, 0, 0);
			generalHolder.d3d11DevCon->VSSetConstantBuffers(0, 1, &m_cbPerObjectBuffer);
			generalHolder.d3d11DevCon->PSSetConstantBuffers(1, 1, &m_cbPerObjectBuffer);

			//// If this subset has a diffuse texture, send it to the pixel shader
			//if (material[m_objModel.SubsetMaterialID[i]].HasDiffTexture)
			//	generalHolder.d3d11DevCon->PSSetShaderResources(0, 1, &m_Textures.TextureList[material[m_objModel.SubsetMaterialID[i]].DiffuseTextureID]);

			//// If this subset has a normal (bump) map, send it to the pixel shader
			//if (material[m_objModel.SubsetMaterialID[i]].HasNormMap)
			//	generalHolder.d3d11DevCon->PSSetShaderResources(1, 1, &m_Textures.TextureList[material[m_objModel.SubsetMaterialID[i]].NormMapTextureID]);

			// Draw the NON-transparent stuff
			int indexStart = m_objModel.SubsetIndexStart[i];
			int indexDrawAmount = m_objModel.SubsetIndexStart[i + 1] - indexStart;
			generalHolder.d3d11DevCon->DrawIndexed(indexDrawAmount, indexStart, 0);
		}
	}



	// Draw other opaque objects (non-transparent)



	// Now after all the non transparent objects have been drawn, draw the transparent stuff



	/////Draw our model's TRANSPARENT subsets/////
	generalHolder.d3d11DevCon->IASetIndexBuffer(m_objModel.IndexBuff, DXGI_FORMAT_R32_UINT, 0);
	generalHolder.d3d11DevCon->IASetVertexBuffers(0, 1, &m_objModel.VertBuff, &stride, &offset);

	m_WVP = m_objModel.World * *cam->p_camView * *cam->p_camProjection;
	m_cbPerObj.WVP = XMMatrixTranspose(m_WVP);
	m_cbPerObj.World = XMMatrixTranspose(m_objModel.World);

	//for (int i = 0; i < m_objModel.Subsets; ++i)
	//{
	//	// Only draw the TRANSPARENT parts of the model now. 
	//	if (material[m_objModel.SubsetMaterialID[i]].IsTransparent)
	//	{
	//		m_cbPerObj.difColor = material[m_objModel.SubsetMaterialID[i]].Diffuse;      // Let shader know which color to draw the model 
	//		// (if no diffuse texture we defined)
	//		m_cbPerObj.hasTexture = material[m_objModel.SubsetMaterialID[i]].HasDiffTexture; // Let shader know if we need to use a texture
	//		m_cbPerObj.hasNormMap = material[m_objModel.SubsetMaterialID[i]].HasNormMap; // Let shader know if we need to do normal mapping
	//		generalHolder.d3d11DevCon->UpdateSubresource(m_cbPerObjectBuffer, 0, NULL, &m_cbPerObj, 0, 0);
	//		generalHolder.d3d11DevCon->VSSetConstantBuffers(0, 1, &m_cbPerObjectBuffer);
	//		generalHolder.d3d11DevCon->PSSetConstantBuffers(1, 1, &m_cbPerObjectBuffer);

	//		// If this subset has a diffuse texture, send it to the pixel shader
	//		if (material[m_objModel.SubsetMaterialID[i]].HasDiffTexture)
	//			generalHolder.d3d11DevCon->PSSetShaderResources(0, 1, &m_Textures.TextureList[material[m_objModel.SubsetMaterialID[i]].DiffuseTextureID]);

	//		// If this subset has a normal (bump) map, send it to the pixel shader
	//		if (material[m_objModel.SubsetMaterialID[i]].HasNormMap)
	//			generalHolder.d3d11DevCon->PSSetShaderResources(1, 1, &m_Textures.TextureList[material[m_objModel.SubsetMaterialID[i]].NormMapTextureID]);

	//		// The transparent parts are now drawn
	//		int indexStart = m_objModel.SubsetIndexStart[i];
	//		int indexDrawAmount = m_objModel.SubsetIndexStart[i + 1] - indexStart;
	//		generalHolder.d3d11DevCon->DrawIndexed(indexDrawAmount, indexStart, 0);
	//	}
	//}
}




bool DModels::LoadObjModel(
	std::wstring Filename,
	bool IsRHCoordSys,
	bool ComputeNormals,
	bool hasMat)
{
	//HRESULT hr = 0;

	wifstream fileIn(Filename.c_str());   // Open file
	wstring meshMatLib;                    // String to hold our obj material library filename (model.mtl)

	// Arrays to store our model's information
	vector<DWORD> indices;
	vector<XMFLOAT3> vertPos;
	vector<XMFLOAT3> vertNorm;
	vector<XMFLOAT2> vertTexCoord;
	vector<std::wstring> meshMaterials;

	// Vertex definition indices
	vector<int> vertPosIndex;
	vector<int> vertNormIndex;
	vector<int> vertTCIndex;

	// Make sure we have a default if no tex coords or normals are defined
	bool hasTexCoord = false;
	bool hasNorm = false;

	// Temp variables to store into vectors
	wstring meshMaterialsTemp;
	int vertPosIndexTemp;
	int vertNormIndexTemp;
	int vertTCIndexTemp;

	wchar_t checkChar;      // The variable we will use to store one char from file at a time
	wstring face;      // Holds the string containing our face vertices
	int vIndex = 0;         // Keep track of our vertex index count
	int triangleCount = 0;  // Total Triangles
	int totalVerts = 0;
	int meshTriangles = 0;

	//Check to see if the file was opened
	if (fileIn)
	{
		while (fileIn)
		{
			checkChar = fileIn.get();   //Get next char

			switch (checkChar)
			{
				// A comment. Skip rest of the line
			case '#':
				checkChar = fileIn.get();
				while (checkChar != '\n')
					checkChar = fileIn.get();
				break;

				// Get Vertex Descriptions;
			case 'v':
				checkChar = fileIn.get();
				if (checkChar == ' ')  // v - vert position
				{
					float vz, vy, vx;
					fileIn >> vx >> vy >> vz;   // Store the next three types

					if (IsRHCoordSys)            // If model is from an RH Coord System
						vertPos.push_back(XMFLOAT3(vx, vy, vz * -1.0f));   // Invert the Z axis
					else
						vertPos.push_back(XMFLOAT3(vx, vy, vz));
				}
				if (checkChar == 't')  // vt - vert tex coords
				{
					float vtcu, vtcv;
					fileIn >> vtcu >> vtcv;     // Store next two types

					if (IsRHCoordSys)            // If model is from an RH Coord System
						vertTexCoord.push_back(XMFLOAT2(vtcu, 1.0f - vtcv));  // Reverse the "v" axis
					else
						vertTexCoord.push_back(XMFLOAT2(vtcu, vtcv));

					hasTexCoord = true;         // We know the model uses texture coords
				}
				if (checkChar == 'n')          // vn - vert normal
				{
					float vnx, vny, vnz;
					fileIn >> vnx >> vny >> vnz;    // Store next three types

					if (IsRHCoordSys)                // If model is from an RH Coord System
						vertNorm.push_back(XMFLOAT3(vnx, vny, vnz * -1.0f));  // Invert the Z axis
					else
						vertNorm.push_back(XMFLOAT3(vnx, vny, vnz));

					hasNorm = true;                 // We know the model defines normals
				}
				break;

				// New group (Subset)
			case 'g': // g - defines a group
				checkChar = fileIn.get();
				if (checkChar == ' ')
				{
					m_objModel.SubsetIndexStart.push_back(vIndex);       // Start index for this subset
					m_objModel.Subsets++;
				}
				break;

				// Get Face Index
			case 'f': // f - defines the faces
				checkChar = fileIn.get();
				if (checkChar == ' ')
				{
					face = L"";               // Holds all vertex definitions for the face (eg. "1\1\1 2\2\2 3\3\3")
					wstring VertDef;   // Holds one vertex definition at a time (eg. "1\1\1")
					triangleCount = 0;      // Keep track of the triangles for this face, since we will have to 
					// "retriangulate" faces with more than 3 sides

					checkChar = fileIn.get();
					while (checkChar != '\n')
					{
						face += checkChar;          // Add the char to our face string
						checkChar = fileIn.get();   // Get the next Character
						if (checkChar == ' ')      // If its a space...
							triangleCount++;        // Increase our triangle count
					}

					// Check for space at the end of our face string
					if (face[face.length() - 1] == ' ')
						triangleCount--;    // Each space adds to our triangle count

					triangleCount -= 1;     // Ever vertex in the face AFTER the first two are new faces

					std::wstringstream ss(face);

					if (face.length() > 0)
					{
						int firstVIndex, lastVIndex;    // Holds the first and last vertice's index

						for (int i = 0; i < 3; ++i)      // First three vertices (first triangle)
						{
							ss >> VertDef;  // Get vertex definition (vPos/vTexCoord/vNorm)

							std::wstring vertPart;
							int whichPart = 0;      // (vPos, vTexCoord, or vNorm)

							// Parse this string
							for (int j = 0; j < (int)VertDef.length(); ++j)
							{
								if (VertDef[j] != '/') // If there is no divider "/", add a char to our vertPart
									vertPart += VertDef[j];

								// If the current char is a divider "/", or its the last character in the string
								if (VertDef[j] == '/' || j == VertDef.length() - 1)
								{
									std::wistringstream wstringToInt(vertPart); // Used to convert wstring to int

									if (whichPart == 0)  // If it's the vPos
									{
										wstringToInt >> vertPosIndexTemp;
										vertPosIndexTemp -= 1;      // subtract one since c++ arrays start with 0, and obj start with 1

										// Check to see if the vert pos was the only thing specified
										if (j == VertDef.length() - 1)
										{
											vertNormIndexTemp = 0;
											vertTCIndexTemp = 0;
										}
									}

									else if (whichPart == 1) // If vTexCoord
									{
										if (vertPart != L"")   // Check to see if there even is a tex coord
										{
											wstringToInt >> vertTCIndexTemp;
											vertTCIndexTemp -= 1;   // subtract one since c++ arrays start with 0, and obj start with 1
										}
										else    // If there is no tex coord, make a default
											vertTCIndexTemp = 0;

										// If the cur. char is the second to last in the string, then
										// there must be no normal, so set a default normal
										if (j == VertDef.length() - 1)
											vertNormIndexTemp = 0;

									}
									else if (whichPart == 2) // If vNorm
									{
										std::wistringstream wstringToInt(vertPart);

										wstringToInt >> vertNormIndexTemp;
										vertNormIndexTemp -= 1;     // subtract one since c++ arrays start with 0, and obj start with 1
									}

									vertPart = L"";   // Get ready for next vertex part
									whichPart++;    // Move on to next vertex part                  
								}
							}

							// Check to make sure there is at least one subset
							if (m_objModel.Subsets == 0)
							{
								m_objModel.SubsetIndexStart.push_back(vIndex);       // Start index for this subset
								m_objModel.Subsets++;
							}

							// Avoid duplicate vertices
							bool vertAlreadyExists = false;
							if (totalVerts >= 3) // Make sure we at least have one triangle to check
							{
								// Loop through all the vertices
								for (int iCheck = 0; iCheck < totalVerts; ++iCheck)
								{
									// If the vertex position and texture coordinate in memory are the same
									// As the vertex position and texture coordinate we just now got out
									// of the obj file, we will set this faces vertex index to the vertex's
									// index value in memory. This makes sure we don't create duplicate vertices
									if (vertPosIndexTemp == vertPosIndex[iCheck] && !vertAlreadyExists)
									{
										if (vertTCIndexTemp == vertTCIndex[iCheck])
										{
											m_objModel.Indices.push_back(iCheck);        // Set index for this vertex
											vertAlreadyExists = true;       // If we've made it here, the vertex already exists
										}
									}
								}
							}

							// If this vertex is not already in our vertex arrays, put it there
							if (!vertAlreadyExists)
							{
								vertPosIndex.push_back(vertPosIndexTemp);
								vertTCIndex.push_back(vertTCIndexTemp);
								vertNormIndex.push_back(vertNormIndexTemp);
								totalVerts++;   // We created a new vertex
								m_objModel.Indices.push_back(totalVerts - 1);  // Set index for this vertex
							}

							// If this is the very first vertex in the face, we need to
							// make sure the rest of the triangles use this vertex
							if (i == 0)
							{
								firstVIndex = m_objModel.Indices[vIndex];    //The first vertex index of this FACE

							}

							// If this was the last vertex in the first triangle, we will make sure
							// the next triangle uses this one (eg. tri1(1,2,3) tri2(1,3,4) tri3(1,4,5))
							if (i == 2)
							{
								lastVIndex = m_objModel.Indices[vIndex]; // The last vertex index of this TRIANGLE
							}
							vIndex++;   // Increment index count
						}

						meshTriangles++;    // One triangle down

						// If there are more than three vertices in the face definition, we need to make sure
						// we convert the face to triangles. We created our first triangle above, now we will
						// create a new triangle for every new vertex in the face, using the very first vertex
						// of the face, and the last vertex from the triangle before the current triangle
						for (int l = 0; l < triangleCount - 1; ++l)    // Loop through the next vertices to create new triangles
						{
							// First vertex of this triangle (the very first vertex of the face too)
							m_objModel.Indices.push_back(firstVIndex);           // Set index for this vertex
							vIndex++;

							// Second Vertex of this triangle (the last vertex used in the tri before this one)
							m_objModel.Indices.push_back(lastVIndex);            // Set index for this vertex
							vIndex++;

							// Get the third vertex for this triangle
							ss >> VertDef;

							std::wstring vertPart;
							int whichPart = 0;

							// Parse this string (same as above)
							for (int j = 0; j < (int)VertDef.length(); ++j)
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

										// Check to see if the vert pos was the only thing specified
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

							// Check for duplicate vertices
							bool vertAlreadyExists = false;
							if (totalVerts >= 3) // Make sure we at least have one triangle to check
							{
								for (int iCheck = 0; iCheck < totalVerts; ++iCheck)
								{
									if (vertPosIndexTemp == vertPosIndex[iCheck] && !vertAlreadyExists)
									{
										if (vertTCIndexTemp == vertTCIndex[iCheck])
										{
											m_objModel.Indices.push_back(iCheck);        // Set index for this vertex
											vertAlreadyExists = true;       // If we've made it here, the vertex already exists
										}
									}
								}
							}

							if (!vertAlreadyExists)
							{
								vertPosIndex.push_back(vertPosIndexTemp);
								vertTCIndex.push_back(vertTCIndexTemp);
								vertNormIndex.push_back(vertNormIndexTemp);
								totalVerts++;                       // New vertex created, add to total verts
								m_objModel.Indices.push_back(totalVerts - 1);  // Set index for this vertex
							}

							// Set the second vertex for the next triangle to the last vertex we got        
							lastVIndex = m_objModel.Indices[vIndex]; // The last vertex index of this TRIANGLE

							meshTriangles++;    // New triangle defined
							vIndex++;
						}
					}
				}
				break;

			case 'm': // mtllib - material library filename
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
										// Store the material libraries file name
										fileIn >> meshMatLib;
									}
								}
							}
						}
					}
				}

				break;

			case 'u': // usemtl - which material to use
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
										meshMaterialsTemp = L"";  // Make sure this is cleared

										fileIn >> meshMaterialsTemp; // Get next type (string)

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
	}
	else    // If we could not open the file
	{
		//SwapChain->SetFullscreenState(false, NULL); // Make sure we are out of fullscreen

		// create message
		std::wstring message = L"Could not open: ";
		message += Filename;

		MessageBox(0, message.c_str(),  // Display message
			L"Error", MB_OK);

		return false;
	}

	m_objModel.SubsetIndexStart.push_back(vIndex); // There won't be another index start after our last subset, so set it here

	// sometimes "g" is defined at the very top of the file, then again before the first group of faces.
	// This makes sure the first subset does not conatain "0" indices.
	if (m_objModel.SubsetIndexStart[1] == 0)
	{
		m_objModel.SubsetIndexStart.erase(m_objModel.SubsetIndexStart.begin() + 1);
		m_objModel.Subsets--;
	}

	// Make sure we have a default for the tex coord and normal
	// if one or both are not specified
	if (!hasNorm)
		vertNorm.push_back(XMFLOAT3(0.0f, 0.0f, 0.0f));
	if (!hasTexCoord)
		vertTexCoord.push_back(XMFLOAT2(0.0f, 0.0f));

	// Close the obj file, and open the mtl file
	fileIn.close();
#if 0
	if (hasMat)
	{

		fileIn.open(meshMatLib.c_str());

		std::wstring lastStringRead;
		int matCount = material.size(); // Total materials

		if (fileIn.is_open())
		{
			while (fileIn)
			{
				checkChar = fileIn.get();   // Get next char

				switch (checkChar)
				{
					// Check for comment
				case '#':
					checkChar = fileIn.get();
					while (checkChar != '\n')
						checkChar = fileIn.get();
					break;

					// Set the colors
				case 'K':
					checkChar = fileIn.get();
					if (checkChar == 'd')  // Diffuse Color
					{
						checkChar = fileIn.get();   // Remove space

						fileIn >> material[matCount - 1].Diffuse.x;
						fileIn >> material[matCount - 1].Diffuse.y;
						fileIn >> material[matCount - 1].Diffuse.z;
					}

					if (checkChar == 'a')  // Ambient Color
					{
						checkChar = fileIn.get();   // Remove space

						fileIn >> material[matCount - 1].Ambient.x;
						fileIn >> material[matCount - 1].Ambient.y;
						fileIn >> material[matCount - 1].Ambient.z;
					}

					if (checkChar == 's')  // Ambient Color
					{
						checkChar = fileIn.get();   // Remove space

						fileIn >> material[matCount - 1].Specular.x;
						fileIn >> material[matCount - 1].Specular.y;
						fileIn >> material[matCount - 1].Specular.z;
					}
					break;

				case 'N':
					checkChar = fileIn.get();

					if (checkChar == 's')  // Specular Power (Coefficient)
					{
						checkChar = fileIn.get();   // Remove space

						fileIn >> material[matCount - 1].Specular.w;
					}

					break;

					// Check for transparency
				case 'T':
					checkChar = fileIn.get();
					if (checkChar == 'r')
					{
						checkChar = fileIn.get();   // Remove space
						float Transparency;
						fileIn >> Transparency;

						material[matCount - 1].Diffuse.w = Transparency;

						if (Transparency > 0.0f)
							material[matCount - 1].IsTransparent = true;
					}
					break;

					// Some obj files specify d for transparency
				case 'd':
					checkChar = fileIn.get();
					if (checkChar == ' ')
					{
						float Transparency;
						fileIn >> Transparency;

						// 'd' - 0 being most transparent, and 1 being opaque, opposite of Tr
						Transparency = 1.0f - Transparency;

						material[matCount - 1].Diffuse.w = Transparency;

						if (Transparency > 0.0f)
							material[matCount - 1].IsTransparent = true;
					}
					break;

					// Get the diffuse map (texture)
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
								// map_Kd - Diffuse map
								checkChar = fileIn.get();
								if (checkChar == 'K')
								{
									checkChar = fileIn.get();
									if (checkChar == 'd')
									{
										std::wstring fileNamePath;

										fileIn.get();   // Remove whitespace between map_Kd and file

										// Get the file path - We read the pathname char by char since
										// pathnames can sometimes contain spaces, so we will read until
										// we find the file extension
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
										for (int i = 0; i < (int)m_Textures.TextureNameArray.size(); ++i)
										{
											if (fileNamePath == m_Textures.TextureNameArray[i])
											{
												alreadyLoaded = true;
												material[matCount - 1].DiffuseTextureID = i;
												material[matCount - 1].HasDiffTexture = true;
											}
										}

										//if the texture is not already loaded, load it now
										if (!alreadyLoaded)
										{
											ID3D11ShaderResourceView* tempSRV;

											hr = CreateDDSTextureFromFile(generalHolder.d3d11Device, fileNamePath.c_str(),
												NULL, &tempSRV, NULL);
											if (SUCCEEDED(hr))
											{
												m_Textures.TextureNameArray.push_back(fileNamePath.c_str());
												material[matCount - 1].DiffuseTextureID = m_Textures.TextureList.size();
												m_Textures.TextureList.push_back(tempSRV);
												material[matCount - 1].HasDiffTexture = true;
											}
										}
									}

									// Get Ambient Map (texture)
									if (checkChar == 'a')
									{
										std::wstring fileNamePath;

										fileIn.get();   // Remove whitespace between map_Kd and file

										// Get the file path - We read the pathname char by char since
										// pathnames can sometimes contain spaces, so we will read until
										// we find the file extension
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
										for (int i = 0; i < (int)m_Textures.TextureNameArray.size(); ++i)
										{
											if (fileNamePath == m_Textures.TextureNameArray[i])
											{
												alreadyLoaded = true;
												material[matCount - 1].AmbientTextureID = i;
												material[matCount - 1].HasAmbientTexture = true;
											}
										}

										//if the texture is not already loaded, load it now
										if (!alreadyLoaded)
										{
											ID3D11ShaderResourceView* tempSRV;
											hr = CreateDDSTextureFromFile(generalHolder.d3d11Device, fileNamePath.c_str(),
												NULL, &tempSRV, NULL);
											if (SUCCEEDED(hr))
											{
												m_Textures.TextureNameArray.push_back(fileNamePath.c_str());
												material[matCount - 1].AmbientTextureID = m_Textures.TextureList.size();
												m_Textures.TextureList.push_back(tempSRV);
												material[matCount - 1].HasAmbientTexture = true;
											}
										}
									}

									// Get Specular Map (texture)
									if (checkChar == 's')
									{
										std::wstring fileNamePath;

										fileIn.get();   // Remove whitespace between map_Ks and file

										// Get the file path - We read the pathname char by char since
										// pathnames can sometimes contain spaces, so we will read until
										// we find the file extension
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
										for (int i = 0; i < (int)m_Textures.TextureNameArray.size(); ++i)
										{
											if (fileNamePath == m_Textures.TextureNameArray[i])
											{
												alreadyLoaded = true;
												material[matCount - 1].SpecularTextureID = i;
												material[matCount - 1].HasSpecularTexture = true;
											}
										}

										//if the texture is not already loaded, load it now
										if (!alreadyLoaded)
										{
											ID3D11ShaderResourceView* tempSRV;
											hr = CreateDDSTextureFromFile(generalHolder.d3d11Device, fileNamePath.c_str(),
												NULL, &tempSRV, NULL);
											if (SUCCEEDED(hr))
											{
												m_Textures.TextureNameArray.push_back(fileNamePath.c_str());
												material[matCount - 1].SpecularTextureID = m_Textures.TextureList.size();
												m_Textures.TextureList.push_back(tempSRV);
												material[matCount - 1].HasSpecularTexture = true;
											}
										}
									}
								}

								//map_d - alpha map
								else if (checkChar == 'd')
								{
									std::wstring fileNamePath;

									fileIn.get();   // Remove whitespace between map_Ks and file

									// Get the file path - We read the pathname char by char since
									// pathnames can sometimes contain spaces, so we will read until
									// we find the file extension
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
									for (int i = 0; i < (int)m_Textures.TextureNameArray.size(); ++i)
									{
										if (fileNamePath == m_Textures.TextureNameArray[i])
										{
											alreadyLoaded = true;
											material[matCount - 1].AlphaTextureID = i;
											material[matCount - 1].IsTransparent = true;
										}
									}

									//if the texture is not already loaded, load it now
									if (!alreadyLoaded)
									{
										ID3D11ShaderResourceView* tempSRV;
										hr = CreateDDSTextureFromFile(generalHolder.d3d11Device, fileNamePath.c_str(),
											NULL, &tempSRV, NULL);
										if (SUCCEEDED(hr))
										{
											m_Textures.TextureNameArray.push_back(fileNamePath.c_str());
											material[matCount - 1].AlphaTextureID = m_Textures.TextureList.size();
											m_Textures.TextureList.push_back(tempSRV);
											material[matCount - 1].IsTransparent = true;
										}
									}
								}

								// map_bump - bump map (Normal Map)
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

												fileIn.get();   // Remove whitespace between map_bump and file

												// Get the file path - We read the pathname char by char since
												// pathnames can sometimes contain spaces, so we will read until
												// we find the file extension
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
												for (int i = 0; i < (int)m_Textures.TextureNameArray.size(); ++i)
												{
													if (fileNamePath == m_Textures.TextureNameArray[i])
													{
														alreadyLoaded = true;
														material[matCount - 1].NormMapTextureID = i;
														material[matCount - 1].HasNormMap = true;
													}
												}

												//if the texture is not already loaded, load it now
												if (!alreadyLoaded)
												{
													ID3D11ShaderResourceView* tempSRV;
													hr = CreateDDSTextureFromFile(generalHolder.d3d11Device, fileNamePath.c_str(),
														NULL, &tempSRV, NULL);
													if (SUCCEEDED(hr))
													{
														m_Textures.TextureNameArray.push_back(fileNamePath.c_str());
														material[matCount - 1].NormMapTextureID = m_Textures.TextureList.size();
														m_Textures.TextureList.push_back(tempSRV);
														material[matCount - 1].HasNormMap = true;
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

				case 'n': // newmtl - Declare new material
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
											// New material, set its defaults
											SurfaceMaterial tempMat;
											material.push_back(tempMat);
											fileIn >> material[matCount].MatName;
											material[matCount].IsTransparent = false;
											material[matCount].HasDiffTexture = false;
											material[matCount].HasAmbientTexture = false;
											material[matCount].HasSpecularTexture = false;
											material[matCount].HasAlphaTexture = false;
											material[matCount].HasNormMap = false;
											material[matCount].NormMapTextureID = 0;
											material[matCount].DiffuseTextureID = 0;
											material[matCount].AlphaTextureID = 0;
											material[matCount].SpecularTextureID = 0;
											material[matCount].AmbientTextureID = 0;
											material[matCount].Specular = XMFLOAT4(0, 0, 0, 0);
											material[matCount].Ambient = XMFLOAT3(0, 0, 0);
											material[matCount].Diffuse = XMFLOAT4(0, 0, 0, 0);
											matCount++;
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
		}
		else    // If we could not open the material library
		{
			//SwapChain->SetFullscreenState(false, NULL); // Make sure we are out of fullscreen

			std::wstring message = L"Could not open: ";
			message += meshMatLib;

			//MessageBox(0, message.c_str(),
			//	L"Error", MB_OK);

			return false;
		}

	}

	// Set the subsets material to the index value
	// of the its material in our material array
	for (int i = 0; i < m_objModel.Subsets; ++i)
	{
		bool hasMat = false;
		for (int j = 0; j < (int)material.size(); ++j)
		{
			if (meshMaterials[i] == material[j].MatName)
			{
				m_objModel.SubsetMaterialID.push_back(j);
				hasMat = true;
			}
		}
		if (!hasMat)
			m_objModel.SubsetMaterialID.push_back(0); // Use first material in array
	}

#endif // 0
	std::vector<Vertex> vertices;
	Vertex tempVert;

	// Create our vertices using the information we got 
	// from the file and store them in a vector
	for (int j = 0; j < totalVerts; ++j)
	{
		tempVert.pos = vertPos[vertPosIndex[j]];
		tempVert.normal = vertNorm[vertNormIndex[j]];
		tempVert.texCoord = vertTexCoord[vertTCIndex[j]];

		vertices.push_back(tempVert);
		m_objModel.Vertices.push_back(tempVert.pos);
	}

	//If computeNormals was set to true then we will create our own
	//normals, if it was set to false we will use the obj files normals
	if (ComputeNormals)
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
			vecX = vertices[m_objModel.Indices[(i * 3)]].pos.x - vertices[m_objModel.Indices[(i * 3) + 2]].pos.x;
			vecY = vertices[m_objModel.Indices[(i * 3)]].pos.y - vertices[m_objModel.Indices[(i * 3) + 2]].pos.y;
			vecZ = vertices[m_objModel.Indices[(i * 3)]].pos.z - vertices[m_objModel.Indices[(i * 3) + 2]].pos.z;
			edge1 = XMVectorSet(vecX, vecY, vecZ, 0.0f);    //Create our first edge

			//Get the vector describing another edge of our triangle (edge 2,1)
			vecX = vertices[m_objModel.Indices[(i * 3) + 2]].pos.x - vertices[m_objModel.Indices[(i * 3) + 1]].pos.x;
			vecY = vertices[m_objModel.Indices[(i * 3) + 2]].pos.y - vertices[m_objModel.Indices[(i * 3) + 1]].pos.y;
			vecZ = vertices[m_objModel.Indices[(i * 3) + 2]].pos.z - vertices[m_objModel.Indices[(i * 3) + 1]].pos.z;
			edge2 = XMVectorSet(vecX, vecY, vecZ, 0.0f);    //Create our second edge

			//Cross multiply the two edge vectors to get the un-normalized face normal
			XMStoreFloat3(&unnormalized, XMVector3Cross(edge1, edge2));

			tempNormal.push_back(unnormalized);

			//Find first texture coordinate edge 2d vector
			tcU1 = vertices[m_objModel.Indices[(i * 3)]].texCoord.x - vertices[m_objModel.Indices[(i * 3) + 2]].texCoord.x;
			tcV1 = vertices[m_objModel.Indices[(i * 3)]].texCoord.y - vertices[m_objModel.Indices[(i * 3) + 2]].texCoord.y;

			//Find second texture coordinate edge 2d vector
			tcU2 = vertices[m_objModel.Indices[(i * 3) + 2]].texCoord.x - vertices[m_objModel.Indices[(i * 3) + 1]].texCoord.x;
			tcV2 = vertices[m_objModel.Indices[(i * 3) + 2]].texCoord.y - vertices[m_objModel.Indices[(i * 3) + 1]].texCoord.y;

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
		float tX, tY, tZ;   //temp axis variables

		//Go through each vertex
		for (int i = 0; i < totalVerts; ++i)
		{
			//Check which triangles use this vertex
			for (int j = 0; j < meshTriangles; ++j)
			{
				if (m_objModel.Indices[j * 3] == i ||
					m_objModel.Indices[(j * 3) + 1] == i ||
					m_objModel.Indices[(j * 3) + 2] == i)
				{
					tX = XMVectorGetX(normalSum) + tempNormal[j].x;
					tY = XMVectorGetY(normalSum) + tempNormal[j].y;
					tZ = XMVectorGetZ(normalSum) + tempNormal[j].z;

					normalSum = XMVectorSet(tX, tY, tZ, 0.0f);  //If a face is using the vertex, add the unormalized face normal to the normalSum

					//We can reuse tX, tY, tZ to sum up tangents
					tX = XMVectorGetX(tangentSum) + tempTangent[j].x;
					tY = XMVectorGetY(tangentSum) + tempTangent[j].y;
					tZ = XMVectorGetZ(tangentSum) + tempTangent[j].z;

					tangentSum = XMVectorSet(tX, tY, tZ, 0.0f); //sum up face tangents using this vertex

					facesUsing++;
				}
			}

			//Get the actual normal by dividing the normalSum by the number of faces sharing the vertex
			normalSum = normalSum / (float)facesUsing;
			tangentSum = tangentSum / (float)facesUsing;

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

	// Create Axis-Aligned Bounding Box (AABB)
	XMFLOAT3 minVertex = XMFLOAT3(FLT_MAX, FLT_MAX, FLT_MAX);
	XMFLOAT3 maxVertex = XMFLOAT3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	m_objModel.BoundingSphere = 0;

	for (UINT i = 0; i < m_objModel.Vertices.size(); i++)
	{
		// The minVertex and maxVertex will most likely not be actual vertices in the model, but vertices
		// that use the smallest and largest x, y, and z values from the model to be sure ALL vertices are
		// covered by the bounding volume

		//Get the smallest vertex 
		minVertex.x = min(minVertex.x, m_objModel.Vertices[i].x);	// Find smallest x value in model
		minVertex.y = min(minVertex.y, m_objModel.Vertices[i].y);	// Find smallest y value in model
		minVertex.z = min(minVertex.z, m_objModel.Vertices[i].z);	// Find smallest z value in model

		//Get the largest vertex 
		maxVertex.x = max(maxVertex.x, m_objModel.Vertices[i].x);	// Find largest x value in model
		maxVertex.y = max(maxVertex.y, m_objModel.Vertices[i].y);	// Find largest y value in model
		maxVertex.z = max(maxVertex.z, m_objModel.Vertices[i].z);	// Find largest z value in model
	}

	// Our AABB [0] is the min vertex and [1] is the max
	m_objModel.AABB.push_back(minVertex);
	m_objModel.AABB.push_back(maxVertex);

	// Get models true center
	m_objModel.Center.x = maxVertex.x - minVertex.x / 2.0f;
	m_objModel.Center.y = maxVertex.y - minVertex.y / 2.0f;
	m_objModel.Center.z = maxVertex.z - minVertex.z / 2.0f;

	// Now that we have the center, get the bounding sphere	
	for (UINT i = 0; i < m_objModel.Vertices.size(); i++)
	{
		float x = (m_objModel.Center.x - m_objModel.Vertices[i].x) * (m_objModel.Center.x - m_objModel.Vertices[i].x);
		float y = (m_objModel.Center.y - m_objModel.Vertices[i].y) * (m_objModel.Center.y - m_objModel.Vertices[i].y);
		float z = (m_objModel.Center.z - m_objModel.Vertices[i].z) * (m_objModel.Center.z - m_objModel.Vertices[i].z);

		// Get models bounding sphere
		m_objModel.BoundingSphere = max(m_objModel.BoundingSphere, (x + y + z));
	}

	// We didn't use the square root when finding the largest distance since it slows things down.
	// We can square root the answer from above to get the actual bounding sphere now
	m_objModel.BoundingSphere = sqrt(m_objModel.BoundingSphere);


	//Create index buffer
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * meshTriangles * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData;

	iinitData.pSysMem = &m_objModel.Indices[0];
	generalHolder.d3d11Device->CreateBuffer(&indexBufferDesc, &iinitData, &m_objModel.IndexBuff);

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
	*tester = generalHolder.d3d11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &m_objModel.VertBuff);






	// Create the buffer to send to the cbuffer in effect file
	D3D11_BUFFER_DESC cbbd;
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof(m_cbPerObject);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	*tester = generalHolder.d3d11Device->CreateBuffer(&cbbd, NULL, &m_cbPerObjectBuffer);

	// Create the buffer to send to the cbuffer per frame in effect file
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof(cbPerFrame);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	*tester = generalHolder.d3d11Device->CreateBuffer(&cbbd, NULL, &m_cbPerFrameBuffer);


	// Create the Shader Objects
	*tester = generalHolder.d3d11Device->CreatePixelShader(Trivial_PS, sizeof(Trivial_PS), NULL,  &m_PS);
	*tester = generalHolder.d3d11Device->CreateVertexShader(Trivial_VS, sizeof(Trivial_VS), NULL, &m_VS);
	
	// Set Vertex and Pixel Shaders
	generalHolder.d3d11DevCon->VSSetShader(m_VS, 0, 0);
	generalHolder.d3d11DevCon->PSSetShader(m_PS, 0, 0);

	m_light.pos = XMFLOAT3(0.0f, 7.0f, 0.0f);
	m_light.dir = XMFLOAT3(0.5f, 0.75f, -0.5f);
	m_light.range = 1000.0f;
	m_light.cone = 12.0f;
	m_light.att = XMFLOAT3(0.4f, 0.02f, 0.000f);
	m_light.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_light.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	UINT numElements = ARRAYSIZE(layout);

	// Create the Input Layout
	*tester = generalHolder.d3d11Device->CreateInputLayout(layout, 4, Grid_VS, sizeof(Grid_VS), &m_vertLayout);

	// Set the Input Layout
	generalHolder.d3d11DevCon->IASetInputLayout(m_vertLayout);

	// Set Primitive Topology
	generalHolder.d3d11DevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);









	return true;
}

void GeneralHolder::Init(
	ID3D11RenderTargetView* bb,
	ID3D11Device* dev,
	IDXGISwapChain* swap,
	ID3D11DeviceContext* devc,
	ID3D11DepthStencilView* depths,
	ID3D11Texture2D* stencBuf)
{
	backBuffer = bb;
	d3d11Device = dev;
	SwapChain = swap;
	d3d11DevCon = devc;
	depthStencilView = depths;
	depthStencilBuffer = stencBuf;
}


void DModels::Init(
				GeneralHolder _gh,
				CameraMatrix *_cam,
				HRESULT* _tester
				)
{
	generalHolder = _gh;
	cam = _cam;
	tester = _tester;
	//DXGI_SWAP_CHAIN_DESC scd;

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
	*tester = generalHolder.d3d11Device->CreateSamplerState(&sampDesc, &m_LinearSamplerState);

	D3D11_RASTERIZER_DESC cmdesc;

	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_BACK;
	cmdesc.FrontCounterClockwise = true;
	*tester = generalHolder.d3d11Device->CreateRasterizerState(&cmdesc, &m_CCWcullMode);

	cmdesc.FrontCounterClockwise = false;

	*tester = generalHolder.d3d11Device->CreateRasterizerState(&cmdesc, &m_CWcullMode);


//
//#pragma region Inits
//	// TODO: PART 1 STEP 3a
//#pragma region SwapChainWindow
//	//swap chain settings
//	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
//
//
//	scd.BufferCount = 1;                                    // one back buffer
//
//	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
//	scd.BufferDesc.Width = BACKBUFFER_WIDTH;
//	scd.BufferDesc.Height = BACKBUFFER_HEIGHT;
//	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
//	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
//	scd.BufferDesc.RefreshRate.Denominator = 1;
//	scd.BufferDesc.RefreshRate.Numerator = 0;
//
//	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
//	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;// | D3D11_CREATE_DEVICE_DEBUG;
//	scd.OutputWindow = window;                              // the window to be used
//	scd.SampleDesc.Count = 2;                               // how many multisamples
//	scd.SampleDesc.Quality = D3D11_STANDARD_MULTISAMPLE_PATTERN;
//	scd.Windowed = TRUE;                                    // windowed/full-screen mode
//	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
//	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
//
//
//	const UINT num_requested_feature_levels = 1;
//
//	// TODO: PART 1 STEP 3b
//	tester = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL,
//		D3D11_CREATE_DEVICE_DEBUG, NULL, NULL, D3D11_SDK_VERSION,
//		&scd, &swap, &dev, NULL, &d3d11DevCon);
//#pragma endregion
//
//
//	// TODO: PART 1 STEP 4
//
//	////get the address of the back buffer
//	ID3D11Texture2D *_BackBuffer = nullptr;
//	tester = swap->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&_BackBuffer));
//	//
//	//use the back buffer address to create the render target
//	tester = dev->CreateRenderTargetView(_BackBuffer, NULL, &backBuffer);
//	_BackBuffer->Release();
//
//	//set the render target as the back buffer
//	d3d11DevCon->OMSetRenderTargets(1, &backBuffer, NULL);
//
//
//	// TODO: PART 1 STEP 5
//	//viewport
//	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
//
//	viewport.MinDepth = 0.0f;
//	viewport.MaxDepth = 1.0f;
//	viewport.TopLeftX = 0;
//	viewport.TopLeftY = 0;
//	viewport.Width = BACKBUFFER_WIDTH;
//	viewport.Height = BACKBUFFER_HEIGHT;
//
//	//set viewport
//	d3d11DevCon->RSSetViewports(1, &viewport);
//
//#pragma endregion


	

}