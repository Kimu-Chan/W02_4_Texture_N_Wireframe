#pragma once

#include "Engine/GameFrameWork/Actor.h"

class AGizmoHandle;

class FEditorManager : public TSingleton<FEditorManager>
{
public:
    
    //inline AActor* GetSelectedActor() const {return SelectedActor;}
    
    void SelectActor(AActor* NewActor);

    inline ACamera* GetCamera() const {return Camera;}

    void SetCamera(ACamera* NewCamera);

	void SetGizmoHandle(AGizmoHandle* NewGizmoHandle) { GizmoHandle = NewGizmoHandle; }
    AGizmoHandle* GetGizmoHandle() const {return GizmoHandle;}

    void ToggleGizmoHandleLocal(bool bIsLocal);

    USceneComponent* GetSelectedComponent() const;

	void SelectComponent(USceneComponent* SelectedComponent);

	void ClearSelectedComponent();
    
private:
    ACamera* Camera = nullptr;
    AActor* SelectedActor = nullptr;
    USceneComponent* SelectedComponent;
    AGizmoHandle* GizmoHandle = nullptr;
};
