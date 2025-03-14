#pragma once
#include "GameFrameWork/Actor.h"

/**
 * TODO: 현재는 Actor를 상속받지만, 카메라 액터에 컴포넌트 형식으로 추가하거나,
 *       오브젝트로 관리해도 가능할 듯함.
 */
class AWorldGrid : public AActor
{
public:
    AWorldGrid();
    
    virtual void Tick(float DeltaTime);

private:
    float GridGap;
    int GridCellsPerSide; // 항상 짝수 값으로
    int GridVertexNum;
};
