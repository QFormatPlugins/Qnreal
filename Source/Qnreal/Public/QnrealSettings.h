// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Entities/QEntityClassesData.h"
#include "QnrealSettings.generated.h"

UENUM(Blueprintable)
enum EQuakeEntityPivot
{
	Lower_Left = 0,
	Upper_Left = 1,
	Lower_Right = 2,
	Upper_Right = 3,
	Center = 4,
	Lower_Center = 5,
	Upper_Center = 6,
};

USTRUCT(Blueprintable)
struct QNREAL_API FQuakeMapAssetOptions
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "General")
	UQEntityClassesData *EntityClassOverrides = nullptr;

	UPROPERTY(EditAnywhere, Category = "General")
	UMaterial *BaseMaterial = nullptr;

	UPROPERTY(EditAnywhere, Category = "General")
	float InverseScale = 1;

	UPROPERTY(EditAnywhere, Category = "General")
	TEnumAsByte<EQuakeEntityPivot> DefaultPivot = Lower_Left;

	UPROPERTY(EditAnywhere, Category = "LightMap")
	float MaxLightmapSize = 512;

	UPROPERTY(EditAnywhere, Category = "LightMap")
	float LightMapDivider = 2600;

	UPROPERTY(EditAnywhere, Category = "Materials and Textures")
	FName TextureFolder = "";

	UPROPERTY(EditAnywhere, Category = "Materials and Textures")
	FName MaterialOverrideFolder = "";

	UPROPERTY(EditAnywhere, Category = "Materials and Textures")
	FString SkipTexture = "skip";

	UPROPERTY(EditAnywhere, Category = "Materials and Textures")
	FString ClipTexture = "clip";

	UPROPERTY(EditAnywhere, Category = "Materials and Textures")
	FString SkyTexture = "sky";
};

USTRUCT(Blueprintable)
struct QNREAL_API FQuakeWadAssetOptions
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "General")
	bool bExpotTextures = true;
};

UCLASS(Config = QnrealEditorSettings)
class QNREAL_API UQnrealSettings : public UObject
{
	GENERATED_BODY()

public:
	UQnrealSettings(const FObjectInitializer &Obj) {}

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly)
	FQuakeMapAssetOptions MapAssetOptions;
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly)
	FQuakeWadAssetOptions WadAssetOptions;
	virtual void Serialize(FArchive &Ar) override;
	UPROPERTY(Config)
	bool bIsInitialized = false;
};

// TODO: Maybe not needed
inline void UQnrealSettings::Serialize(FArchive &Ar)
{
	Ar << bIsInitialized;
	UObject::Serialize(Ar);
}
