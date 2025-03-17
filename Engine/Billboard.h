#pragma once
#include "CoreUObject/Components/PrimitiveComponent.h"
class UBillboard : public UPrimitiveComponent
{
	UCLASS(UBillboard, UPrimitiveComponent);
	using Super = UPrimitiveComponent;
public:
	UBillboard();
	virtual ~UBillboard() = default;

	virtual void Render() override;
	virtual void Tick(float DeltaTime) override;
	void SetTexture(ID3D11ShaderResourceView* InTexture);

private:
	ID3D11ShaderResourceView* Texture = nullptr;
	ID3D11SamplerState* SamplerState = nullptr;
	int32 TotalCols = 1;
	int32 TotalRows = 1;
};

