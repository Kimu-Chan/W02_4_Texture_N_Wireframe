#pragma once
#include "CoreUObject/Components/PrimitiveComponent.h"
class UBillboard : public UPrimitiveComponent
{
	UCLASS(UBillboard, UPrimitiveComponent);
	using Super = UPrimitiveComponent;
public:
	UBillboard();
	virtual ~UBillboard() = default;

	virtual void BeginPlay() override;
	virtual void Render(class URenderer* Renderer) override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type Reason);
	void SetTexture(class ID3D11ShaderResourceView* InTexture);

private:
	ID3D11ShaderResourceView* Texture = nullptr;
	int32 TotalCols = 1;
	int32 TotalRows = 1;
};

