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

	EPrimitiveType GetType() { return Type; }

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
	void SetBoundingBoxRenderable(bool bRender);

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

protected:
	EPrimitiveType Type = EPrimitiveType::EPT_None;
};

class UCubeComp : public UPrimitiveComponent
{
	using Super = UPrimitiveComponent;
public:
	UCubeComp()
	{
		bCanBeRendered = true;
		Type = EPrimitiveType::EPT_Cube;
	}
	virtual ~UCubeComp() = default;
};
class USphereComp : public UPrimitiveComponent
{
	using Super = UPrimitiveComponent;
public:
	USphereComp()
	{
		bCanBeRendered = true;
		Type = EPrimitiveType::EPT_Sphere;
	}
	virtual ~USphereComp() = default;
};

class UTriangleComp : public UPrimitiveComponent
{
	using Super = UPrimitiveComponent;
public:
	UTriangleComp()
	{
		bCanBeRendered = true;
		Type = EPrimitiveType::EPT_Triangle;
	}
	virtual ~UTriangleComp() = default;
};

class ULineComp : public UPrimitiveComponent
{
	using Super = UPrimitiveComponent;

public:
	ULineComp()
	{
		bCanBeRendered = true;
		Type = EPrimitiveType::EPT_Line;
	}
	virtual ~ULineComp() = default;
};

class UCylinderComp : public UPrimitiveComponent
{
	using Super = UPrimitiveComponent;

public:
	UCylinderComp()
	{
		bCanBeRendered = true;
		Type = EPrimitiveType::EPT_Cylinder;
	}
	virtual ~UCylinderComp() = default;
};

class UConeComp : public UPrimitiveComponent
{
	using Super = UPrimitiveComponent;
public:
	UConeComp()
	{
		bCanBeRendered = true;
		Type = EPrimitiveType::EPT_Cone;
	}
	virtual ~UConeComp() = default;

protected:
};
