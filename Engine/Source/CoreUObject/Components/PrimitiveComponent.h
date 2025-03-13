#pragma once

#include "Engine/Engine.h"
#include "CoreUObject/Components/SceneComponent.h"
#include "Primitive/PrimitiveVertices.h"
#include "Core/Math/Plane.h"


class UPrimitiveComponent : public USceneComponent
{
	using Super = USceneComponent;
public:
	UPrimitiveComponent() = default;
	virtual ~UPrimitiveComponent() = default;

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void UpdateConstantPicking(const URenderer& Renderer, FVector4 UUIDColor) const;
	void UpdateConstantDepth(const URenderer& Renderer, int Depth) const;
	virtual void Render();

	virtual EPrimitiveType GetType() { return EPrimitiveType::EPT_None; }

	bool IsUseVertexColor() const { return bUseVertexColor; }

	void SetCustomColor(const FVector4& InColor)
	{
		CustomColor = InColor; 
		bUseVertexColor = false;
	}

	void SetUseVertexColor(bool bUse)
	{
		bUseVertexColor = bUse;
	}
	const FVector4& GetCustomColor() const { return CustomColor; }

public:
	virtual void RegisterComponentWithWorld(class UWorld* World);

public:
	void SetCanBeRendered(bool bRender) { bCanBeRendered = bRender; }

	void SetIsOrthoGraphic(bool IsOrtho) { bIsOrthoGraphic = IsOrtho; }
	bool GetIsOrthoGraphic() { return bIsOrthoGraphic;}
	
	// 바운딩 박스
public:
	virtual void InitBoundingBox() override;
	virtual void UpdateBoundingBox() override;

protected:
	bool bCanBeRendered = false;
	bool bUseVertexColor = true;
	bool bIsOrthoGraphic = false;
	FVector4 CustomColor = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
};

class UCubeComp : public UPrimitiveComponent
{
	using Super = UPrimitiveComponent;
public:
	UCubeComp()
	{
		bCanBeRendered = true;
	}
	virtual ~UCubeComp() = default;
	EPrimitiveType GetType() override
	{
		return EPrimitiveType::EPT_Cube;
	}

protected:
	virtual void InitBoundingBox() override
	{
		FVector Min;
		FVector Max;
		UEngine::Get().GetRenderer()->GetPrimitiveLocalBounds(EPrimitiveType::EPT_Cube, Min, Max);

		BoundingBox.Init(Min, Max);
	}
};
class USphereComp : public UPrimitiveComponent
{
	using Super = UPrimitiveComponent;
public:
	USphereComp()
	{
		bCanBeRendered = true;
	}
	virtual ~USphereComp() = default;
	EPrimitiveType GetType() override
	{
		return EPrimitiveType::EPT_Sphere;
	}
protected:
	virtual void InitBoundingBox() override
	{
		FVector Min;
		FVector Max;
		UEngine::Get().GetRenderer()->GetPrimitiveLocalBounds(EPrimitiveType::EPT_Sphere, Min, Max);

		BoundingBox.Init(Min, Max);
	}
};

class UTriangleComp : public UPrimitiveComponent
{
	using Super = UPrimitiveComponent;
public:
	UTriangleComp()
	{
		bCanBeRendered = true;
	}
	virtual ~UTriangleComp() = default;
	EPrimitiveType GetType() override
	{
		return EPrimitiveType::EPT_Triangle;
	}
protected:
	virtual void InitBoundingBox() override
	{
		FVector Min;
		FVector Max;
		UEngine::Get().GetRenderer()->GetPrimitiveLocalBounds(EPrimitiveType::EPT_Triangle, Min, Max);

		BoundingBox.Init(Min, Max);
	}
};

class ULineComp : public UPrimitiveComponent
{
	using Super = UPrimitiveComponent;

public:
	ULineComp()
	{
		bCanBeRendered = true;
	}
	virtual ~ULineComp() = default;
	EPrimitiveType GetType() override
	{
		return EPrimitiveType::EPT_Line;
	}
protected:
	virtual void InitBoundingBox() override
	{
		FVector Min;
		FVector Max;
		UEngine::Get().GetRenderer()->GetPrimitiveLocalBounds(EPrimitiveType::EPT_Line, Min, Max);

		BoundingBox.Init(Min, Max);
	}
};

class UCylinderComp : public UPrimitiveComponent
{
	using Super = UPrimitiveComponent;

public:
	UCylinderComp()
	{
		bCanBeRendered = true;
	}
	virtual ~UCylinderComp() = default;
	EPrimitiveType GetType() override
	{
		return EPrimitiveType::EPT_Cylinder;
	}
protected:
	virtual void InitBoundingBox() override
	{
		FVector Min;
		FVector Max;
		UEngine::Get().GetRenderer()->GetPrimitiveLocalBounds(EPrimitiveType::EPT_Cylinder, Min, Max);

		BoundingBox.Init(Min, Max);
	}
};

class UConeComp : public UPrimitiveComponent
{
	using Super = UPrimitiveComponent;
public:
	UConeComp()
	{
		bCanBeRendered = true;
	}
	virtual ~UConeComp() = default;
	EPrimitiveType GetType() override
	{
		return EPrimitiveType::EPT_Cone;
	}

protected:
	virtual void InitBoundingBox() override
	{
		FVector Min;
		FVector Max;
		UEngine::Get().GetRenderer()->GetPrimitiveLocalBounds(EPrimitiveType::EPT_Cone, Min, Max);

		BoundingBox.Init(Min, Max);
	}
};
