#pragma once

#include <CoreMinimal.h>
#include <qwad/wad.h>
#include "WadTexture2D.generated.h"

USTRUCT()
struct FWadTexture2D
{
	GENERATED_BODY()

	void SetName(FString Name);
	void Generate(const FString &PackagePath, const qformats::wad::QuakeTexture *Qtex);
	static FString ToCleanName(const FString &OriginalName);
	
	UPROPERTY(VisibleAnywhere) FString OriginalName;
	UPROPERTY(VisibleAnywhere) FString CleanName;
	UPROPERTY(VisibleAnywhere) FString WadName;
	UPROPERTY(VisibleAnywhere) UTexture2D *Texture = nullptr;
};
