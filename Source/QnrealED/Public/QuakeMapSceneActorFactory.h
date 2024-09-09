#pragma once

#include "CoreMinimal.h"
#include "ActorFactories/ActorFactory.h"
#include "QuakeMapSceneActorFactory.generated.h"

UCLASS()
class QNREALED_API UQuakeMapSceneActorFactory : public UActorFactory
{
	GENERATED_BODY()
public:
	UQuakeMapSceneActorFactory(const FObjectInitializer &ObjectInitializer);

	virtual bool CanCreateActorFrom(const FAssetData &AssetData, FText &OutErrorMsg) override;
	virtual void PostSpawnActor(UObject *Asset, AActor *NewActor) override;
	virtual bool CanPlaceElementsFromAssetData(const FAssetData &InAssetData) override;
	virtual void PostPlaceAsset(TArrayView<const FTypedElementHandle> InHandle, const FAssetPlacementInfo &InPlacementInfo, const FPlacementOptions &InPlacementOptions) override;

	virtual AActor *GetDefaultActor(const FAssetData &AssetData) override;
	virtual AActor *SpawnActor(UObject *InAsset, ULevel *InLevel, const FTransform &InTransform, const FActorSpawnParameters &InSpawnParams) override;
};
