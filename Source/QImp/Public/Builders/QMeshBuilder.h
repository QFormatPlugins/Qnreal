#pragma once

#include <qmap/entities.h>
#include "RawMesh.h"

class FQMeshBuilder
{
public:
	FQMeshBuilder(const TArray<UMaterialInstanceDynamic*>& Materials)
		: Materials(Materials)
	{
	}

	void SetVertexOffset(const fvec3 Offset) { VertOffset = Offset; }
	void SetInverseScale(const float Scale) { InverseScale = Scale; }
	void ProcessEntity(const qformats::map::SolidEntityPtr &Entity);
	void SetupRenderSourceModel(UStaticMesh *Mesh, float LightMapDivider, float MaxLightmapSize );
	void SetupClippingSourceModel(UStaticMesh *Mesh);
	bool HasClipMesh() const {return bHasClipMesh; }
	bool HasRenderMesh() const {return RenderMesh.VertexPositions.Num() > 0;}
	double GetAreaEstimate() const { return AreaEstimate; }
private:
	void AddFacetoRawMesh(const qformats::map::FacePtr &Face, FRawMesh &RawMesh);
	
	FVector3f EmptyVector = FVector3f(0, 0, 0);
	FColor WhiteVertex = FColor(255, 255, 255, 255);
	fvec3 VertOffset{};
	double AreaEstimate = 0;
	float InverseScale = 1;
	bool bHasClipMesh = false;
	TArray<UMaterialInstanceDynamic*> TempMaterials;
	TMap<int32_t, int32_t> MatIDMap;
	int OffsetIndex = 0;
	int MaterialIndex = 0;
	

	const TArray<UMaterialInstanceDynamic*> &Materials;
	FRawMesh RenderMesh{};
	FRawMesh ClipMesh{};
};
