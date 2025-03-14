#include "pch.h"
#include "WorldGrid.h"

AWorldGrid::AWorldGrid()
    : GridGap(1.f)
    , GridCellsPerSide(10) // TODO: 이건 엔진에서 다루고, 결정해야 할 정보일 듯함.
    , GridVertexNum(((GridCellsPerSide + 1) * 2) * 2) // TODO: 위와 마찬가지.
{}

void AWorldGrid::Tick(float DeltaTime)
{
    AActor::Tick(DeltaTime);

}
