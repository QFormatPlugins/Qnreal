#pragma once

#include "QuakeMapData.generated.h"

UENUM()
enum EEntityType
{
	EntityType_Point = 0,
	EntityType_Solid = 1,
};

USTRUCT()
struct FEntity
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(VisibleAnywhere) TEnumAsByte<EEntityType> Type = EntityType_Point;
	UPROPERTY(EditAnywhere) FString UniqueClassName = "";
	UPROPERTY(VisibleAnywhere) FString ClassName = "";
	UPROPERTY(VisibleAnywhere) FVector3d Origin{};
	UPROPERTY(VisibleAnywhere) float Angle = 0;
	UPROPERTY(VisibleAnywhere) TMap<FString,FString> Properties{};
	UPROPERTY(VisibleAnywhere) TSubclassOf<AActor> ClassTemplate = nullptr;
};

USTRUCT()
struct FSolidEntity : public FEntity
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(VisibleAnywhere) UStaticMesh *Mesh = nullptr;
	UPROPERTY(VisibleAnywhere) UStaticMesh *ClipMesh = nullptr;
	UPROPERTY(VisibleAnywhere) FVector3d Center{};
	UPROPERTY(VisibleAnywhere) FVector3d Pivot{};
	UPROPERTY(VisibleAnywhere) bool bVisibleInGame=true;
};

UCLASS()
class QIMP_API UQuakeMapData : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere) FSolidEntity WorldSpawn;
	UPROPERTY(EditAnywhere, meta = (TitleProperty = "UniqueClassName"))
	TArray<FSolidEntity> SolidEntities;
	UPROPERTY(EditAnywhere, meta = (TitleProperty = "UniqueClassName"))
	TArray<FEntity> PointEntities;

	bool bImportLights = false;
	float InverseScale = 1.0;

	FSimpleMulticastDelegate QuakeMapUpdated;
};
