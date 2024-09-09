#pragma once

#include <CoreMinimal.h>
#include "QEntityActor.h"
#include "QSolidEntityActor.generated.h"

UCLASS(Blueprintable)
class QNREAL_API AQSolidEntityActor : public AQEntityActor
{
	GENERATED_BODY()
public:
	AQSolidEntityActor();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TEnumAsByte<EComponentMobility::Type> Mobility = EComponentMobility::Static;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UStaticMeshComponent *EntityMeshComponent;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UStaticMeshComponent *ClipMeshComponent;

	virtual void Setup() override;
};
