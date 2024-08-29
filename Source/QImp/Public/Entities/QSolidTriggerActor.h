#pragma once
#include "QSolidEntityActor.h"
#include "QSolidTriggerActor.generated.h"

UCLASS(Blueprintable)
class QIMP_API AQSolidTriggerActor : public AQSolidEntityActor
{
	GENERATED_BODY()
public:
	AQSolidTriggerActor();
	virtual void Setup() override;
	UFUNCTION()
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, Category="Trigger")
	bool bOnce = false;
	UPROPERTY(EditAnywhere, Category="Trigger")
	bool bTeleport = false;
	UPROPERTY(EditAnywhere, Category="Trigger")
	bool bTriggered = false;
	UPROPERTY(EditAnywhere, Category="Trigger")
	FString TriggerTarget;
	
};
