#include "Builders/QMeshBuilder.h"

void FQMeshBuilder::ProcessEntity(const qformats::map::SolidEntityPtr& Entity)
{
	for (const auto& Brush : Entity->GetClippedBrushes())
	{
		for (const auto& Face : Brush.GetFaces())
		{
			if (Face->Type() == qformats::map::Face::SOLID)
			{
				AddFacetoRawMesh(Face, RenderMesh);
			}
		}

		auto width = Brush.max.x() - Brush.min.x();
		auto depht = Brush.max.z() - Brush.min.z();
		auto height = Brush.max.y() - Brush.min.y();
		AreaEstimate += (2 * (width * height + width * depht + height * depht));
	}

	RenderMesh.CompactMaterialIndices();

	OffsetIndex = 0;
	for (const auto& Brush : Entity->GetBrushes())
	{
		for (const auto& Face : Brush.GetFaces())
		{
			if (Face->Type() == qformats::map::Face::CLIP)
			{
				bHasClipMesh = true;
				AddFacetoRawMesh(Face, ClipMesh);
			}
		}
	}
}

void FQMeshBuilder::AddFacetoRawMesh(const qformats::map::FacePtr& Face, FRawMesh& RawMesh)
{
	const auto& vertices = Face->GetVertices();
	const auto& indices = Face->GetIndices();


	for (int i = vertices.size() - 1; i >= 0; i--)
	{
		auto pt = vertices[i].point;
		pt -= VertOffset;
		RawMesh.VertexPositions.Add(FVector3f(
			-pt[0],
			pt[1],
			pt[2]
		) / InverseScale);
	}

	for (int i = 0; i < indices.size() - 2; i += 3)
	{
		auto endv = vertices.size() - 1;
		const auto& pt = vertices[endv - indices[i]];
		const auto& pt2 = vertices[endv - indices[i + 1]];
		const auto& pt3 = vertices[endv - indices[i + 2]];

		RawMesh.WedgeIndices.Add(indices[i] + OffsetIndex);
		RawMesh.WedgeIndices.Add(indices[i + 1] + OffsetIndex);
		RawMesh.WedgeIndices.Add(indices[i + 2] + OffsetIndex);

		RawMesh.WedgeColors.Add(WhiteVertex);
		RawMesh.WedgeColors.Add(WhiteVertex);
		RawMesh.WedgeColors.Add(WhiteVertex);

		RawMesh.WedgeTexCoords[0].Add(FVector2f(pt.uv[0], pt.uv[1]));
		RawMesh.WedgeTexCoords[0].Add(FVector2f(pt2.uv[0], pt2.uv[1]));
		RawMesh.WedgeTexCoords[0].Add(FVector2f(pt3.uv[0], pt3.uv[1]));

		RawMesh.WedgeTangentX.Add(EmptyVector);
		RawMesh.WedgeTangentX.Add(EmptyVector);
		RawMesh.WedgeTangentX.Add(EmptyVector);

		RawMesh.WedgeTangentY.Add(EmptyVector);
		RawMesh.WedgeTangentY.Add(EmptyVector);
		RawMesh.WedgeTangentY.Add(EmptyVector);

		RawMesh.WedgeTangentZ.Add(FVector3f(pt.normal[0], pt.normal[1], pt.normal[2]));
		RawMesh.WedgeTangentZ.Add(FVector3f(pt2.normal[0], pt2.normal[1], pt2.normal[2]));
		RawMesh.WedgeTangentZ.Add(FVector3f(pt3.normal[0], pt3.normal[1], pt3.normal[2]));

		auto TexID = Face->TextureID();
		if (!MatIDMap.Contains(TexID))
		{
			if (TexID < Materials.Num())
			{
				TempMaterials.Emplace(Materials[TexID]);
				MatIDMap.Add(TexID, MaterialIndex++);
			}
		}
		RawMesh.FaceMaterialIndices.Add(MatIDMap[TexID]);
		RawMesh.FaceSmoothingMasks.Add(0);
	}
	OffsetIndex += vertices.size();
}

void FQMeshBuilder::SetupRenderSourceModel(UStaticMesh* Mesh, float LightMapDivider, float MaxLightmapSize)
{
	if (RenderMesh.VertexPositions.Num() == 0)
	{
		return;
	}

	for (auto Mat : TempMaterials)
	{
		Mesh->AddMaterial(Mat);
	}

	Mesh->AddSourceModel();
	auto& SrcModel = Mesh->GetSourceModel(0);
	SrcModel.BuildSettings.bUseHighPrecisionTangentBasis = true;
	SrcModel.BuildSettings.bRecomputeNormals = true;
	SrcModel.BuildSettings.bRecomputeTangents = true;
	SrcModel.BuildSettings.bUseFullPrecisionUVs = true;
	SrcModel.BuildSettings.bGenerateLightmapUVs = true;

	auto LightMapEstimate = AreaEstimate / LightMapDivider;
	if (LightMapEstimate > MaxLightmapSize)
	{
		LightMapEstimate = MaxLightmapSize;
	}

	if (LightMapEstimate < 32)
	{
		LightMapEstimate = 32;
	}
	SrcModel.BuildSettings.MinLightmapResolution = LightMapEstimate;
	SrcModel.BuildSettings.DstLightmapIndex = 0;
	SrcModel.BuildSettings.DstLightmapIndex = 1;
	SrcModel.BuildSettings.bComputeWeightedNormals = true;

	// Get LODGroup info
	ITargetPlatformManagerModule& TargetPlatformManager = GetTargetPlatformManagerRef();
	ITargetPlatform* RunningPlatform = TargetPlatformManager.GetRunningTargetPlatform();
	check(RunningPlatform);

	const FStaticMeshLODSettings& LODSettings = RunningPlatform->GetStaticMeshLODSettings();
	const auto& NewGroup = LODSettings.GetLODGroup(NAME_None);
	SrcModel.ReductionSettings = NewGroup.GetDefaultSettings(0);
	SrcModel.ReductionSettings.bKeepSymmetry = true;
	SrcModel.ReductionSettings.bCullOccluded = true;

	SrcModel.RawMeshBulkData->SaveRawMesh(RenderMesh);
	SrcModel.CreateSubObjects(Mesh);
	Mesh->ImportVersion = EImportStaticMeshVersion::LastVersion;
	Mesh->SetLightMapCoordinateIndex(1);
	Mesh->SetLightMapResolution(LightMapEstimate);
	Mesh->bCustomizedCollision = false;
	Mesh->ComplexCollisionMesh = Mesh;
	Mesh->SetLightingGuid();
}

void FQMeshBuilder::SetupClippingSourceModel(UStaticMesh* Mesh)
{
	if (ClipMesh.VertexPositions.Num() == 0)
	{
		return;
	}

	Mesh->AddSourceModel();
	auto& SrcModel = Mesh->GetSourceModel(0);
	SrcModel.BuildSettings.bUseHighPrecisionTangentBasis = false;
	SrcModel.BuildSettings.bRecomputeNormals = false;
	SrcModel.BuildSettings.bRecomputeTangents = false;
	SrcModel.BuildSettings.bUseFullPrecisionUVs = false;
	SrcModel.BuildSettings.bGenerateLightmapUVs = false;
	SrcModel.BuildSettings.MinLightmapResolution = 0;
	SrcModel.BuildSettings.bComputeWeightedNormals = true;

	// Get LODGroup info
	ITargetPlatformManagerModule& TargetPlatformManager = GetTargetPlatformManagerRef();
	ITargetPlatform* RunningPlatform = TargetPlatformManager.GetRunningTargetPlatform();
	check(RunningPlatform);

	const FStaticMeshLODSettings& LODSettings = RunningPlatform->GetStaticMeshLODSettings();
	const auto& NewGroup = LODSettings.GetLODGroup(NAME_None);
	SrcModel.ReductionSettings = NewGroup.GetDefaultSettings(0);
	SrcModel.ReductionSettings.bKeepSymmetry = true;
	SrcModel.ReductionSettings.bCullOccluded = true;

	SrcModel.RawMeshBulkData->SaveRawMesh(ClipMesh);
	SrcModel.CreateSubObjects(Mesh);

	Mesh->ImportVersion = LastVersion;
	Mesh->SetLightMapResolution(0);
	Mesh->bCustomizedCollision = false;
	Mesh->ComplexCollisionMesh = Mesh;
}
