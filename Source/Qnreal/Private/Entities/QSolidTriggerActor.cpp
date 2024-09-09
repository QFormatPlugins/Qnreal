#include "Entities/QSolidTriggerActor.h"
#include "Entities/QWorldSpawnActor.h"

AQSolidTriggerActor::AQSolidTriggerActor()
{
}

void AQSolidTriggerActor::Setup()
{
	Super::Setup();
	if (EntityMeshComponent->IsValidLowLevel())
	{
		EntityMeshComponent->SetVisibility(false);
		EntityMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		EntityMeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
		EntityMeshComponent->SetMobility(EComponentMobility::Movable);
		EntityMeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		EntityMeshComponent->bHiddenInGame = true;

		bOnce = EntityComponent->EntityData.ClassName.Contains("once");
		bTeleport = EntityComponent->EntityData.ClassName.Contains("teleport");
		if (EntityComponent->EntityData.Properties.Contains("target"))
		{
			TriggerTarget = EntityComponent->EntityData.Properties["target"];
		}
	}
}

void AQSolidTriggerActor::BeginPlay()
{
}

void AQSolidTriggerActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	AQWorldSpawnActor* WorldSpawn = Cast<AQWorldSpawnActor>(GetOwner());
	if (WorldSpawn != nullptr && WorldSpawn->TriggerTargets.Contains(TriggerTarget))
	{
		for (const auto& Targets = WorldSpawn->TriggerTargets[TriggerTarget].Targets; const auto& Target : Targets)
		{
			if (Target == nullptr)
			{
				continue;
			}

			if (Target->GetClass()->ImplementsInterface(UQEntityEvents::StaticClass()))
			{
				Execute_OnEntityGotTriggered(Target, OtherActor, this);
			}
		}
	}

	Super::NotifyActorBeginOverlap(OtherActor);
}
