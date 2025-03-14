#include "pch.h" 
#include "Picker.h"
#include "Core/HAL/PlatformType.h"
#include "Core/Input/PlayerInput.h"
#include "Core/Rendering/URenderer.h"
#include "CoreUObject/Components/PrimitiveComponent.h"
#include "Gizmo/GizmoHandle.h"
#include "Static/EditorManager.h"
#include "Camera.h"
#include "Core/Math/Ray.h"
#include "World.h"
#include "Static/EditorManager.h"

APicker::APicker()
{
    bIsGizmo = true;
}

FVector4 APicker::EncodeUUID(unsigned int UUID)
{
    float a = (UUID >> 24) & 0xff;
    float b = (UUID >> 16) & 0xff;
    float g = (UUID >> 8) & 0xff;
    float r = UUID & 0xff;

    FVector4 color = { r, g, b, a };

    return color;
}

int APicker::DecodeUUID(FVector4 color)
{
    return (static_cast<unsigned int>(color.W) << 24) | (static_cast<unsigned int>(color.Z) << 16) | (static_cast<unsigned int>(color.Y) << 8) | (static_cast<unsigned int>(color.X));
}

void APicker::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void APicker::LateTick(float DeltaTime)
{
    AActor::LateTick(DeltaTime);

    if (APlayerInput::Get().GetMouseDown(false))
    {
        if (!PickByColor())
        {
            PickByRay();
        }

    }


    // 기즈모 핸들링
    if (APlayerInput::Get().IsPressedMouse(false))
    {
		HandleGizmo();
    }
    else
    {
        if (AGizmoHandle* Handle = FEditorManager::Get().GetGizmoHandle())
        {
            Handle->SetSelectedAxis(ESelectedAxis::None);
        }
    }
}

const char* APicker::GetTypeName()
{
    return "Picker";
}

bool APicker::PickByColor()
{
    bool bIsPicked = false;
    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(UEngine::Get().GetWindowHandle(), &pt);

    float ratioX = UEngine::Get().GetInitializedScreenWidth() / (float)UEngine::Get().GetScreenWidth();
    float ratioY = UEngine::Get().GetInitializedScreenHeight() / (float)UEngine::Get().GetScreenHeight();
    pt.x = pt.x * ratioX;
    pt.y = pt.y * ratioY;

    FVector4 color = UEngine::Get().GetRenderer()->GetPixel(FVector(pt.x, pt.y, 0));

    uint32_t UUID = DecodeUUID(color);

    UActorComponent* PickedComponent = UEngine::Get().GetObjectByUUID<UActorComponent>(UUID);

    if (PickedComponent != nullptr)
    {
        bIsPicked = true;
        AActor* PickedActor = PickedComponent->GetOwner();

        // 액터없는 컴포넌트가 검출될 수 있나? -> return false
        if (PickedActor == nullptr) return false;
        if (PickedComponent->GetOwner()->IsGizmoActor() == false)
        {
            if (PickedActor == FEditorManager::Get().GetSelectedActor())
            {
                FEditorManager::Get().SelectActor(nullptr);
            }
            else
            {
                FEditorManager::Get().SelectActor(PickedActor);
            }
        }
    }
    UE_LOG("Pick - UUID: %d", UUID);
    return bIsPicked;
}

bool APicker::PickByRay()
{
    bool bIsPicked = false;
    // 충돌 검출
	USceneComponent* FirstHitComponent = nullptr;

    if (GetWorld()->LineTrace(FRay::GetRayByMousePoint(FEditorManager::Get().GetCamera()), &FirstHitComponent))
    {
        bIsPicked = true;
        if (FirstHitComponent == nullptr)
            return false;

        AActor* HitActor = FirstHitComponent->GetOwner();
        if (HitActor != nullptr && HitActor->IsGizmoActor() == false)
        {
            if (HitActor == FEditorManager::Get().GetSelectedActor())
            {
                FEditorManager::Get().SelectActor(nullptr);
            }
            else
            {
                FEditorManager::Get().SelectActor(HitActor);
            }
        }
    }

    return bIsPicked;
    
}

void APicker::HandleGizmo()
{
    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(UEngine::Get().GetWindowHandle(), &pt);
    FVector4 color = UEngine::Get().GetRenderer()->GetPixel(FVector(pt.x, pt.y, 0));
    uint32_t UUID = DecodeUUID(color);

    UActorComponent* PickedComponent = UEngine::Get().GetObjectByUUID<UActorComponent>(UUID);
    if (PickedComponent != nullptr)
    {
        if (AGizmoHandle* Gizmo = dynamic_cast<AGizmoHandle*>(PickedComponent->GetOwner()))
        {
            if (Gizmo->GetSelectedAxis() != ESelectedAxis::None) return;
            UCylinderComp* CylinderComp = static_cast<UCylinderComp*>(PickedComponent);
            FVector4 CompColor = CylinderComp->GetCustomColor();
            if (1.0f - FMath::Abs(CompColor.X) < KINDA_SMALL_NUMBER) // Red - X��
            {
                Gizmo->SetSelectedAxis(ESelectedAxis::X);
            }
            else if (1.0f - FMath::Abs(CompColor.Y) < KINDA_SMALL_NUMBER) // Green - Y��
            {
                Gizmo->SetSelectedAxis(ESelectedAxis::Y);
            }
            else  // Blue - Z��
            {
                Gizmo->SetSelectedAxis(ESelectedAxis::Z);
            }
        }
    }
}
