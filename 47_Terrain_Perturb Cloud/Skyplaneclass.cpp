#include "SkyPlaneClass.h"

SkyPlaneClass::SkyPlaneClass()
{
    m_skyPlane = 0;
    m_vertexBuffer = 0;
    m_indexBuffer = 0;
    m_CloudTexture = 0;
    m_PerturbTexture = 0;
}

SkyPlaneClass::SkyPlaneClass(const SkyPlaneClass&)
{
}

SkyPlaneClass::~SkyPlaneClass()
{
}

// Initialize() 에서는 SkyPlane의 geometry를 구성하기 위한 파라미터를 설정한다. 
bool SkyPlaneClass::Initialize(ID3D11Device* device, WCHAR* cloudTextureFilename, WCHAR* perturbTextureFilename)
{
    // skyPlaneResolution : 평면이 x, z 방향으로 구성되어야하는 쿼드의 수를 표시, 이값을 늘리면 높은 폴리곤을 통해 더 부드러운 효과를 줄 수 있다.
    // textureRepeat : 평면에서 텍스처를 반복할 횟수, UV 좌표를 생성하는데 사용, 단일 텍스처만 적용하면 평면이 너무 픽셀화 되어보이기 때문에 적절히 텍스처를 반복해서 매핑할 필요가 있다.
    // skyPlaneWidth, skyPlaneTop, skyPlaneBottom :  평면의 길이, 곡선 평면의 높이와 밑면
    // 평면의 아래쪽 네 꼭짓점은 skyPlaneBottom에 있고, 평면의 중심은 skyPlaneTop에 있다. 모든 점은 이 두 값 사이에서 보간된다.
    int skyPlaneResolution, textureRepeat;
    float skyPlaneWidth, skyPlaneTop, skyPlaneBottom;
    bool result;

    // Set the sky plane parameters.
    skyPlaneResolution = 10;
    skyPlaneWidth = 10.0f;
    skyPlaneTop = 0.5f;
    skyPlaneBottom = 0.0f;
    textureRepeat = 4;

    // 밝기는 구름을 더 사실적으로 보이게 할 수 있다. 밝기 값은 구름의 흰색을 낮추어 실제 구름처럼 희미하게 보이게 할 수 있다.

    // Set the sky plane shader related parameters.
    m_scale = 0.3f;
    m_brightness = 0.65f;

    // Initialize the translation to zero.
    m_translation = 0.0f;

    // 모든 값이 설정되고 나면 sky plane을 만들고 정점 및 인덱스 버퍼를 로드한다. 
        // Create the sky plane.
    result = InitializeSkyPlane(skyPlaneResolution, skyPlaneWidth, skyPlaneTop, skyPlaneBottom, textureRepeat);
    if (!result)
        return false;

    // Create the vertex and index buffer for the sky plane.
    result = InitializeBuffers(device, skyPlaneResolution);
    if (!result)
        return false;

    // Load the sky plane textures.
    result = LoadTextures(device, cloudTextureFilename, perturbTextureFilename);
    if (!result)
        return false;

    return true;
}

void SkyPlaneClass::Shutdown()
{
    // Release the sky plane textures.
    ReleaseTextures();

    // Release the vertex and index buffer that were used for rendering the sky plane.
    ShutdownBuffers();

    // Release the sky plane array.
    ShutdownSkyPlane();

    return;
}

void SkyPlaneClass::Render(ID3D11DeviceContext* deviceContext)
{
    // Render the sky plane.
    RenderBuffers(deviceContext);

    return;
}

// Frame() 은 cloud bitmap이 프레임마다 설정한 속도값으로 변환하기위한 함수다. 
void SkyPlaneClass::Frame()
{
    // Increment the texture translation value each frame.
    m_translation += 0.0001f;
    if (m_translation > 1.0f)
    {
        m_translation -= 1.0f;
    }

    return;
}

int SkyPlaneClass::GetIndexCount()
{
    return m_indexCount;
}

ID3D11ShaderResourceView* SkyPlaneClass::GetCloudTexture()
{
    return m_CloudTexture->GetTexture();
}

ID3D11ShaderResourceView* SkyPlaneClass::GetPerturbTexture()
{
    return m_PerturbTexture->GetTexture();
}

float SkyPlaneClass::GetScale()
{
    return m_scale;
}

float SkyPlaneClass::GetBrightness()
{
    return m_brightness;
}

float SkyPlaneClass::GetTranslation()
{
    return m_translation;
}

// InitializeSkyPlane() 은 Initialize()에서 초기화한 Sky Plane의 설정값으로 구부러진 sky plane geometry를 만드는 함수다.
bool SkyPlaneClass::InitializeSkyPlane(int skyPlaneResolution, float skyPlaneWidth, float skyPlaneTop, float skyPlaneBottom, int textureRepeat)
{
    float quadSize, radius, constant, textureDelta;
    int i, j, index;
    float positionX, positionY, positionZ, tu, tv;

    // Create the array to hold the sky plane coordinates.
    m_skyPlane = new SkyPlaneType[(skyPlaneResolution + 1) * (skyPlaneResolution + 1)];
    if (!m_skyPlane)
        return false;

    // Determine the size of each quad on the sky plane.
    quadSize = skyPlaneWidth / (float)skyPlaneResolution;

    // Calculate the radius of the sky plane based on the width.
    radius = skyPlaneWidth / 2.0f;

    // Calculate the height constant to increment by.
    constant = (skyPlaneTop - skyPlaneBottom) / (radius * radius);

    // Calculate the texture coordinate increment value.
    textureDelta = (float)textureRepeat / (float)skyPlaneResolution;

    // Loop through the sky plane and build the coordinates based on the increment values given.
    for (j = 0; j <= skyPlaneResolution; j++)
    {
        for (i = 0; i <= skyPlaneResolution; i++)
        {
            // Calculate the vertex coordinates.
            positionX = (-0.5f * skyPlaneWidth) + ((float)i * quadSize);
            positionZ = (-0.5f * skyPlaneWidth) + ((float)j * quadSize);
            positionY = skyPlaneTop - (constant * ((positionX * positionX) + (positionZ * positionZ)));

            // Calculate the texture coordinates.
            tu = (float)i * textureDelta;
            tv = (float)j * textureDelta;

            // Calculate the index into the sky plane array to add this coordinate.
            index = j * (skyPlaneResolution + 1) + i;

            // Add the coordinates to the sky plane array.
            m_skyPlane[index].x = positionX;
            m_skyPlane[index].y = positionY;
            m_skyPlane[index].z = positionZ;
            m_skyPlane[index].tu = tu;
            m_skyPlane[index].tv = tv;
        }
    }

    return true;
}

void SkyPlaneClass::ShutdownSkyPlane()
{
    // Release the sky plane array.
    if (m_skyPlane)
    {
        delete[] m_skyPlane;
        m_skyPlane = 0;
    }

    return;
}

bool SkyPlaneClass::InitializeBuffers(ID3D11Device* device, int skyPlaneResolution)
{
    VertexType* vertices;
    unsigned long* indices;
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
    HRESULT result;
    int i, j, index, index1, index2, index3, index4;


    // Calculate the number of vertices in the sky plane mesh.
    m_vertexCount = (skyPlaneResolution + 1) * (skyPlaneResolution + 1) * 6;

    // Set the index count to the same as the vertex count.
    m_indexCount = m_vertexCount;

    // Create the vertex array.
    vertices = new VertexType[m_vertexCount];
    if (!vertices)
    {
        return false;
    }

    // Create the index array.
    indices = new unsigned long[m_indexCount];
    if (!indices)
    {
        return false;
    }

    // Initialize the index into the vertex array.
    index = 0;

    // Load the vertex and index array with the sky plane array data.
    for (j = 0; j < skyPlaneResolution; j++)
    {
        for (i = 0; i < skyPlaneResolution; i++)
        {
            index1 = j * (skyPlaneResolution + 1) + i;
            index2 = j * (skyPlaneResolution + 1) + (i + 1);
            index3 = (j + 1) * (skyPlaneResolution + 1) + i;
            index4 = (j + 1) * (skyPlaneResolution + 1) + (i + 1);

            // Triangle 1 - Upper Left
            vertices[index].position = XMFLOAT3(m_skyPlane[index1].x, m_skyPlane[index1].y, m_skyPlane[index1].z);
            vertices[index].texture = XMFLOAT2(m_skyPlane[index1].tu, m_skyPlane[index1].tv);
            indices[index] = index;
            index++;

            // Triangle 1 - Upper Right
            vertices[index].position = XMFLOAT3(m_skyPlane[index2].x, m_skyPlane[index2].y, m_skyPlane[index2].z);
            vertices[index].texture = XMFLOAT2(m_skyPlane[index2].tu, m_skyPlane[index2].tv);
            indices[index] = index;
            index++;

            // Triangle 1 - Bottom Left
            vertices[index].position = XMFLOAT3(m_skyPlane[index3].x, m_skyPlane[index3].y, m_skyPlane[index3].z);
            vertices[index].texture = XMFLOAT2(m_skyPlane[index3].tu, m_skyPlane[index3].tv);
            indices[index] = index;
            index++;

            // Triangle 2 - Bottom Left
            vertices[index].position = XMFLOAT3(m_skyPlane[index3].x, m_skyPlane[index3].y, m_skyPlane[index3].z);
            vertices[index].texture = XMFLOAT2(m_skyPlane[index3].tu, m_skyPlane[index3].tv);
            indices[index] = index;
            index++;

            // Triangle 2 - Upper Right
            vertices[index].position = XMFLOAT3(m_skyPlane[index2].x, m_skyPlane[index2].y, m_skyPlane[index2].z);
            vertices[index].texture = XMFLOAT2(m_skyPlane[index2].tu, m_skyPlane[index2].tv);
            indices[index] = index;
            index++;

            // Triangle 2 - Bottom Right
            vertices[index].position = XMFLOAT3(m_skyPlane[index4].x, m_skyPlane[index4].y, m_skyPlane[index4].z);
            vertices[index].texture = XMFLOAT2(m_skyPlane[index4].tu, m_skyPlane[index4].tv);
            indices[index] = index;
            index++;
        }
    }

    // Set up the description of the vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    // Now finally create the vertex buffer.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
    if (FAILED(result))
    {
        return false;
    }

    // Set up the description of the index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the index data.
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    // Create the index buffer.
    result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
    if (FAILED(result))
    {
        return false;
    }

    // Release the arrays now that the vertex and index buffers have been created and loaded.
    delete[] vertices;
    vertices = 0;

    delete[] indices;
    indices = 0;

    return true;
}

void SkyPlaneClass::ShutdownBuffers()
{
    // Release the index buffer.
    if (m_indexBuffer)
    {
        m_indexBuffer->Release();
        m_indexBuffer = 0;
    }

    // Release the vertex buffer.
    if (m_vertexBuffer)
    {
        m_vertexBuffer->Release();
        m_vertexBuffer = 0;
    }

    return;
}

void SkyPlaneClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
    unsigned int stride;
    unsigned int offset;


    // Set vertex buffer stride and offset.
    stride = sizeof(VertexType);
    offset = 0;

    // Set the vertex buffer to active in the input assembler so it can be rendered.
    deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
    deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    return;
}

bool SkyPlaneClass::LoadTextures(ID3D11Device* device, WCHAR* textureFilename1, WCHAR* textureFilename2)
{
    bool result;

    // Create the cloud texture object.
    m_CloudTexture = new TextureClass;
    if (!m_CloudTexture)
    {
        return false;
    }

    // Initialize the cloud texture object.
    result = m_CloudTexture->Initialize(device, textureFilename1);
    if (!result)
    {
        return false;
    }

    // Create the perturb texture object.
    m_PerturbTexture = new TextureClass;
    if (!m_PerturbTexture)
    {
        return false;
    }

    // Initialize the perturb texture object.
    result = m_PerturbTexture->Initialize(device, textureFilename2);
    if (!result)
    {
        return false;
    }

    return true;
}

void SkyPlaneClass::ReleaseTextures()
{
    // Release the texture objects.
    if (m_PerturbTexture)
    {
        m_PerturbTexture->Shutdown();
        delete m_PerturbTexture;
        m_PerturbTexture = 0;
    }

    if (m_CloudTexture)
    {
        m_CloudTexture->Shutdown();
        delete m_CloudTexture;
        m_CloudTexture = 0;
    }

    return;
}
