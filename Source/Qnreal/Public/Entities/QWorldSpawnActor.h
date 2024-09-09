#pragma once
#include "QEntityActor.h"
#include "QuakeMapData.h"

#include "QSolidEntityActor.h"
#include "QWorldSpawnActor.generated.h"

USTRUCT(Blueprintable)
struct FTriggerTargets
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	TArray<AActor *> Targets;
};

UCLASS()
class QNREAL_API AQWorldSpawnActor : public AActor
{
	GENERATED_BODY()
public:
	AQWorldSpawnActor();
	virtual void BeginDestroy() override;
	virtual void Destroyed() override;

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "QuakeMapAsset")
	UQuakeMapData *MapData;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Entities")
	TArray<AActor *> SolidEntities;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Entities")
	TArray<AActor *> PointEntities;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Entities")
	TMap<FString, FTriggerTargets> TriggerTargets;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UStaticMeshComponent *WorldSpawnMeshComponent;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UStaticMeshComponent *WorldSpawnClipMeshComponent;

	virtual void OnConstruction(const FTransform &Transform) override;
	void OnQuakeMapUpdated();
	void ReloadFromAsset();
	virtual void PostRegisterAllComponents() override;

private:
	bool bAlreadyPostRegistered = false;
	void SetupMeshComponent() const;
	void ImportPointEntity(const FEntity &Entity);
	void SetupEntityComponent(AActor *Actor, FName Name, const FEntity &Entity);
};
