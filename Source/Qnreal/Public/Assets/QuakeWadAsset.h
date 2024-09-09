#pragma once

#include <CoreMinimal.h>

#include "WadTexture2D.h"
#include <qwad/wad.h>
#include "AssetRegistry/AssetRegistryModule.h"

#include <QuakeWadAsset.generated.h>

UCLASS()
class QNREAL_API UQuakeWadAsset : public UObject
{
	GENERATED_BODY()
public:
	UQuakeWadAsset();
	virtual ~UQuakeWadAsset() override;
	void LoadWadFromFile(FString FileName);

	UPROPERTY(VisibleAnywhere)
	FString SourceQWadFile;
	UPROPERTY(EditAnywhere, meta = (TitleProperty = "CleanName"))
	TArray<FWadTexture2D> Textures;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bExternalTextures = true;

private:
	qformats::wad::qwad_ptr WadFile;
	bool bAlreadyInManager = false;
	friend class FWadManager;
};

class FWadManager
{
public:
	static FWadManager *GetInstance();
	void Refresh(const FAssetRegistryModule &AssetRegistryModule);
	void AddWadAsset(UQuakeWadAsset *Wa);
	void RemoveWad(UQuakeWadAsset *Wa);
	UTexture2D *FindTexture(FName Name);
	UTexture2D *FindTexture(const char *name) { return FindTexture(FName(name)); }

private:
	static FWadManager *Instance;
	std::vector<UQuakeWadAsset *> WadAssets;
	std::map<FString, UTexture2D *> TextureCache;
};