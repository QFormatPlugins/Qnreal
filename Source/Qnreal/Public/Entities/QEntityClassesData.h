#pragma once

#include <CoreMinimal.h>
#include "QEntityClassesData.generated.h"

class AQEntityActor;

USTRUCT()
struct FQEntityClassesDataEntry
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	FName ClassName;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> Class;
};

UCLASS(BlueprintType, Category = "QUnreal")
class QNREAL_API UQEntityClassesData : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, meta = (TitleProperty = "ClassName"))
	TMap<FString, TSubclassOf<AActor>> Classes;
};
