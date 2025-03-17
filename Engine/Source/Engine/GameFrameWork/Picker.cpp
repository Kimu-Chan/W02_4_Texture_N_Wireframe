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
#include "Input/PlayerController.h"
#include "Static/EditorManager.h"

REGISTER_CLASS(APicker);
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

    if (APlayerController::Get().IsUiInput())
    {
        return;
    }

    if (APlayerInput::Get().IsMousePressed(false))    //좌클릭
    {
        if (!PickByColor())
        {
            PickByRay();
        }
    }


    // 기즈모 핸들링
    if (APlayerInput::Get().IsMouseDown(false))    //좌클릭
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
    int32 X = 0;
    int32 Y = 0;
    APlayerInput::Get().GetMousePosition(X, Y);

    FVector4 color = UEngine::Get().GetRenderer()->GetPixel(X, Y);
    uint32_t UUID = DecodeUUID(color);

    UActorComponent* PickedComponent = UEngine::Get().GetObjectByUUID<UActorComponent>(UUID);

    bool bIsPicked = false;
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
    int32 X = 0;
    int32 Y = 0;
    APlayerInput::Get().GetMousePosition(X, Y);
    
    FVector4 color = UEngine::Get().GetRenderer()->GetPixel(X, Y);
    uint32_t UUID = DecodeUUID(color);

    UActorComponent* PickedComponent = UEngine::Get().GetObjectByUUID<UActorComponent>(UUID);
    if (PickedComponent != nullptr)
    {
        if (PickedComponent->GetOwner() && PickedComponent->GetOwner()->IsA<AGizmoHandle>())
        {
			AGizmoHandle* Gizmo = static_cast<AGizmoHandle*>(PickedComponent->GetOwner());
            if (Gizmo->GetSelectedAxis() != ESelectedAxis::None) return;
            UCylinderComp* CylinderComp = static_cast<UCylinderComp*>(PickedComponent);
            FVector4 CompColor = CylinderComp->GetCustomColor();
            if (1.0f - FMath::Abs(CompColor.X) < KINDA_SMALL_NUMBER) // Red - X축
            {
                Gizmo->SetSelectedAxis(ESelectedAxis::X);
            }
            else if (1.0f - FMath::Abs(CompColor.Y) < KINDA_SMALL_NUMBER) // Green - Y축
            {
                Gizmo->SetSelectedAxis(ESelectedAxis::Y);
            }
            else  // Blue - Z축
            {
                Gizmo->SetSelectedAxis(ESelectedAxis::Z);
            }
        }
    }
}
