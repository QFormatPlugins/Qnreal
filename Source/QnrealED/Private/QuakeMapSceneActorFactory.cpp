#include "QuakeMapSceneActorFactory.h"
#include <Assets/QuakeMapAsset.h>

#include "Entities/QWorldSpawnActor.h"
#include "PhysicsEngine/BodySetup.h"


UQuakeMapSceneActorFactory::UQuakeMapSceneActorFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DisplayName = FText::FromString(TEXT("Map Scene"));
	NewActorClass = AQWorldSpawnActor::StaticClass();
	bUsePlacementExtent = true;
}

bool UQuakeMapSceneActorFactory::CanCreateActorFrom(const FAssetData& AssetData, FText& OutErrorMsg) {

	if (AssetData.IsValid() && !AssetData.GetClass()->IsChildOf(AQWorldSpawnActor::StaticClass()))
	{
		OutErrorMsg = FText::FromString(TEXT("MAP Asset is not valid"));
		return false;
	}

	return true;
}

void UQuakeMapSceneActorFactory::PostSpawnActor(UObject *Asset, AActor *NewActor)
{
	UQuakeMapAsset *QMAsset = Cast<UQuakeMapAsset>(Asset);

	if (QMAsset != nullptr)
	{
		FString ActorName = "WorldSpawn_";
		ActorName.Append(QMAsset->GetName());
		AQWorldSpawnActor *NewWSActor = CastChecked<AQWorldSpawnActor>(NewActor);
		FActorLabelUtilities::SetActorLabelUnique(NewWSActor, ActorName);
		NewWSActor->MapData = QMAsset->MapData;
	}
}

bool UQuakeMapSceneActorFactory::CanPlaceElementsFromAssetData(const FAssetData& InAssetData)
{
	return true;
}

AActor* UQuakeMapSceneActorFactory::GetDefaultActor(const FAssetData& AssetData)
{
	
	auto newWorldSpawn = NewActorClass->GetDefaultObject<AQWorldSpawnActor>();
	return newWorldSpawn;
}

AActor* UQuakeMapSceneActorFactory::SpawnActor(UObject* InAsset, ULevel* InLevel, const FTransform& InTransform, const FActorSpawnParameters& InSpawnParams)
{
	AQWorldSpawnActor* actor;
	{
		actor = Cast<AQWorldSpawnActor>(Super::SpawnActor(InAsset, InLevel, InTransform, InSpawnParams));
	}

	auto MapAsset = Cast<UQuakeMapAsset>(InAsset);
	actor->MapData = MapAsset->MapData;
	actor->SetPivotOffset(MapAsset->MapData->WorldSpawn.Pivot);
	
	if (actor->HasAnyFlags(RF_Transient))
	{
		return actor;
	}

	if (actor->SolidEntities.Num() > 0) return actor;
	int meshNum = 0;
	actor->SetFolderPath(*MapAsset->GetName());
	MapAsset->MapData->QuakeMapUpdated.AddUObject(actor, &AQWorldSpawnActor::OnQuakeMapUpdated);
	actor->ReloadFromAsset();
	return actor;
}

void UQuakeMapSceneActorFactory::PostPlaceAsset(TArrayView<const FTypedElementHandle> InHandle,
	const FAssetPlacementInfo& InPlacementInfo, const FPlacementOptions& InPlacementOptions)
{
	Super::PostPlaceAsset(InHandle, InPlacementInfo, InPlacementOptions);
}
