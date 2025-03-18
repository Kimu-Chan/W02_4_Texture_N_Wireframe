#pragma once

#include "Engine/GameFrameWork/Actor.h"

class AGizmoHandle;

class FEditorManager : public TSingleton<FEditorManager>
{
public:
    
    inline AActor* GetSelectedActor() const {return SelectedActor;}
    
    void SelectActor(AActor* NewActor);

    inline ACamera* GetCamera() const {return Camera;}

    void SetCamera(ACamera* NewCamera);

    AGizmoHandle* GetGizmoHandle() const {return GizmoHandle;}

    void ToggleGizmoHandleLocal(bool bIsLocal);
    
private:
    ACamera* Camera = nullptr;
    AActor* SelectedActor = nullptr;
    AGizmoHandle* GizmoHandle = nullptr;
};
