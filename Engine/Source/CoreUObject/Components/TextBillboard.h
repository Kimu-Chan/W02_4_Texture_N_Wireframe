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
	virtual void Render(class URenderer* Renderer) override;
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;
	
	void SetText(const std::wstring& InString);

private:
	std::wstring TextString;
};



