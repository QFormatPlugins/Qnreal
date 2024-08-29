#include "Entities/QSolidEntityActor.h"
#include "PhysicsEngine/BodySetup.h"

AQSolidEntityActor::AQSolidEntityActor()
{
	EntityMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EntityMesh"));
	RootComponent = EntityMeshComponent;
	ClipMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ClipMesh"));
	ClipMeshComponent->SetupAttachment(CastChecked<USceneComponent, UStaticMeshComponent>(EntityMeshComponent));
	ClipMeshComponent->bHiddenInGame = true;
	ClipMeshComponent->SetActive(false);
	PrimaryActorTick.bCanEverTick = false;
}

void AQSolidEntityActor::Setup()
{
	if (EntityMeshComponent->GetStaticMesh()->IsValidLowLevelFast() && EntityMeshComponent->GetStaticMesh()->IsSourceModelValid(0))
	{
		EntityMeshComponent->UpdateCollisionFromStaticMesh();
		EntityMeshComponent->GetBodySetup()->CollisionTraceFlag = ECollisionTraceFlag::CTF_UseComplexAsSimple;
		EntityMeshComponent->SetMobility(Mobility);
		EntityMeshComponent->SetVisibility(true);
		EntityMeshComponent->bHiddenInGame = false;
	} else
	{
		EntityMeshComponent->UnregisterComponent();
	}

	if (ClipMeshComponent->GetStaticMesh()->IsValidLowLevelFast() && ClipMeshComponent->GetStaticMesh()->IsSourceModelValid(0))
	{
		ClipMeshComponent->SetActive(true);
		ClipMeshComponent->UpdateCollisionFromStaticMesh();
		ClipMeshComponent->GetBodySetup()->CollisionTraceFlag = ECollisionTraceFlag::CTF_UseComplexAsSimple;
		ClipMeshComponent->SetMobility(Mobility);
		ClipMeshComponent->SetVisibility(true);
		ClipMeshComponent->bHiddenInGame = true;
		ClipMeshComponent->SetRelativeLocation(EntityMeshComponent->GetRelativeLocation());
	} else
	{
		ClipMeshComponent->UnregisterComponent();
	}
}
