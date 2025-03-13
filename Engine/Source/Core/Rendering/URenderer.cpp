#include "pch.h" 
#include "URenderer.h"
#include "Core/Rendering/BufferCache.h"
#include "Static/EditorManager.h"
#include "Core/Math/Transform.h"
#include "Engine/GameFrameWork/Camera.h"
#include "CoreUObject/Components/PrimitiveComponent.h"

void URenderer::Create(HWND hWindow)
{
    CreateDeviceAndSwapChain(hWindow);
    CreateFrameBuffer();
    CreateRasterizerState();
    CreateBufferCache();
    CreateDepthStencilBuffer();
    CreateDepthStencilState();

    CreatePickingTexture(hWindow);
    
    InitMatrix();
}

void URenderer::Release()
{
    ReleaseRasterizerState();

	// Reset Render Target
    DeviceContext->OMSetRenderTargets(0, nullptr, DepthStencilView);

    ReleaseFrameBuffer();
    ReleaseDepthStencilBuffer();
    ReleaseDeviceAndSwapChain();
}

void URenderer::CreateShader()
{
    /**
    * �����ϵ� ���̴��� ����Ʈ�ڵ带 ������ ���� (ID3DBlob)
    *
    * ���� �޸� ���۸� ��Ÿ���� ����
    *   - ���⼭�� shader object bytecode�� ������� ����
    * ���� �� �޼��带 �����Ѵ�.
    *   - LPVOID GetBufferPointer
    *     - ���۸� ����Ű�� void* �����͸� �����ش�.
    *   - SIZE_T GetBufferSize
    *     - ������ ũ��(����Ʈ ����)�� �����ش�
    */
    ID3DBlob* VertexShaderCSO;
    ID3DBlob* PixelShaderCSO;

    ID3DBlob* PickingShaderCSO;
    
    ID3DBlob* ErrorMsg = nullptr;

	// Compile Shader //
    D3DCompileFromFile(L"Shaders/ShaderW0.hlsl", nullptr, nullptr, "mainVS", "vs_5_0", 0, 0, &VertexShaderCSO, &ErrorMsg);
    Device->CreateVertexShader(VertexShaderCSO->GetBufferPointer(), VertexShaderCSO->GetBufferSize(), nullptr, &SimpleVertexShader);

    D3DCompileFromFile(L"Shaders/ShaderW0.hlsl", nullptr, nullptr, "mainPS", "ps_5_0", 0, 0, &PixelShaderCSO, &ErrorMsg);
    Device->CreatePixelShader(PixelShaderCSO->GetBufferPointer(), PixelShaderCSO->GetBufferSize(), nullptr, &SimplePixelShader);

    D3DCompileFromFile(L"Shaders/ShaderW0.hlsl", nullptr, nullptr, "PickingPS", "ps_5_0", 0, 0, &PickingShaderCSO, nullptr);
    Device->CreatePixelShader(PickingShaderCSO->GetBufferPointer(), PickingShaderCSO->GetBufferSize(), nullptr, &PickingPixelShader);
    
        if (ErrorMsg)
        {
                std::cout << (char*)ErrorMsg->GetBufferPointer() << std::endl;
                ErrorMsg->Release();
        }

	// Input Layout //
    D3D11_INPUT_ELEMENT_DESC Layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    Device->CreateInputLayout(Layout, ARRAYSIZE(Layout), VertexShaderCSO->GetBufferPointer(), VertexShaderCSO->GetBufferSize(), &SimpleInputLayout);

    VertexShaderCSO->Release();
    PixelShaderCSO->Release();
    PickingShaderCSO->Release();

    // unit byte
    Stride = sizeof(FVertexSimple);
}

void URenderer::ReleaseShader()
{
    if (SimpleInputLayout)
    {
        SimpleInputLayout->Release();
        SimpleInputLayout = nullptr;
    }

    if (SimplePixelShader)
    {
        SimplePixelShader->Release();
        SimplePixelShader = nullptr;
    }

    if (SimpleVertexShader)
    {
        SimpleVertexShader->Release();
        SimpleVertexShader = nullptr;
    }
}

void URenderer::CreateConstantBuffer()
{
    D3D11_BUFFER_DESC ConstantBufferDesc = {};
    ConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;                        // �� ������ CPU���� ������Ʈ �ϱ� ����
    ConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;             // ��� ���۷� ����
    ConstantBufferDesc.ByteWidth = sizeof(FConstants) + 0xf & 0xfffffff0;  // 16byte�� ����� �ø�
    ConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;            // CPU���� ���� ������ �����ϰ� ����

    Device->CreateBuffer(&ConstantBufferDesc, nullptr, &ConstantBuffer);

    D3D11_BUFFER_DESC ConstantBufferDescPicking = {};
    ConstantBufferDescPicking.Usage = D3D11_USAGE_DYNAMIC;                        // �� ������ CPU���� ������Ʈ �ϱ� ����
    ConstantBufferDescPicking.BindFlags = D3D11_BIND_CONSTANT_BUFFER;             // ��� ���۷� ����
    ConstantBufferDescPicking.ByteWidth = sizeof(FPickingConstants) + 0xf & 0xfffffff0;  // 16byte�� ����� �ø�
    ConstantBufferDescPicking.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;            // CPU���� ���� ������ �����ϰ� ����

    Device->CreateBuffer(&ConstantBufferDescPicking, nullptr, &ConstantPickingBuffer);

    D3D11_BUFFER_DESC ConstantBufferDescDepth = {};
    ConstantBufferDescPicking.Usage = D3D11_USAGE_DYNAMIC;                        // �� ������ CPU���� ������Ʈ �ϱ� ����
    ConstantBufferDescPicking.BindFlags = D3D11_BIND_CONSTANT_BUFFER;             // ��� ���۷� ����
    ConstantBufferDescPicking.ByteWidth = sizeof(FDepthConstants) + 0xf & 0xfffffff0;  // 16byte�� ����� �ø�
    ConstantBufferDescPicking.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;            // CPU���� ���� ������ �����ϰ� ����

    Device->CreateBuffer(&ConstantBufferDescPicking, nullptr, &ConstantsDepthBuffer);
}

void URenderer::ReleaseConstantBuffer()
{
    if (ConstantBuffer)
    {
        ConstantBuffer->Release();
        ConstantBuffer = nullptr;
    }

    if (ConstantPickingBuffer)
    {
        ConstantPickingBuffer->Release();
        ConstantPickingBuffer = nullptr;
    }

    if (ConstantsDepthBuffer)
    {
        ConstantsDepthBuffer->Release();
        ConstantsDepthBuffer = nullptr;
    }
}

void URenderer::SwapBuffer() const
{
    SwapChain->Present(1, 0); // SyncInterval: VSync Enable
}

void URenderer::Prepare() const
{
    // Clear Screen
    DeviceContext->ClearRenderTargetView(FrameBufferRTV, ClearColor);
    DeviceContext->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    
    // InputAssembler�� Vertex �ؼ� ����� ����
    DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Rasterization�� Viewport�� ���� 
    DeviceContext->RSSetViewports(1, &ViewportInfo);
    DeviceContext->RSSetState(RasterizerState);

    /**
     * OutputMerger ����
     * ������ ������������ ���� �ܰ�ν�, ��� �׸���(���� Ÿ��)�� ��� �׸���(�����)�� ����
     */
    DeviceContext->OMSetRenderTargets(1, &FrameBufferRTV, DepthStencilView);    // DepthStencil �� ����
    DeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);
}

void URenderer::PrepareShader() const
{
    // �⺻ ���̴��� InputLayout�� ����
    DeviceContext->VSSetShader(SimpleVertexShader, nullptr, 0);
    DeviceContext->PSSetShader(SimplePixelShader, nullptr, 0);
    DeviceContext->IASetInputLayout(SimpleInputLayout);

    // ���ؽ� ���̴��� ��� ���۸� ����
    if (ConstantBuffer)
    {
        DeviceContext->VSSetConstantBuffers(0, 1, &ConstantBuffer);
    }
    if (ConstantsDepthBuffer)
    {
        DeviceContext->PSSetConstantBuffers(2, 1, &ConstantsDepthBuffer);
    }
}

void URenderer::RenderPrimitive(UPrimitiveComponent* PrimitiveComp)
{
    if (BufferCache == nullptr)
    {
        return;
    }

        BufferInfo Info = BufferCache->GetBufferInfo(PrimitiveComp->GetType());

        if (Info.GetBuffer() == nullptr)
        {
                return;
        }

        //if (CurrentTopology != Info.GetTopology())
        {
                DeviceContext->IASetPrimitiveTopology(Info.GetTopology());
                CurrentTopology = Info.GetTopology();
        }

    ConstantUpdateInfo UpdateInfo{ 
        PrimitiveComp->GetWorldTransform(), 
        PrimitiveComp->GetCustomColor(), 
        PrimitiveComp->IsUseVertexColor()
    };

    UpdateConstant(UpdateInfo);
    
    RenderPrimitiveInternal(Info.GetBuffer(), Info.GetSize());

}

void URenderer::RenderPrimitiveInternal(ID3D11Buffer* pBuffer, UINT numVertices) const
{
    UINT Offset = 0;
    DeviceContext->IASetVertexBuffers(0, 1, &pBuffer, &Stride, &Offset);

    DeviceContext->Draw(numVertices, 0);
}

ID3D11Buffer* URenderer::CreateVertexBuffer(const FVertexSimple* Vertices, UINT ByteWidth) const
{
    D3D11_BUFFER_DESC VertexBufferDesc = {};
    VertexBufferDesc.ByteWidth = ByteWidth;
    VertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA VertexBufferSRD = {};
    VertexBufferSRD.pSysMem = Vertices;

    ID3D11Buffer* VertexBuffer;
    const HRESULT Result = Device->CreateBuffer(&VertexBufferDesc, &VertexBufferSRD, &VertexBuffer);
    if (FAILED(Result))
    {
        return nullptr;
    }
    return VertexBuffer;
}

void URenderer::ReleaseVertexBuffer(ID3D11Buffer* pBuffer) const
{
    pBuffer->Release();
}

void URenderer::UpdateConstant(const ConstantUpdateInfo& UpdateInfo) const
{
    if (!ConstantBuffer) return;

    D3D11_MAPPED_SUBRESOURCE ConstantBufferMSR;

    FMatrix MVP = 
        FMatrix::Transpose(ProjectionMatrix) * 
        FMatrix::Transpose(ViewMatrix) * 
        FMatrix::Transpose(UpdateInfo.Transform.GetMatrix());    // ��� ���۸� CPU �޸𸮿� ����

    // D3D11_MAP_WRITE_DISCARD�� ���� ������ �����ϰ� ���ο� �����ͷ� ����� ���� ���
    DeviceContext->Map(ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ConstantBufferMSR);
    {
        // ���ε� �޸𸮸� FConstants ����ü�� ĳ����
        FConstants* Constants = static_cast<FConstants*>(ConstantBufferMSR.pData);
        Constants->MVP = MVP;
        Constants->Color = UpdateInfo.Color;
        Constants->bUseVertexColor = UpdateInfo.bUseVertexColor ? 1 : 0;
    }
    DeviceContext->Unmap(ConstantBuffer, 0);
}


ID3D11Device* URenderer::GetDevice() const
{ return Device; }

ID3D11DeviceContext* URenderer::GetDeviceContext() const
{ return DeviceContext; }

void URenderer::CreateDeviceAndSwapChain(HWND hWindow)
{
    // �����ϴ� Direct3D ��� ������ ����
    D3D_FEATURE_LEVEL FeatureLevels[] = { D3D_FEATURE_LEVEL_11_0 };

    // SwapChain ����ü �ʱ�ȭ
    DXGI_SWAP_CHAIN_DESC SwapChainDesc = {};
    SwapChainDesc.BufferDesc.Width = 0;                            // â ũ�⿡ �°� �ڵ����� ����
    SwapChainDesc.BufferDesc.Height = 0;                           // â ũ�⿡ �°� �ڵ����� ����
    SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // ���� ����
    SwapChainDesc.SampleDesc.Count = 1;                            // ��Ƽ ���ø� ��Ȱ��ȭ
    SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;   // ���� Ÿ������ ����
    SwapChainDesc.BufferCount = 2;                                 // ���� ���۸�
    SwapChainDesc.OutputWindow = hWindow;                          // �������� â �ڵ�
    SwapChainDesc.Windowed = TRUE;                                 // â ���
    SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;      // ���� ���

    // Direct3D Device�� SwapChain�� ����
    D3D11CreateDeviceAndSwapChain(
        // �Է� �Ű�����
        nullptr,                                                       // ����̽��� ���� �� ����� ���� ����Ϳ� ���� ������
        D3D_DRIVER_TYPE_HARDWARE,                                      // ���� ����̹� ������ ��Ÿ���� D3D_DRIVER_TYPE ������ ��
        nullptr,                                                       // ����Ʈ���� �����Ͷ������� �����ϴ� DLL�� ���� �ڵ�
        D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG,  // ����� ��Ÿ�� ������ �����ϴ� D3D11_CREATE_DEVICE_FLAG ������ ������ ����
        FeatureLevels,                                                 // ������� ��� ������ ������ �����ϴ� D3D_FEATURE_LEVEL �迭�� ���� ������
        ARRAYSIZE(FeatureLevels),                                      // pFeatureLevels �迭�� ��� ��
        D3D11_SDK_VERSION,                                             // SDK ����. �ַ� D3D11_SDK_VERSION�� ���
        &SwapChainDesc,                                                // SwapChain ������ ���õ� DXGI_SWAP_CHAIN_DESC ����ü�� ���� ������

        // ��� �Ű�����
        &SwapChain,                                                    // ������ IDXGISwapChain �������̽��� ���� ������
        &Device,                                                       // ������ ID3D11Device �������̽��� ���� ������
        nullptr,                                                       // ���õ� ��� ������ ��Ÿ���� D3D_FEATURE_LEVEL ���� ��ȯ
        &DeviceContext                                                 // ������ ID3D11DeviceContext �������̽��� ���� ������
    );

    // ������ SwapChain�� ���� ��������
    SwapChain->GetDesc(&SwapChainDesc);

    // ����Ʈ ���� ����
    ViewportInfo = {
        0.0f, 0.0f,
        static_cast<float>(SwapChainDesc.BufferDesc.Width), static_cast<float>(SwapChainDesc.BufferDesc.Height),
        0.0f, 1.0f
    };
}

void URenderer::ReleaseDeviceAndSwapChain()
{
    if (DeviceContext)
    {
        DeviceContext->Flush(); // �����ִ� GPU ��� ����
    }

    if (SwapChain)
    {
        SwapChain->Release();
        SwapChain = nullptr;
    }

    if (Device)
    {
        Device->Release();
        Device = nullptr;
    }

    if (DeviceContext)
    {
        DeviceContext->Release();
        DeviceContext = nullptr;
    }
}

void URenderer::CreateFrameBuffer()
{
    // ���� ü�����κ��� �� ���� �ؽ�ó ��������
    SwapChain->GetBuffer(0, IID_PPV_ARGS(&FrameBuffer));

    // ���� Ÿ�� �� ����
    D3D11_RENDER_TARGET_VIEW_DESC FrameBufferRTVDesc = {};
    FrameBufferRTVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;      // ���� ����
    FrameBufferRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D; // 2D �ؽ�ó

    Device->CreateRenderTargetView(FrameBuffer, &FrameBufferRTVDesc, &FrameBufferRTV);
}

void URenderer::CreateDepthStencilBuffer()
{
    D3D11_TEXTURE2D_DESC DepthBufferDesc = {};
    DepthBufferDesc.Width = static_cast<UINT>(ViewportInfo.Width);
    DepthBufferDesc.Height = static_cast<UINT>(ViewportInfo.Height);
    DepthBufferDesc.MipLevels = 1;
    DepthBufferDesc.ArraySize = 1;
    DepthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;            // 32��Ʈ �� 24��Ʈ�� ����, 8��Ʈ�� ���ٽ�
    DepthBufferDesc.SampleDesc.Count = 1;
    DepthBufferDesc.SampleDesc.Quality = 0;
    DepthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    DepthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;              // �ؽ��� ���ε� �÷��׸� DepthStencil�� ����
    DepthBufferDesc.CPUAccessFlags = 0;
    DepthBufferDesc.MiscFlags = 0;

    HRESULT result = Device->CreateTexture2D(&DepthBufferDesc, nullptr, &DepthStencilBuffer);

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = DepthBufferDesc.Format;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;

    result = Device->CreateDepthStencilView(DepthStencilBuffer, &dsvDesc, &DepthStencilView);
}

void URenderer::CreateDepthStencilState()
{
    D3D11_DEPTH_STENCIL_DESC DepthStencilDesc = {};
    DepthStencilDesc.DepthEnable = TRUE;
    DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;                     // �� ���� ���̰��� ���� �� �ȼ��� ������

    Device->CreateDepthStencilState(&DepthStencilDesc, &DepthStencilState);
    
    D3D11_DEPTH_STENCIL_DESC IgnoreDepthStencilDesc = {};
    DepthStencilDesc.DepthEnable = TRUE;
    DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    DepthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;                     
    Device->CreateDepthStencilState(&IgnoreDepthStencilDesc ,&IgnoreDepthStencilState);
}

void URenderer::ReleaseFrameBuffer()
{
    if (FrameBuffer)
    {
        FrameBuffer->Release();
        FrameBuffer = nullptr;
    }

    if (FrameBufferRTV)
    {
        FrameBufferRTV->Release();
        FrameBufferRTV = nullptr;
    }

    if (PickingFrameBuffer)
    {
                PickingFrameBuffer->Release();
                PickingFrameBuffer = nullptr;
    }

    if (PickingFrameBufferRTV)
    {
                PickingFrameBufferRTV->Release();
                PickingFrameBufferRTV = nullptr;
    }
}

void URenderer::ReleaseDepthStencilBuffer()
{
    if (DepthStencilBuffer)
    {
        DepthStencilBuffer->Release();
        DepthStencilBuffer = nullptr;
    }
    if (DepthStencilView)
    {
        DepthStencilView->Release();
        DepthStencilView = nullptr;
    }
    if (DepthStencilState)
    {
        DepthStencilState->Release();
        DepthStencilState = nullptr;
    }
    if (IgnoreDepthStencilState)
    {
        IgnoreDepthStencilState->Release();
        IgnoreDepthStencilState = nullptr;
    }
}

void URenderer::CreateRasterizerState()
{
    D3D11_RASTERIZER_DESC RasterizerDesc = {};
    RasterizerDesc.FillMode = D3D11_FILL_SOLID; // ä��� ���
    RasterizerDesc.CullMode = D3D11_CULL_BACK;  // �� ���̽� �ø�
    RasterizerDesc.FrontCounterClockwise = FALSE;

    Device->CreateRasterizerState(&RasterizerDesc, &RasterizerState);
}

void URenderer::ReleaseRasterizerState()
{
    if (RasterizerState)
    {
        RasterizerState->Release();
        RasterizerState = nullptr;
    }
}

void URenderer::CreateBufferCache()
{
    BufferCache = std::make_unique<FBufferCache>();
}

void URenderer::InitMatrix()
{
        WorldMatrix = FMatrix::Identity;
        ViewMatrix = FMatrix::Identity;
        ProjectionMatrix = FMatrix::Identity;
}

void URenderer::ReleasePickingFrameBuffer()
{
        if (PickingFrameBuffer)
        {
                PickingFrameBuffer->Release();
                PickingFrameBuffer = nullptr;
        }
        if (PickingFrameBufferRTV)
        {
                PickingFrameBufferRTV->Release();
                PickingFrameBufferRTV = nullptr;
        }
}

void URenderer::CreatePickingTexture(HWND hWnd)
{
    RECT Rect;
    int Width , Height;

    Width = ViewportInfo.Width;
        Height = ViewportInfo.Height;

    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = Width;
    textureDesc.Height = Height;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    Device->CreateTexture2D(&textureDesc, nullptr, &PickingFrameBuffer);

    D3D11_RENDER_TARGET_VIEW_DESC PickingFrameBufferRTVDesc = {};
    PickingFrameBufferRTVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;      // ���� ����
    PickingFrameBufferRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D; // 2D �ؽ�ó
    
    Device->CreateRenderTargetView(PickingFrameBuffer, &PickingFrameBufferRTVDesc, &PickingFrameBufferRTV);
}

void URenderer::PrepareZIgnore()
{
    DeviceContext->OMSetDepthStencilState(IgnoreDepthStencilState, 0);
}

void URenderer::PreparePicking()
{
    // ���� Ÿ�� ���ε�
    DeviceContext->OMSetRenderTargets(1, &PickingFrameBufferRTV, DepthStencilView);
    DeviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
    DeviceContext->OMSetDepthStencilState(DepthStencilState, 0);                // DepthStencil ���� ����. StencilRef: ���ٽ� �׽�Ʈ ����� ���۷���

    DeviceContext->ClearRenderTargetView(PickingFrameBufferRTV, PickingClearColor);
}

void URenderer::PreparePickingShader() const
{
    DeviceContext->PSSetShader(PickingPixelShader, nullptr, 0);

    if (ConstantPickingBuffer)
    {
        DeviceContext->PSSetConstantBuffers(1, 1, &ConstantPickingBuffer);
    }
}

void URenderer::UpdateConstantPicking(FVector4 UUIDColor) const
{
    if (!ConstantPickingBuffer) return;

    D3D11_MAPPED_SUBRESOURCE ConstantBufferMSR;

    UUIDColor = FVector4(UUIDColor.X/255.0f, UUIDColor.Y/255.0f, UUIDColor.Z/255.0f, UUIDColor.W/255.0f);
    
    DeviceContext->Map(ConstantPickingBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ConstantBufferMSR);
    {
        FPickingConstants* Constants = static_cast<FPickingConstants*>(ConstantBufferMSR.pData);
        Constants->UUIDColor = UUIDColor;
    }
    DeviceContext->Unmap(ConstantPickingBuffer, 0);
}

void URenderer::UpdateConstantDepth(int Depth) const
{
    if (!ConstantsDepthBuffer) return;

    ACamera* Cam = FEditorManager::Get().GetCamera();
    
    D3D11_MAPPED_SUBRESOURCE ConstantBufferMSR;
    
    DeviceContext->Map(ConstantsDepthBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ConstantBufferMSR);
    {
        FDepthConstants* Constants = static_cast<FDepthConstants*>(ConstantBufferMSR.pData);
        Constants->DepthOffset = Depth;
        Constants->nearPlane = Cam->GetNear();
        Constants->farPlane = Cam->GetFar();
    }
    DeviceContext->Unmap(ConstantsDepthBuffer, 0);
}

void URenderer::PrepareMain()
{
    DeviceContext->OMSetDepthStencilState(DepthStencilState, 0);                // DepthStencil ���� ����. StencilRef: ���ٽ� �׽�Ʈ ����� ���۷���
    DeviceContext->OMSetRenderTargets(1, &FrameBufferRTV, DepthStencilView);
    DeviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
}

void URenderer::PrepareMainShader()
{
    DeviceContext->PSSetShader(SimplePixelShader, nullptr, 0);
}

FVector4 URenderer::GetPixel(FVector MPos)
{
    MPos.X = FMath::Clamp(MPos.X, 0.0f, ViewportInfo.Width);
    MPos.Y = FMath::Clamp(MPos.Y, 0.0f, ViewportInfo.Height);
    // 1. Staging �ؽ�ó ���� (1x1 �ȼ�)
    D3D11_TEXTURE2D_DESC stagingDesc = {};
    stagingDesc.Width = 1; // �ȼ� 1���� ����
    stagingDesc.Height = 1;
    stagingDesc.MipLevels = 1;
    stagingDesc.ArraySize = 1;
    stagingDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // ���� �ؽ�ó ���˰� ����
    stagingDesc.SampleDesc.Count = 1;
    stagingDesc.Usage = D3D11_USAGE_STAGING;
    stagingDesc.BindFlags = 0;
    stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

    ID3D11Texture2D* stagingTexture = nullptr;
    Device->CreateTexture2D(&stagingDesc, nullptr, &stagingTexture);

    // 2. ������ ���� ���� (D3D11_BOX)
    D3D11_BOX srcBox = {};
    srcBox.left = static_cast<UINT>(MPos.X);
    srcBox.right = srcBox.left + 1; // 1�ȼ� �ʺ�
    srcBox.top = static_cast<UINT>(MPos.Y);
    srcBox.bottom = srcBox.top + 1; // 1�ȼ� ����
    srcBox.front = 0;
    srcBox.back = 1;
    FVector4 color{ 1, 1, 1, 1 };

    if (stagingTexture == nullptr)
        return color;

    // 3. Ư�� ��ǥ�� ����
    DeviceContext->CopySubresourceRegion(
        stagingTexture, // ��� �ؽ�ó
        0,              // ��� ���긮�ҽ�
        0, 0, 0,        // ��� ��ǥ (x, y, z)
        PickingFrameBuffer, // ���� �ؽ�ó
        0,              // ���� ���긮�ҽ�
        &srcBox         // ���� ����
    );

    // 4. ������ ����
    D3D11_MAPPED_SUBRESOURCE mapped = {};
    DeviceContext->Map(stagingTexture, 0, D3D11_MAP_READ, 0, &mapped);

    // 5. �ȼ� ������ ���� (1x1 �ؽ�ó�̹Ƿ� offset = 0)
    const BYTE* pixelData = static_cast<const BYTE*>(mapped.pData);

    if (pixelData)
    {
        color.X = static_cast<float>(pixelData[0]); // R
        color.Y = static_cast<float>(pixelData[1]); // G
        color.Z = static_cast<float>(pixelData[2]); // B
        color.W = static_cast<float>(pixelData[3]); // A
    }

    std::cout << "X: " << (int)color.X << " Y: " << (int)color.Y
        << " Z: " << color.Z << " A: " << color.W << "\n";

    // 6. ���� ���� �� ����
    DeviceContext->Unmap(stagingTexture, 0);
    stagingTexture->Release();

    return color;
}

void URenderer::UpdateViewMatrix(const FTransform& CameraTransform)
{
    ViewMatrix = CameraTransform.GetViewMatrix();
}

void URenderer::UpdateProjectionMatrix(ACamera* Camera)
{
    float AspectRatio = UEngine::Get().GetScreenRatio();

    float FOV = FMath::DegreesToRadians(Camera->GetFieldOfView());
    float Near = Camera->GetNear();
    float Far = Camera->GetFar();

    if (Camera->ProjectionMode == ECameraProjectionMode::Perspective)
    {
        ProjectionMatrix = FMatrix::PerspectiveFovLH(FOV, AspectRatio, Near, Far);
    }
    else if (Camera->ProjectionMode == ECameraProjectionMode::Orthographic)
    {
		//@TODO: Delete Magic Number '720p'
        ProjectionMatrix = FMatrix::OrthoLH((float)UEngine::Get().GetScreenWidth() / 720, (float)UEngine::Get().GetScreenHeight() / 720, Near, Far);
    }
}

void URenderer::OnUpdateWindowSize(int Width, int Height)
{
    if (SwapChain)
    {
        SwapChain->ResizeBuffers(0, Width, Height, DXGI_FORMAT_B8G8R8A8_UNORM, 0);

        DXGI_SWAP_CHAIN_DESC SwapChainDesc;
        SwapChain->GetDesc(&SwapChainDesc);
        ViewportInfo = {
            0.0f, 0.0f,
            static_cast<float>(SwapChainDesc.BufferDesc.Width), static_cast<float>(SwapChainDesc.BufferDesc.Height),
            0.0f, 1.0f
        };

        ReleaseFrameBuffer();
        CreateFrameBuffer();

        ReleasePickingFrameBuffer();
        CreatePickingTexture(UEngine::Get().GetWindowHandle());

        ReleaseDepthStencilBuffer();
        CreateDepthStencilBuffer();
    }
}

void URenderer::GetPrimitiveLocalBounds(EPrimitiveType Type, FVector& OutMin, FVector& OutMax)
{
    BufferInfo Info = BufferCache->GetBufferInfo(Type);
    if (Info.GetBuffer() == nullptr)
    {
        return;
    }
    OutMin = Info.GetMin();
    OutMax = Info.GetMax();
}

void URenderer::RenderPickingTexture()
{
	// Copy the picking texture to the back buffer
    ID3D11Texture2D* backBuffer;
    SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
    DeviceContext->CopyResource(backBuffer, PickingFrameBuffer);
    backBuffer->Release();
}
