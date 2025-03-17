#include "pch.h" 
#include "URenderer.h"
#include "Static/EditorManager.h"
#include "Core/Math/Transform.h"
#include "Engine/GameFrameWork/Camera.h"
#include "CoreUObject/Components/PrimitiveComponent.h"


void URenderer::Create(HWND hWindow)
{
    hWnd = hWindow;
    CreateDeviceAndSwapChain(hWindow);
    CreateFrameBuffer();
    CreateRasterizerState();
    CreateBufferCache();
    CreateShaderCache();
    CreateDepthStencilBuffer();
    CreateDepthStencilState();
    CreateBlendState();
    CreatePickingFrameBuffer();

    CreateTextureBuffer();
    CreateTextureSamplerState();

    AdjustDebugLineVertexBuffer(DebugLineNumStep);
    InitMatrix();
}

void URenderer::Release()
{
    ReleaseRasterizerState();

    // Reset Render Target
    DeviceContext->OMSetRenderTargets(0, nullptr, DepthStencilView);

    ReleaseFrameBuffer();
    ReleaseDepthStencilResources();
    ReleaseDeviceAndSwapChain();

    if (debugDevice)
    {
        debugDevice->Release();
    }
}

void URenderer::CreateShader()
{
    if (ShaderCache == nullptr)
    {
        return;
    }

	ShaderCache->CreateShaders(ShaderCache->GetShaderNames(L"Shaders"));

    // unit byte
    Stride = sizeof(FVertexSimple);
    GridStride = sizeof(FVertexGrid);
}

void URenderer::ReleaseShader()
{

}

void URenderer::CreateConstantBuffer()
{
    HRESULT hr = S_OK;
    
    D3D11_BUFFER_DESC DynamicConstantBufferDesc = {};
    DynamicConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;                        // 매 프레임 CPU에서 업데이트 하기 위해
    DynamicConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;             // 상수 버퍼로 설정
    DynamicConstantBufferDesc.ByteWidth = sizeof(FCbChangeEveryObject) + 0xf & 0xfffffff0;  // 16byte의 배수로 올림
    DynamicConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;            // CPU에서 쓰기 접근이 가능하게 설정
    hr = Device->CreateBuffer(&DynamicConstantBufferDesc, nullptr, &CbChangeEveryObject);
    if (FAILED(hr))
        return;

    DynamicConstantBufferDesc.ByteWidth = sizeof(FCbChangeEveryFrame) + 0xf & 0xfffffff0;  // 16byte의 배수로 올림
    hr = Device->CreateBuffer(&DynamicConstantBufferDesc, nullptr, &CbChangeEveryFrame);
    if (FAILED(hr))
        return;
    
    D3D11_BUFFER_DESC DefaultConstantBufferDesc = {};
    DefaultConstantBufferDesc.Usage = D3D11_USAGE_DEFAULT;                        // 특정 상황에만 CPU에서 업데이트 하기 위해
    DefaultConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;             // 상수 버퍼로 설정
    DefaultConstantBufferDesc.ByteWidth = sizeof(FCbChangeOnResizeAndFov) + 0xf & 0xfffffff0;  // 16byte의 배수로 올림
    DefaultConstantBufferDesc.CPUAccessFlags = 0;                                 // CPU에서 접근 불가능
    hr = Device->CreateBuffer(&DefaultConstantBufferDesc, nullptr, &CbChangeOnResizeAndFov);
    if (FAILED(hr))
        return;
    
    D3D11_BUFFER_DESC ConstantBufferDescPicking = {};
    ConstantBufferDescPicking.Usage = D3D11_USAGE_DYNAMIC;                        // 매 프레임 CPU에서 업데이트 하기 위해
    ConstantBufferDescPicking.BindFlags = D3D11_BIND_CONSTANT_BUFFER;             // 상수 버퍼로 설정
    ConstantBufferDescPicking.ByteWidth = sizeof(FPickingConstants) + 0xf & 0xfffffff0;  // 16byte의 배수로 올림
    ConstantBufferDescPicking.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;            // CPU에서 쓰기 접근이 가능하게 설정
    hr = Device->CreateBuffer(&ConstantBufferDescPicking, nullptr, &ConstantPickingBuffer);
    if (FAILED(hr))
        return;
    
    D3D11_BUFFER_DESC ConstantBufferDescDepth = {};
    ConstantBufferDescDepth.Usage = D3D11_USAGE_DYNAMIC;                        // 매 프레임 CPU에서 업데이트 하기 위해
    ConstantBufferDescDepth.BindFlags = D3D11_BIND_CONSTANT_BUFFER;             // 상수 버퍼로 설정
    ConstantBufferDescDepth.ByteWidth = sizeof(FDepthConstants) + 0xf & 0xfffffff0;  // 16byte의 배수로 올림
    ConstantBufferDescDepth.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;            // CPU에서 쓰기 접근이 가능하게 설정
    hr = Device->CreateBuffer(&ConstantBufferDescDepth, nullptr, &ConstantsDepthBuffer);
    if (FAILED(hr))
        return;

    D3D11_BUFFER_DESC TextureConstantBufferDesc = {};
    TextureConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    TextureConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    TextureConstantBufferDesc.ByteWidth = sizeof(FTextureConstants) + 0xf & 0xfffffff0;
    TextureConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    hr = Device->CreateBuffer(&TextureConstantBufferDesc, nullptr, &TextureConstantBuffer);
    if (FAILED(hr))
        return;
    
    /**
     * 여기에서 상수 버퍼를 쉐이더에 바인딩.
     * 현재는 각각 다른 레지스터에 바인딩 하므로 겹치지 않고 구분됨.
     * 따라서 초기화 단계에서 모두 바인딩.
     */
    DeviceContext->VSSetConstantBuffers(0, 1, &CbChangeEveryObject);
    DeviceContext->VSSetConstantBuffers(1, 1, &CbChangeEveryFrame);
    DeviceContext->VSSetConstantBuffers(2, 1, &CbChangeOnResizeAndFov);
    DeviceContext->VSSetConstantBuffers(5, 1, &TextureConstantBuffer);

    DeviceContext->PSSetConstantBuffers(1, 1, &CbChangeEveryFrame);
    DeviceContext->PSSetConstantBuffers(2, 1, &CbChangeOnResizeAndFov);
    DeviceContext->PSSetConstantBuffers(3, 1, &ConstantPickingBuffer);
    DeviceContext->PSSetConstantBuffers(4, 1, &ConstantsDepthBuffer);
}

void URenderer::ReleaseConstantBuffer()
{
    if (CbChangeEveryObject)
    {
        CbChangeEveryObject->Release();
        CbChangeEveryObject = nullptr;
    }

    if (CbChangeEveryFrame)
    {
        CbChangeEveryFrame->Release();
        CbChangeEveryFrame = nullptr;
    }

    if (CbChangeOnResizeAndFov)
    {
        CbChangeOnResizeAndFov->Release();
        CbChangeOnResizeAndFov = nullptr;
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

void URenderer::SwapBuffer()
{
    HRESULT hr = SwapChain->Present(1, 0); // SyncInterval: VSync Enable
	bSwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
}

void URenderer::PrepareRender()
{
    // Clear Screen
	DeviceContext->ClearRenderTargetView(FrameBufferRTV, ClearColor);
    DeviceContext->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    // InputAssembler의 Vertex 해석 방식을 설정
    DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Rasterization할 Viewport를 설정 
    DeviceContext->RSSetViewports(1, &ViewportInfo);


    switch (CurrentRasterizerStateType)
    {
    case EViewModeIndex::ERS_Solid:
        CurrentRasterizerState = &RasterizerState_Solid;
        break;
    case EViewModeIndex::ERS_Wireframe:
        CurrentRasterizerState = &RasterizerState_Wireframe;
        break;
    default:
        break;
    }

    DeviceContext->RSSetState(*CurrentRasterizerState);

    /**
     * OutputMerger 설정
     * 렌더링 파이프라인의 최종 단계로써, 어디에 그릴지(렌더 타겟)와 어떻게 그릴지(블렌딩)를 지정
     */
    DeviceContext->OMSetRenderTargets(1, &FrameBufferRTV, DepthStencilView);    // DepthStencil 뷰 설정
    DeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);
}

void URenderer::PrepareShader() const
{
    // 기본 셰이더랑 InputLayout을 설정
	ID3D11VertexShader* MainVertexShader = ShaderCache->GetVertexShader(L"ShaderMain");
	ID3D11PixelShader* MainPixelShader = ShaderCache->GetPixelShader(L"ShaderMain");
	ID3D11InputLayout* MainInputLayout = ShaderCache->GetInputLayout(L"ShaderMain");
    DeviceContext->VSSetShader(MainVertexShader, nullptr, 0);
    DeviceContext->PSSetShader(MainPixelShader, nullptr, 0);
    DeviceContext->IASetInputLayout(MainInputLayout);
}

void URenderer::RenderPrimitive(UPrimitiveComponent* PrimitiveComp)
{
    if (BufferCache == nullptr)
    {
        return;
    }

    // 버텍스
    FVertexBufferInfo VertexInfo = BufferCache->GetVertexBufferInfo(PrimitiveComp->GetType());

    if (VertexInfo.GetVertexBuffer() == nullptr)
    {
        return;
    }

    auto Topology = VertexInfo.GetTopology();
    {
        DeviceContext->IASetPrimitiveTopology(Topology);
        CurrentTopology = Topology;
    }

    ConstantUpdateInfo UpdateInfo{ 
        PrimitiveComp->GetWorldTransform().GetMatrix(),
        PrimitiveComp->GetCustomColor(), 
        PrimitiveComp->IsUseVertexColor()
    };

    UpdateObjectConstantBuffer(UpdateInfo);

    // 인덱스
	FIndexBufferInfo IndexInfo = BufferCache->GetIndexBufferInfo(PrimitiveComp->GetType());

	ID3D11Buffer* VertexBuffer = VertexInfo.GetVertexBuffer();
	ID3D11Buffer* IndexBuffer = IndexInfo.GetIndexBuffer();

	int Size = IndexBuffer ? IndexInfo.GetSize() : VertexInfo.GetSize();

    RenderPrimitiveInternal(VertexBuffer, IndexBuffer, Size);

}

void URenderer::RenderPrimitiveInternal(ID3D11Buffer* VertexBuffer, ID3D11Buffer* IndexBuffer, UINT Size) const
{
    UINT Offset = 0;
    DeviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, &Stride, &Offset);

    if (!IndexBuffer)
    {
        DeviceContext->Draw(Size, 0);
    }
    else
	{
		DeviceContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		DeviceContext->DrawIndexed(Size, 0, 0);
	}
}

void URenderer::RenderBox(const FBox& Box, const FVector4& Color)
{
    // 월드변환이 이미 돼있다
    ConstantUpdateInfo UpdateInfo
    {
        FMatrix::Identity,
        Color,
        false,
    };

    UpdateObjectConstantBuffer(UpdateInfo);

    DeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

    // Box의 Min, Max를 이용해 버텍스 배열 생성
    FVertexSimple BoxVertices[8] =
    {
        {Box.Min.X, Box.Min.Y, Box.Min.Z, 1.0f, 1.0f, 1.0f, 1.0f},
        {Box.Min.X, Box.Min.Y, Box.Max.Z, 1.0f, 1.0f, 1.0f, 1.0f},
        {Box.Min.X, Box.Max.Y, Box.Min.Z, 1.0f, 1.0f, 1.0f, 1.0f},
        {Box.Min.X, Box.Max.Y, Box.Max.Z, 1.0f, 1.0f, 1.0f, 1.0f},
        {Box.Max.X, Box.Min.Y, Box.Min.Z, 1.0f, 1.0f, 1.0f, 1.0f},
        {Box.Max.X, Box.Min.Y, Box.Max.Z, 1.0f, 1.0f, 1.0f, 1.0f},
        {Box.Max.X, Box.Max.Y, Box.Min.Z, 1.0f, 1.0f, 1.0f, 1.0f},
        {Box.Max.X, Box.Max.Y, Box.Max.Z, 1.0f, 1.0f, 1.0f, 1.0f},
    };

	if (DynamicVertexBuffer == nullptr)
        DynamicVertexBuffer = CreateDynamicVertexBuffer(sizeof(FVertexSimple) * 8);


    UINT Stride = sizeof(FVertexSimple);
    UINT Offset = 0;

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	HRESULT hr = DeviceContext->Map(DynamicVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);

	memcpy(MappedResource.pData, BoxVertices, sizeof(FVertexSimple) * 8);

	DeviceContext->Unmap(DynamicVertexBuffer, 0);

	FIndexBufferInfo IndexBufferInfo = BufferCache->GetIndexBufferInfo(EPrimitiveType::EPT_Cube);
	ID3D11Buffer* IndexBuffer = IndexBufferInfo.GetIndexBuffer();

    DeviceContext->IASetVertexBuffers(0, 1, &DynamicVertexBuffer, &Stride, &Offset);
    DeviceContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    DeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

    DeviceContext->DrawIndexed(IndexBufferInfo.GetSize(), 0, 0);
}

void URenderer::PrepareWorldGrid()
{
    UINT Offset = 0;
    DeviceContext->IASetVertexBuffers(0, 1, &GridVertexBuffer, &GridStride, &Offset);
    
    DeviceContext->OMSetDepthStencilState(DepthStencilState, 0);
    DeviceContext->OMSetRenderTargets(1, &FrameBufferRTV, DepthStencilView);
    DeviceContext->OMSetBlendState(GridBlendState, nullptr, 0xFFFFFFFF);
    
    DeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	ID3D11VertexShader* ShaderGridVS = ShaderCache->GetVertexShader(L"ShaderGrid");
	ID3D11PixelShader* ShaderGridPS = ShaderCache->GetPixelShader(L"ShaderGrid");
	ID3D11InputLayout* InputLayoutGrid = ShaderCache->GetInputLayout(L"ShaderGrid");

	DeviceContext->VSSetShader(ShaderGridVS, nullptr, 0);
	DeviceContext->PSSetShader(ShaderGridPS, nullptr, 0);
	DeviceContext->IASetInputLayout(InputLayoutGrid);
}

void URenderer::RenderWorldGrid()
{
    PrepareWorldGrid();
    
    AActor* CameraActor = FEditorManager::Get().GetCamera();
    if (CameraActor == nullptr)
    {
        return;
    }
    
    float GridGap = UEngine::Get().GetWorldGridGap();

    FTransform CameraTransform = CameraActor->GetActorTransform();
    FVector CameraLocation = CameraTransform.GetPosition();

    int32 StepX = static_cast<int32>(CameraLocation.X / GridGap);
    int32 StepY = static_cast<int32>(CameraLocation.Y / GridGap);

    FVector GridOffset(StepX * GridGap, StepY * GridGap, 0.f);
    FVector GridScale(GridGap, GridGap, 1.f);

    FTransform GridTransform(GridOffset, FVector::ZeroVector, GridScale);
    
    ConstantUpdateInfo UpdateInfo
    {
        GridTransform.GetMatrix(),
        FVector4(0.2f, 0.2f, 0.2f, 1.0f),
        false,
    };

    UpdateObjectConstantBuffer(UpdateInfo);

    DeviceContext->Draw(GridVertexNum, 0);

    // restore
    DeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    // 나머지는 PrepareMainShader에서 작업중이므로, 생략
}

ID3D11Buffer* URenderer::CreateImmutableVertexBuffer(const FVertexSimple* Vertices, UINT ByteWidth) const
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

ID3D11Buffer* URenderer::CreateDynamicVertexBuffer(UINT ByteWidth)
{
	D3D11_BUFFER_DESC VertexBufferDesc = {};
	VertexBufferDesc.ByteWidth = ByteWidth;
	VertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    ID3D11Buffer* VertexBuffer = nullptr;
	const HRESULT Result = Device->CreateBuffer(&VertexBufferDesc, nullptr, &VertexBuffer);
	if (FAILED(Result))
	{
		return nullptr;
	}
	return VertexBuffer;
}

ID3D11Buffer* URenderer::CreateIndexBuffer(const UINT* Indices, UINT ByteWidth) const
{
	D3D11_BUFFER_DESC IndexBufferDesc = {};
	IndexBufferDesc.ByteWidth = ByteWidth;
	IndexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	D3D11_SUBRESOURCE_DATA IndexBufferSRD = {};
	IndexBufferSRD.pSysMem = Indices;

	ID3D11Buffer* IndexBuffer;
	const HRESULT Result = Device->CreateBuffer(&IndexBufferDesc, &IndexBufferSRD, &IndexBuffer);
	if (FAILED(Result))
	{
		return nullptr;
	}
	return IndexBuffer;
}

void URenderer::ReleaseVertexBuffer(ID3D11Buffer* pBuffer) const
{
    pBuffer->Release();
}

void URenderer::UpdateObjectConstantBuffer(const ConstantUpdateInfo& UpdateInfo) const
{
    D3D11_MAPPED_SUBRESOURCE ConstantBufferMSR;

    // D3D11_MAP_WRITE_DISCARD는 이전 내용을 무시하고 새로운 데이터로 덮어쓰기 위해 사용
    DeviceContext->Map(CbChangeEveryObject, 0, D3D11_MAP_WRITE_DISCARD, 0, &ConstantBufferMSR);
    // 매핑된 메모리를 FConstants 구조체로 캐스팅
    if (FCbChangeEveryObject* Constants = static_cast<FCbChangeEveryObject*>(ConstantBufferMSR.pData))
    {
        Constants->WorldMatrix = FMatrix::Transpose(UpdateInfo.TransformMatrix);
        Constants->CustomColor = UpdateInfo.Color;
        Constants->bUseVertexColor = UpdateInfo.bUseVertexColor ? 1 : 0;
    }
    // UnMap해서 GPU에 값이 전달 될 수 있게 함
    DeviceContext->Unmap(CbChangeEveryObject, 0);
}

ID3D11Device* URenderer::GetDevice() const
{
    return Device;
}

ID3D11DeviceContext* URenderer::GetDeviceContext() const
{
    return DeviceContext;
}

void URenderer::CreateDeviceAndSwapChain(HWND hWindow)
{
    // 지원하는 Direct3D 기능 레벨을 정의
    D3D_FEATURE_LEVEL FeatureLevels[] = { D3D_FEATURE_LEVEL_11_0 };

    // SwapChain 구조체 초기화
    DXGI_SWAP_CHAIN_DESC SwapChainDesc = {};
    SwapChainDesc.BufferDesc.Width = 0;                            // 창 크기에 맞게 자동으로 설정
    SwapChainDesc.BufferDesc.Height = 0;                           // 창 크기에 맞게 자동으로 설정
    SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // 색상 포멧
    SwapChainDesc.SampleDesc.Count = 1;                            // 멀티 샘플링 비활성화
    SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;   // 렌더 타겟으로 설정
    SwapChainDesc.BufferCount = 2;                                 // 더블 버퍼링
    SwapChainDesc.OutputWindow = hWindow;                          // 렌더링할 창 핸들
    SwapChainDesc.Windowed = TRUE;                                 // 창 모드
    SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;      // 스왑 방식

    // Direct3D Device와 SwapChain을 생성
    HRESULT result = D3D11CreateDeviceAndSwapChain(
        // 입력 매개변수
        nullptr,                                                       // 디바이스를 만들 때 사용할 비디오 어댑터에 대한 포인터
        D3D_DRIVER_TYPE_HARDWARE,                                      // 만들 드라이버 유형을 나타내는 D3D_DRIVER_TYPE 열거형 값
        nullptr,                                                       // 소프트웨어 래스터라이저를 구현하는 DLL에 대한 핸들
        D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG,  // 사용할 런타임 계층을 지정하는 D3D11_CREATE_DEVICE_FLAG 열거형 값들의 조합
        FeatureLevels,                                                 // 만들려는 기능 수준의 순서를 결정하는 D3D_FEATURE_LEVEL 배열에 대한 포인터
        ARRAYSIZE(FeatureLevels),                                      // pFeatureLevels 배열의 요소 수
        D3D11_SDK_VERSION,                                             // SDK 버전. 주로 D3D11_SDK_VERSION을 사용
        &SwapChainDesc,                                                // SwapChain 설정과 관련된 DXGI_SWAP_CHAIN_DESC 구조체에 대한 포인터

        // 출력 매개변수
        &SwapChain,                                                    // 생성된 IDXGISwapChain 인터페이스에 대한 포인터
        &Device,                                                       // 생성된 ID3D11Device 인터페이스에 대한 포인터
        nullptr,                                                       // 선택된 기능 수준을 나타내는 D3D_FEATURE_LEVEL 값을 반환
        &DeviceContext                                                 // 생성된 ID3D11DeviceContext 인터페이스에 대한 포인터
    );
    if (FAILED(result))
	{
		wchar_t errorMsg[256];
		swprintf_s(errorMsg, L"Failed to create Device and SwapChain! HRESULT: 0x%08X", result);
		MessageBox(hWnd, errorMsg, L"Error", MB_ICONERROR | MB_OK);
		return;
	}

    // 생성된 SwapChain의 정보 가져오기
    result = SwapChain->GetDesc(&SwapChainDesc);
	if (FAILED(result))
	{
		wchar_t errorMsg[256];
		swprintf_s(errorMsg, L"Failed to get SwapChain Description! HRESULT: 0x%08X", result);
		MessageBox(hWnd, errorMsg, L"Error", MB_ICONERROR | MB_OK);
		return;
	}

    // 뷰포트 정보 설정
    ViewportInfo = {
        .TopLeftX= 0.0f, .TopLeftY= 0.0f,
        .Width= static_cast<float>(SwapChainDesc.BufferDesc.Width),
		.Height= static_cast<float>(SwapChainDesc.BufferDesc.Height),
        .MinDepth= 0.0f, .MaxDepth= 1.0f
    };

    Device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&debugDevice));
    if (SUCCEEDED(Device->QueryInterface(__uuidof(ID3D11InfoQueue), reinterpret_cast<void**>(&infoQueue))))
    {
        infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, TRUE);
        infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, TRUE);
        infoQueue->Release();
    }
    OutputDebugString(L"Debug layer initialized.\n");
}

void URenderer::ReleaseDeviceAndSwapChain()
{
    if (DeviceContext)
    {
        DeviceContext->Flush(); // 남이있는 GPU 명령 실행
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
    // 스왑 체인으로부터 백 버퍼 텍스처 가져오기
    HRESULT result = SwapChain->GetBuffer(0, IID_PPV_ARGS(&FrameBuffer));
	if (FAILED(result))
	{
		wchar_t errorMsg[256];
		swprintf_s(errorMsg, L"Failed to get Back Buffer! HRESULT: 0x%08X", result);
		MessageBox(hWnd, errorMsg, L"Error", MB_ICONERROR | MB_OK);
		return;
	}

    if (FrameBuffer == nullptr) {
        MessageBox(hWnd, L"FrameBuffer is not initialized!", L"Error", MB_ICONERROR | MB_OK);
        return;
    }

    DXGI_SWAP_CHAIN_DESC SwapChainDesc;
    result = SwapChain->GetDesc(&SwapChainDesc);
    if (FAILED(result))
    {
        return;
    }

    // 렌더 타겟 뷰 생성
    D3D11_RENDER_TARGET_VIEW_DESC FrameBufferRTVDesc = {};
    FrameBufferRTVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;      // 색상 포맷
    FrameBufferRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D; // 2D 텍스처

    result = Device->CreateRenderTargetView(FrameBuffer, &FrameBufferRTVDesc, &FrameBufferRTV);
	if (FAILED(result))
	{
		wchar_t errorMsg[256];
		swprintf_s(errorMsg, L"Failed to create Render Target View! HRESULT: 0x%08X", result);
		MessageBox(hWnd, errorMsg, L"Error", MB_ICONERROR | MB_OK);
        return;
	}
}

void URenderer::CreateDepthStencilBuffer()
{
    D3D11_TEXTURE2D_DESC DepthBufferDesc = {};
    DepthBufferDesc.Width = static_cast<UINT>(ViewportInfo.Width);
    DepthBufferDesc.Height = static_cast<UINT>(ViewportInfo.Height);
    DepthBufferDesc.MipLevels = 1;
    DepthBufferDesc.ArraySize = 1;
    DepthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;            // 32비트 중 24비트는 깊이, 8비트는 스텐실
    DepthBufferDesc.SampleDesc.Count = 1;
    DepthBufferDesc.SampleDesc.Quality = 0;
    DepthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    DepthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;              // 텍스쳐 바인딩 플래그를 DepthStencil로 설정
    DepthBufferDesc.CPUAccessFlags = 0;
    DepthBufferDesc.MiscFlags = 0;

    HRESULT result = Device->CreateTexture2D(&DepthBufferDesc, nullptr, &DepthStencilBuffer);
	if (FAILED(result))
	{
		wchar_t errorMsg[256];
		swprintf_s(errorMsg, L"Failed to create Depth Stencil Buffer! HRESULT: 0x%08X", result);
		MessageBox(hWnd, errorMsg, L"Error", MB_ICONERROR | MB_OK);
		return;
	}

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = DepthBufferDesc.Format;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;

    result = Device->CreateDepthStencilView(DepthStencilBuffer, &dsvDesc, &DepthStencilView);
	if (FAILED(result))
	{
		wchar_t errorMsg[256];
		swprintf_s(errorMsg, L"Failed to create Depth Stencil View! HRESULT: 0x%08X", result);
		MessageBox(hWnd, errorMsg, L"Error", MB_ICONERROR | MB_OK);
		return;
	}
}

void URenderer::CreateDepthStencilState()
{
    D3D11_DEPTH_STENCIL_DESC DepthStencilDesc = {};
    DepthStencilDesc.DepthEnable = TRUE;
    DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;                     // 더 작은 깊이값이 왔을 때 픽셀을 갱신함

    HRESULT result = Device->CreateDepthStencilState(&DepthStencilDesc, &DepthStencilState);
	if (FAILED(result))
	{
		wchar_t errorMsg[256];
		swprintf_s(errorMsg, L"Failed to create Depth Stencil State! HRESULT: 0x%08X", result);
		MessageBox(hWnd, errorMsg, L"Error", MB_ICONERROR | MB_OK);
		return;
	}

    D3D11_DEPTH_STENCIL_DESC IgnoreDepthStencilDesc = {};
    IgnoreDepthStencilDesc.DepthEnable = TRUE;
    IgnoreDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    IgnoreDepthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
    result = Device->CreateDepthStencilState(&IgnoreDepthStencilDesc, &IgnoreDepthStencilState);
	if (FAILED(result))
	{
		wchar_t errorMsg[256];
		swprintf_s(errorMsg, L"Failed to create Ignore Depth Stencil State! HRESULT: 0x%08X", result);
		MessageBox(hWnd, errorMsg, L"Error", MB_ICONERROR | MB_OK);
		return;
	}
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
}

void URenderer::ReleaseDepthStencilState()
{
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

void URenderer::ReleaseDepthStencilResources()
{
    ReleaseDepthStencilBuffer();
	ReleaseDepthStencilState();
}

void URenderer::CreateRasterizerState()
{
    D3D11_RASTERIZER_DESC RasterizerDesc = {};
    RasterizerDesc.FillMode = D3D11_FILL_SOLID; // 채우기 모드
    RasterizerDesc.CullMode = D3D11_CULL_BACK;  // 백 페이스 컬링
    RasterizerDesc.FrontCounterClockwise = FALSE;

    HRESULT result = Device->CreateRasterizerState(&RasterizerDesc, &RasterizerState_Solid);
	if (FAILED(result))
	{
		wchar_t errorMsg[256];
		swprintf_s(errorMsg, L"Failed to create Rasterizer State! HRESULT: 0x%08X", result);
		MessageBox(hWnd, errorMsg, L"Error", MB_ICONERROR | MB_OK);
		return;
	}

    RasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	RasterizerDesc.CullMode = D3D11_CULL_NONE;

	result = Device->CreateRasterizerState(&RasterizerDesc, &RasterizerState_Wireframe);
	if (FAILED(result))
	{
		wchar_t errorMsg[256];
		swprintf_s(errorMsg, L"Failed to create Wireframe Rasterizer State! HRESULT: 0x%08X", result);
		MessageBox(hWnd, errorMsg, L"Error", MB_ICONERROR | MB_OK);
		return;
	}
}

void URenderer::ReleaseRasterizerState()
{
    if (RasterizerState_Solid)
    {
        RasterizerState_Solid->Release();
        RasterizerState_Solid = nullptr;
    }

	if (RasterizerState_Wireframe)
	{
		RasterizerState_Wireframe->Release();
		RasterizerState_Wireframe = nullptr;
	}
}

void URenderer::CreateBufferCache()
{
    BufferCache = std::make_unique<FBufferCache>();
}

void URenderer::CreateShaderCache()
{
	ShaderCache = std::make_unique<FShaderCache>();
}

void URenderer::InitMatrix()
{
    ProjectionMatrix = FMatrix::Identity;
}

void URenderer::CreateBlendState()
{
    D3D11_BLEND_DESC BlendState;
    ZeroMemory(&BlendState, sizeof(D3D11_BLEND_DESC));
    BlendState.RenderTarget[0].BlendEnable = TRUE;
    BlendState.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;  // 소스 색상: 알파값 사용
    BlendState.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA; // 대상 색상: (1 - 알파)
    BlendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    BlendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;   // 알파값 유지
    BlendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    BlendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    Device->CreateBlendState(&BlendState, &GridBlendState);
}

void URenderer::ReleaseBlendState()
{
    if (GridBlendState)
    {
        GridBlendState->Release();
        GridBlendState = nullptr;
    }
}

void URenderer::CreateTextureSamplerState()
{
    // 샘플러 상태 생성
    D3D11_SAMPLER_DESC SamplerDesc = {};
    SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    SamplerDesc.MinLOD = 0;
    SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    URenderer* Renderer = UEngine::Get().GetRenderer();
    Renderer->GetDevice()->CreateSamplerState(&SamplerDesc, &SamplerState);
}

void URenderer::ReleaseTextureSamplerState()
{
    if (SamplerState)
    {
        SamplerState->Release();
    }
}

HRESULT URenderer::GenerateWorldGridVertices(int32 WorldGridCellPerSide)
{
    HRESULT hr = S_OK;

    /**
     * GridVertexNum 값에 대한 설명:
     *   WorldGridCellPerSide 변수는 이름대로 한 모서리에 몇개의 그리드 칸이 존재할지를 설정하는 변수.
     *   만약 n개의 구역을 나눈다면 선은 n + 1개를 그려야하므로, 1을 더함.
     *   선은 가로 선과 세로 선이 존재하므로, 2를 곱함.
     *   하나의 선은 2개의 정점으로 생성되므로, 마지막으로 2를 곱함.
     */
    GridVertexNum = ((WorldGridCellPerSide + 1) * 2) * 2;
    float GridGap = 1.f; // WorldGrid Actor의 Scale을 통해 Gap 조정 가능. 현재는 아래 식의 이해를 돕기 위해 변수로 따로 분리함.

    int32 GridMin = (WorldGridCellPerSide * GridGap / 2) * -1;
    int32 GridMax = WorldGridCellPerSide * GridGap + GridMin;
    
    TArray<FVertexGrid> GridVertexData(GridVertexNum);
    for (int i = 0; i <= WorldGridCellPerSide * 4; i += 4)
    {
        float Offset = GridMin + GridGap * i / 4;
        FVertexGrid LineVertex;
        LineVertex.Location = FVector(Offset, GridMin, 0.f);
        GridVertexData[i] = LineVertex;

        LineVertex.Location = FVector(Offset, GridMax, 0.f);
        GridVertexData[i + 1] = LineVertex;

        LineVertex.Location = FVector(GridMin, Offset, 0.f);
        GridVertexData[i + 2] = LineVertex;

        LineVertex.Location = FVector(GridMax, Offset, 0.f);
        GridVertexData[i + 3] = LineVertex;
    }
    
    D3D11_BUFFER_DESC GridVertexBufferDesc = {};
    ZeroMemory(&GridVertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
    GridVertexBufferDesc.ByteWidth = sizeof(FVertexGrid) * GridVertexNum;
    GridVertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    GridVertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    GridVertexBufferDesc.CPUAccessFlags = 0;
    GridVertexBufferDesc.MiscFlags = 0;
    GridVertexBufferDesc.StructureByteStride = 0;
    
    D3D11_SUBRESOURCE_DATA GridVertexInitData;
    ZeroMemory(&GridVertexInitData, sizeof(GridVertexInitData));
    GridVertexInitData.pSysMem = GridVertexData.GetData();
    
    hr = Device->CreateBuffer(&GridVertexBufferDesc, &GridVertexInitData, &GridVertexBuffer);
    if (FAILED(hr))
        return hr;

    return S_OK;
}

void URenderer::SetRenderMode(EViewModeIndex Type)
{
	CurrentRasterizerStateType = Type;
}

bool URenderer::IsOccluded()
{
    if (bSwapChainOccluded && SwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
    {
        ::Sleep(10);
        return true;
    }
    bSwapChainOccluded = false;
    return false;
}

void URenderer::AddDebugLine(FVector Start, FVector End, FVector Color, float Time)
{
    DebugLines.Add({Start, End, Color, Time});
}

void URenderer::RenderDebugLines(float DeltaTime)
{
    UpdateDebugLines(DeltaTime);

    if (DebugLines.IsEmpty())
    {
        return;
    }
    
    PrepareDebugLines();

    D3D11_MAPPED_SUBRESOURCE MappedSubresource;
    DeviceContext->Map(DebugLineVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource);
    if (FVertexSimple* VertexSimple = reinterpret_cast<FVertexSimple*>(MappedSubresource.pData))
    {
        // TODO: DebugLineVertexBuffer에 디버그 라인 정점 정보 넣기.
        int32 BufferIdx = 0;
        for (int32 i = 0; i < DebugLines.Num(); ++i)
        {
            FVertexSimple StartVertex(
                    DebugLines[i].Start.X,
                    DebugLines[i].Start.Y,
                    DebugLines[i].Start.Z,
                    DebugLines[i].Color.X,
                    DebugLines[i].Color.Y,
                    DebugLines[i].Color.Z,
                    1.f
                );
            VertexSimple[BufferIdx++] = StartVertex;

            FVertexSimple EndVertex(
                    DebugLines[i].End.X,
                    DebugLines[i].End.Y,
                    DebugLines[i].End.Z,
                    DebugLines[i].Color.X,
                    DebugLines[i].Color.Y,
                    DebugLines[i].Color.Z,
                    1.f
                );
            VertexSimple[BufferIdx++] = EndVertex;
        }
    }
    DeviceContext->Unmap(DebugLineVertexBuffer, 0);
    
    DeviceContext->Draw(DebugLines.Num() * 2, 0);
}

void URenderer::CreateTextureBuffer()
{
	D3D11_BUFFER_DESC TextureBufferDesc = {};
	TextureBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    TextureBufferDesc.ByteWidth = sizeof(FVertexUV) * 6;
	TextureBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA TextureBufferInitData = {};
	TextureBufferInitData.pSysMem = UVQuadVertices;

    Device->CreateBuffer(&TextureBufferDesc, &TextureBufferInitData, &TextureVertexBuffer);
}

void URenderer::PrepareBillboard()
{
    UINT Stride = sizeof(FVertexUV);
    UINT Offset = 0;
    DeviceContext->IASetVertexBuffers(0, 1, &TextureVertexBuffer, &Stride, &Offset);
    DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    DeviceContext->IASetInputLayout(ShaderCache->GetInputLayout(L"ShaderTexture"));
    DeviceContext->VSSetShader(ShaderCache->GetVertexShader(L"ShaderTexture"), nullptr, 0);
    DeviceContext->PSSetShader(ShaderCache->GetPixelShader(L"ShaderTexture"), nullptr, 0);
    DeviceContext->PSSetSamplers(0, 1, &SamplerState);
}

void URenderer::RenderBillboard()
{
	DeviceContext->Draw(6, 0);
}

void URenderer::UpdateTextureConstantBuffer(const FMatrix& World, float u, float v)
{
    D3D11_MAPPED_SUBRESOURCE MappedResource;
    HRESULT hr = DeviceContext->Map(TextureConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
    if (FAILED(hr))
        return;

    FTextureConstants* BufferData = reinterpret_cast<FTextureConstants*>(MappedResource.pData);
    BufferData->WorldViewProj =FMatrix::Transpose(World * ViewMatrix * ProjectionMatrix);
    BufferData->u = u;
	BufferData->v = v;

    DeviceContext->Unmap(TextureConstantBuffer, 0);
}

void URenderer::AdjustDebugLineVertexBuffer(uint32 LineNum)
{
    if (DebugLineNumStep == 0)
    {
        // TODO: ERROR 처리
        return;
    }

    uint32 LowerBound = (DebugLineCurrentMaxNum >= DebugLineNumStep) ? (DebugLineCurrentMaxNum - DebugLineNumStep) : 0;
    
    if (LowerBound <= LineNum && LineNum <= DebugLineCurrentMaxNum)
    {
        // Line Num의 개수가 현재 버퍼 크기의 허용 범위
        return;
    }
    
    DebugLineCurrentMaxNum = ((LineNum + DebugLineNumStep) / DebugLineNumStep) * DebugLineNumStep;
    CreateDebugLineVertexBuffer(DebugLineCurrentMaxNum * 2);
}

void URenderer::CreateDebugLineVertexBuffer(uint32 NewSize)
{
    if (DebugLineVertexBuffer)
    {
        DebugLineVertexBuffer->Release();    
    }
    
    D3D11_BUFFER_DESC DebugLineVertexBufferDesc = {};
    ZeroMemory(&DebugLineVertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
    DebugLineVertexBufferDesc.ByteWidth = sizeof(FVertexSimple) * NewSize;
    DebugLineVertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    DebugLineVertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    DebugLineVertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    DebugLineVertexBufferDesc.MiscFlags = 0;
    DebugLineVertexBufferDesc.StructureByteStride = 0;
    
    HRESULT result = Device->CreateBuffer(&DebugLineVertexBufferDesc, NULL, &DebugLineVertexBuffer);
    if (FAILED(result))
    {
        wchar_t errorMsg[256];
        swprintf_s(errorMsg, L"Failed to create Debug Line Vertex Buffer! HRESULT: 0x%08X", result);
        MessageBox(hWnd, errorMsg, L"Error", MB_ICONERROR | MB_OK);
    }
}

void URenderer::UpdateDebugLines(float DeltaTime)
{
    if (DebugLines.IsEmpty())
    {
        return;
    }

    for (auto& DebugLine : DebugLines)
    {
        DebugLine.Time -= DeltaTime;
    }

    DebugLines.RemoveAll(
        [](const FDebugLineInfo& Info)
        {
            return Info.Time <= 0.f;            
        }
    );

    AdjustDebugLineVertexBuffer(DebugLines.Num());
}

void URenderer::PrepareDebugLines()
{
    UINT Offset = 0;
    DeviceContext->IASetVertexBuffers(0, 1, &DebugLineVertexBuffer, &Stride, &Offset);
    
    DeviceContext->OMSetDepthStencilState(DepthStencilState, 0);
    DeviceContext->OMSetRenderTargets(1, &FrameBufferRTV, DepthStencilView);
    DeviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
    
    DeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
    
	ID3D11InputLayout* InputLayout = ShaderCache->GetInputLayout(L"ShaderMain");
	ID3D11VertexShader* VertexShader = ShaderCache->GetVertexShader(L"ShaderDebugLine");
	ID3D11PixelShader* PixelShader = ShaderCache->GetPixelShader(L"ShaderDebugLine");

    DeviceContext->IASetInputLayout(InputLayout);
    DeviceContext->VSSetShader(VertexShader, nullptr, 0);
    DeviceContext->PSSetShader(PixelShader, nullptr, 0);
}

void URenderer::CreatePickingFrameBuffer()
{
    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = ViewportInfo.Width;
    textureDesc.Height = ViewportInfo.Height;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    HRESULT result = Device->CreateTexture2D(&textureDesc, nullptr, &PickingFrameBuffer);
	if (FAILED(result))
	{
		wchar_t errorMsg[256];
		swprintf_s(errorMsg, L"Failed to create Picking Frame Buffer! HRESULT: 0x%08X", result);
		MessageBox(hWnd, errorMsg, L"Error", MB_ICONERROR | MB_OK);
		return;
	}

    D3D11_RENDER_TARGET_VIEW_DESC PickingFrameBufferRTVDesc = {};
    PickingFrameBufferRTVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;      // 색상 포맷
    PickingFrameBufferRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D; // 2D 텍스처

    result = Device->CreateRenderTargetView(PickingFrameBuffer, &PickingFrameBufferRTVDesc, &PickingFrameBufferRTV);
	if (FAILED(result))
	{
		wchar_t errorMsg[256];
		swprintf_s(errorMsg, L"Failed to create Picking Frame Buffer RTV! HRESULT: 0x%08X", result);
		MessageBox(hWnd, errorMsg, L"Error", MB_ICONERROR | MB_OK);
		return;
	}
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

void URenderer::PrepareZIgnore()
{
    DeviceContext->OMSetDepthStencilState(IgnoreDepthStencilState, 0);
}

void URenderer::PreparePicking()
{
    // 렌더 타겟 바인딩
    DeviceContext->OMSetRenderTargets(1, &PickingFrameBufferRTV, DepthStencilView);
    DeviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
    DeviceContext->OMSetDepthStencilState(DepthStencilState, 0);                // DepthStencil 상태 설정. StencilRef: 스텐실 테스트 결과의 레퍼런스

    DeviceContext->ClearRenderTargetView(PickingFrameBufferRTV, PickingClearColor);
}

void URenderer::PreparePickingShader() const
{
	ID3D11PixelShader* PickingPixelShader = ShaderCache->GetPixelShader(L"ShaderPicking");
    DeviceContext->PSSetShader(PickingPixelShader, nullptr, 0);

}

void URenderer::UpdateConstantPicking(FVector4 UUIDColor) const
{
    if (!ConstantPickingBuffer) return;

    D3D11_MAPPED_SUBRESOURCE ConstantBufferMSR;

    UUIDColor = FVector4(UUIDColor.X / 255.0f, UUIDColor.Y / 255.0f, UUIDColor.Z / 255.0f, UUIDColor.W / 255.0f);

    HRESULT result = DeviceContext->Map(ConstantPickingBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ConstantBufferMSR);
	if (FAILED(result))
	{
		wchar_t errorMsg[256];
		swprintf_s(errorMsg, L"Failed to map Constant Picking Buffer! HRESULT: 0x%08X", result);
		MessageBox(hWnd, errorMsg, L"Error", MB_ICONERROR | MB_OK);
		return;
	}
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

    HRESULT result = DeviceContext->Map(ConstantsDepthBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ConstantBufferMSR);
	if (FAILED(result))
	{
		wchar_t errorMsg[256];
		swprintf_s(errorMsg, L"Failed to map Constant Depth Buffer! HRESULT: 0x%08X", result);
		MessageBox(hWnd, errorMsg, L"Error", MB_ICONERROR | MB_OK);
		return;
	}
	{
        FDepthConstants* Constants = static_cast<FDepthConstants*>(ConstantBufferMSR.pData);
        Constants->DepthOffset = Depth;
        //@TODO: nP/fP Constant 전달시, float -> int 문제 없는가?
        Constants->nearPlane = Cam->GetNearClip();
        Constants->farPlane = Cam->GetFarClip();
    }
    DeviceContext->Unmap(ConstantsDepthBuffer, 0);
}

void URenderer::PrepareMain()
{
    DeviceContext->OMSetDepthStencilState(DepthStencilState, 0);                // DepthStencil 상태 설정. StencilRef: 스텐실 테스트 결과의 레퍼런스
    DeviceContext->OMSetRenderTargets(1, &FrameBufferRTV, DepthStencilView);
    DeviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
    DeviceContext->IASetInputLayout(ShaderCache->GetInputLayout(L"ShaderMain"));
}

void URenderer::PrepareMainShader()
{
    DeviceContext->VSSetShader(ShaderCache->GetVertexShader(L"ShaderMain"), nullptr, 0);
    DeviceContext->PSSetShader(ShaderCache->GetPixelShader(L"ShaderMain"), nullptr, 0);
}

FVector4 URenderer::GetPixel(FVector MPos)
{
    FVector TempMPos;

    // Bound Check, 화면 크기에 1px의 패딩을 줌
    TempMPos.X = FMath::Clamp(MPos.X, 1.0f, ViewportInfo.Width - 1);
    TempMPos.Y = FMath::Clamp(MPos.Y, 1.0f, ViewportInfo.Height - 1);
    // 1. Staging 텍스처 생성 (1x1 픽셀)
    D3D11_TEXTURE2D_DESC stagingDesc = {};
    stagingDesc.Width = 1; // 픽셀 1개만 복사
    stagingDesc.Height = 1;
    stagingDesc.MipLevels = 1;
    stagingDesc.ArraySize = 1;
    stagingDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 원본 텍스처 포맷과 동일
    stagingDesc.SampleDesc.Count = 1;
    stagingDesc.Usage = D3D11_USAGE_STAGING;
    stagingDesc.BindFlags = 0;
    stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

    ID3D11Texture2D* stagingTexture = nullptr;
    HRESULT result = Device->CreateTexture2D(&stagingDesc, nullptr, &stagingTexture);
	if (FAILED(result))
	{
		wchar_t errorMsg[256];
		swprintf_s(errorMsg, L"Failed to create Staging Texture! HRESULT: 0x%08X", result);
		MessageBox(hWnd, errorMsg, L"Error", MB_ICONERROR | MB_OK);
		return FVector4();
	}

    // 2. 복사할 영역 정의 (D3D11_BOX)
    D3D11_BOX srcBox = {};
    srcBox.left = static_cast<UINT>(TempMPos.X);
    srcBox.right = srcBox.left + 1; // 1픽셀 너비
    srcBox.top = static_cast<UINT>(TempMPos.Y);
    srcBox.bottom = srcBox.top + 1; // 1픽셀 높이
    srcBox.front = 0;
    srcBox.back = 1;
    FVector4 color{ 1, 1, 1, 1 };

    if (stagingTexture == nullptr)
        return color;

    D3D11_TEXTURE2D_DESC originalDesc;
    PickingFrameBuffer->GetDesc(&originalDesc);

    if (srcBox.left >= originalDesc.Width || srcBox.right > originalDesc.Width ||
        srcBox.top >= originalDesc.Height || srcBox.bottom > originalDesc.Height) {
        // srcBox가 원본 텍스처의 범위를 벗어났습니다.
        // 적절한 오류 처리를 수행합니다.
        MessageBox(hWnd, L"srcBox coordinates are out of the original texture bounds.", L"Error", MB_ICONERROR | MB_OK);
        return FVector4();
    }

    // 3. 특정 좌표만 복사
    DeviceContext->CopySubresourceRegion(
        stagingTexture, // 대상 텍스처
        0,              // 대상 서브리소스
        0, 0, 0,        // 대상 좌표 (x, y, z)
        PickingFrameBuffer, // 원본 텍스처
        0,              // 원본 서브리소스
        &srcBox         // 복사 영역
    );

    // 4. 데이터 매핑
    D3D11_MAPPED_SUBRESOURCE mapped = {};
    result = DeviceContext->Map(stagingTexture, 0, D3D11_MAP_READ, 0, &mapped);
	if (FAILED(result))
	{
		wchar_t errorMsg[256];
		swprintf_s(errorMsg, L"Failed to map Staging Texture! HRESULT: 0x%08X", result);
		MessageBox(hWnd, errorMsg, L"Error", MB_ICONERROR | MB_OK);
		return FVector4();
	}

    // 5. 픽셀 데이터 추출 (1x1 텍스처이므로 offset = 0)
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

    // 6. 매핑 해제 및 정리
    DeviceContext->Unmap(stagingTexture, 0);
    stagingTexture->Release();

    return color;
}

void URenderer::UpdateViewMatrix(const FTransform& CameraTransform)
{
    // Update Constant Buffer
    D3D11_MAPPED_SUBRESOURCE ConstantBufferMSR;
    DeviceContext->Map(CbChangeEveryFrame, 0, D3D11_MAP_WRITE_DISCARD, 0, &ConstantBufferMSR);
    // 매핑된 메모리를 캐스팅
    ViewMatrix = CameraTransform.GetViewMatrix();
    if (FCbChangeEveryFrame* Constants = static_cast<FCbChangeEveryFrame*>(ConstantBufferMSR.pData))
    {
        Constants->ViewMatrix = FMatrix::Transpose(ViewMatrix);
        Constants->ViewPosition = CameraTransform.GetPosition();
    }
    // UnMap해서 GPU에 값이 전달 될 수 있게 함
    DeviceContext->Unmap(CbChangeEveryFrame, 0);
}

void URenderer::UpdateProjectionMatrix(ACamera* Camera)
{
    float AspectRatio = UEngine::Get().GetScreenRatio();

    float FOV = FMath::DegreesToRadians(Camera->GetFieldOfView());
    float NearClip = Camera->GetNearClip();
    float FarClip = Camera->GetFarClip();

    if (Camera->ProjectionMode == ECameraProjectionMode::Perspective)
    {
        ProjectionMatrix = FMatrix::PerspectiveFovLH(FOV, AspectRatio, NearClip, FarClip);
    }
    else
    {
        //@TODO: Delete Magic Number '360'
        float SizeDivisor = 360.f;
        int32 ScreenWidth = UEngine::Get().GetScreenWidth();
        int32 ScreenHeight = UEngine::Get().GetScreenHeight();
        ProjectionMatrix = FMatrix::OrthoLH(ScreenWidth / SizeDivisor, ScreenHeight / SizeDivisor, NearClip, FarClip);
    }

    // Update Constant Buffer
    FCbChangeOnResizeAndFov ChangesOnResizeAndFov;
    ChangesOnResizeAndFov.ProjectionMatrix = FMatrix::Transpose(ProjectionMatrix);
    ChangesOnResizeAndFov.FarClip = FarClip;
    ChangesOnResizeAndFov.NearClip = NearClip;
    DeviceContext->UpdateSubresource(CbChangeOnResizeAndFov, 0, NULL, &ChangesOnResizeAndFov, 0, 0);
}

void URenderer::OnUpdateWindowSize(int Width, int Height)
{
    if (SwapChain)
    {
		DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

        ReleasePickingFrameBuffer();

        ReleaseDepthStencilBuffer();
        ReleaseFrameBuffer();

        //if (Width <= 0 || Height <= 0) {
        //    MessageBox(hWnd, L"Invalid window size parameters.", L"Error", MB_ICONERROR | MB_OK);
        //    return;
        //}

        HRESULT hr = SwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
		if (FAILED(hr))
		{
            wchar_t errorMsg[256];
			swprintf_s(errorMsg, L"Failed to resize SwapChain! HRESULT: 0x%08X", hr);
			MessageBox(hWnd, errorMsg, L"Error", MB_ICONERROR | MB_OK);
			return;
		}

        DXGI_SWAP_CHAIN_DESC SwapChainDesc;
        hr = SwapChain->GetDesc(&SwapChainDesc);
		if (FAILED(hr))
		{
			wchar_t errorMsg[256];
			swprintf_s(errorMsg, L"Failed to get SwapChain Description! HRESULT: 0x%08X", hr);
			return;
		}

        ViewportInfo = {
            .TopLeftX= 0.0f, .TopLeftY= 0.0f,
            .Width= static_cast<float>(Width),
			.Height= static_cast<float>(Height),
            .MinDepth= 0.0f, .MaxDepth= 1.0f
        };

        CreateFrameBuffer();
        CreateDepthStencilBuffer();

    	CreatePickingFrameBuffer();

    }

    if (ACamera* Camera = FEditorManager::Get().GetCamera())
    {
        UpdateProjectionMatrix(Camera);
    }
}

void URenderer::GetPrimitiveLocalBounds(EPrimitiveType Type, FVector& OutMin, FVector& OutMax)
{
	if (Type == EPrimitiveType::EPT_None)
	{
		return;
	}

    FVertexBufferInfo Info = BufferCache->GetVertexBufferInfo(Type);
    if (Info.GetVertexBuffer() == nullptr)
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
    HRESULT result = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
	if (FAILED(result))
	{
		wchar_t errorMsg[256];
		swprintf_s(errorMsg, L"Failed to get Back Buffer(RENDER_PICKING)! HRESULT: 0x%08X", result);
		MessageBox(hWnd, errorMsg, L"Error", MB_ICONERROR | MB_OK);
		return;
	}
    DeviceContext->CopyResource(backBuffer, PickingFrameBuffer);
    backBuffer->Release();
}