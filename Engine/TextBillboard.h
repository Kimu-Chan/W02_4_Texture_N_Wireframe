#pragma once
#include "Billboard.h"
class UTextBillboard : public UBillboard
{
	UCLASS(UTextBillboard, UBillboard);
	using Super = UBillboard;

public:
	UTextBillboard();
	virtual ~UTextBillboard() = default;
	virtual void BeginPlay() override;
	virtual void Render() override;
	
	void SetText(const std::wstring& InString);

private:
	ID3D11Buffer* TextVertexBuffer = nullptr;
	std::wstring TextString;

	void CreateTextVertexBuffer(int32 InVertexCount);
	void UpdateTextVertexBuffer();
};



