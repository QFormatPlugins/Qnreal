#include "Entities/QWorldSpawnActor.h"
#include "Entities/QSolidEntityActor.h"
#include "PhysicsEngine/BodySetup.h"

// Sets default values
AQWorldSpawnActor::AQWorldSpawnActor()
{
	WorldSpawnMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WorldSpawnMesh"));
	RootComponent = WorldSpawnMeshComponent;
	WorldSpawnMeshComponent->SetMobility(EComponentMobility::Static);
	WorldSpawnClipMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WorldSpawnClipMesh"));
	WorldSpawnClipMeshComponent->SetMobility(EComponentMobility::Static);
	WorldSpawnClipMeshComponent->bHiddenInGame = true;
	WorldSpawnClipMeshComponent->SetVisibility(false);
	PrimaryActorTick.bCanEverTick = false;
}

void AQWorldSpawnActor::BeginDestroy()
{
	SolidEntities.Empty();
	Super::BeginDestroy();
}

void AQWorldSpawnActor::Destroyed()
{
	TArray<AActor*> Actors;
	GetAttachedActors(Actors);

	for (auto Actor : Actors)
	{
		Actor->Destroy();
	}
	Super::Destroyed();
}

void AQWorldSpawnActor::OnConstruction(const FTransform& Transform)
{
#ifdef UE_EDITOR
	SetupMeshComponent();
#endif
}

void AQWorldSpawnActor::ReloadFromAsset()
{
	if (!MapData->IsValidLowLevel() || !MapData->WorldSpawn.Mesh->IsValidLowLevel())
	{
		return;
	}

	WorldSpawnMeshComponent->SetStaticMesh(MapData->WorldSpawn.Mesh);
	if (MapData->WorldSpawn.ClipMesh != nullptr && MapData->WorldSpawn.ClipMesh->IsValidLowLevelFast())
	{
		WorldSpawnClipMeshComponent->SetStaticMesh(MapData->WorldSpawn.ClipMesh);
	}
	SetupMeshComponent();

	for (auto Actor : SolidEntities)
	{
		if (Actor->IsValidLowLevelFast())
		{
			Actor->Destroy();
			Actor->Owner = nullptr;	
		}
	}

	for (auto Actor : PointEntities)
	{
		Actor->Destroy();
		Actor->Owner = nullptr;
	}

	SolidEntities.Empty();
	PointEntities.Empty();
	TriggerTargets.Empty();

	for (int i = 0; i < MapData->SolidEntities.Num(); i++)
	{
		auto Entity = MapData->SolidEntities[i];
		auto MeshStr = FString("AS_" + Entity.UniqueClassName);
		FActorSpawnParameters p;
		p.Owner = this;

		auto EntityActor = GetWorld()->SpawnActor<AQSolidEntityActor>(
			Entity.ClassTemplate, GetTransform(), p);

		if (EntityActor == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Could not spawn EntityActor (%s)"), *Entity.UniqueClassName);
			continue;
		}


#if WITH_EDITOR
		FActorLabelUtilities::SetActorLabelUnique(EntityActor, MeshStr);
		EntityActor->SetPivotOffset(Entity.Pivot);
#endif
		SetupEntityComponent(EntityActor, *MeshStr, Entity);
		EntityActor->EntityMeshComponent->SetStaticMesh(Entity.Mesh);
		EntityActor->ClipMeshComponent->SetStaticMesh(Entity.ClipMesh);
		EntityActor->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		EntityActor->Setup();
		SolidEntities.Emplace(EntityActor);
	}

	for (int i = 0; i < MapData->PointEntities.Num(); i++)
	{
		auto Entity = MapData->PointEntities[i];
		ImportPointEntity(Entity);
	}
}

void AQWorldSpawnActor::PostRegisterAllComponents()
{
	if (!GetWorld()->IsEditorWorld() || GetWorld()->IsPlayInEditor() || MapData == nullptr)
	{
		return;
	}
#if WITH_EDITOR
	if (!bAlreadyPostRegistered && MapData->WorldSpawn.Mesh->IsValidLowLevel() && !HasAnyFlags(RF_Transient))
	{
		bAlreadyPostRegistered = true;
		UE_LOG(LogTemp, Warning, TEXT("registered all components, REROLL!!!"));
		ReloadFromAsset();
		MapData->QuakeMapUpdated.AddUObject(this, &AQWorldSpawnActor::OnQuakeMapUpdated);
	}
#endif

	Super::PostRegisterAllComponents();
}

void AQWorldSpawnActor::SetupMeshComponent() const
{
	if (MapData->IsValidLowLevel())
	{
		auto RenderMesh = WorldSpawnMeshComponent->GetStaticMesh();
		if (RenderMesh && RenderMesh->IsValidLowLevelFast() && RenderMesh->IsSourceModelValid(0))
		{
			WorldSpawnMeshComponent->SetMobility(EComponentMobility::Static);
			WorldSpawnMeshComponent->SetStaticMesh(MapData->WorldSpawn.Mesh);
			WorldSpawnMeshComponent->GetBodySetup()->CollisionTraceFlag = ECollisionTraceFlag::CTF_UseComplexAsSimple;
			WorldSpawnMeshComponent->UpdateCollisionFromStaticMesh();
		} else
		{
			WorldSpawnMeshComponent->UnregisterComponent();
		}

		auto ClipMesh = WorldSpawnClipMeshComponent->GetStaticMesh();
		if (ClipMesh && ClipMesh->IsValidLowLevelFast() && ClipMesh->IsSourceModelValid(0))
		{
			WorldSpawnClipMeshComponent->SetMobility(EComponentMobility::Static);
			WorldSpawnClipMeshComponent->SetStaticMesh(MapData->WorldSpawn.ClipMesh);
			WorldSpawnClipMeshComponent->GetBodySetup()->CollisionTraceFlag = CTF_UseComplexAsSimple;
			WorldSpawnClipMeshComponent->UpdateCollisionFromStaticMesh();
			WorldSpawnClipMeshComponent->SetRelativeLocation(WorldSpawnMeshComponent->GetRelativeLocation());
		} else
		{
			WorldSpawnClipMeshComponent->UnregisterComponent();
		}
	}
}

void AQWorldSpawnActor::ImportPointEntity(const FEntity& Entity)
{
	auto Name = FString("AP_" + Entity.UniqueClassName);
	FActorSpawnParameters p;
	p.Owner = this;

	auto Trans = Entity.Origin + GetTransform().GetLocation();
	auto PtActor = GetWorld()->SpawnActor<AActor>(Entity.ClassTemplate, FTransform3d(Trans), p);

	if (PtActor == nullptr)
	{
		return;
	}

	SetupEntityComponent(PtActor, *Name, Entity);

	auto Angle = Entity.Angle;
	if (Angle != 270 && Angle != 90)
	{
		Angle += 180;
	}

	PtActor->SetActorRotation(FRotator(0, Angle, 0));

#if WITH_EDITOR
	FActorLabelUtilities::SetActorLabelUnique(PtActor, Name);
#endif

	PtActor->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	//PtActor->SetMobility(EComponentMobility::Static);
	PointEntities.Emplace(PtActor);
}

void AQWorldSpawnActor::SetupEntityComponent(AActor* Actor, FName Name, const FEntity& Entity)
{
	auto EntityComponent = Actor->FindComponentByClass<UQEntityComponent>();
	if (EntityComponent->IsValidLowLevel())
	{
		EntityComponent->EntityData.EntityName = (Name);
		EntityComponent->EntityData.ClassName = Entity.ClassName;
		EntityComponent->EntityData.Properties = Entity.Properties;
		EntityComponent->EntityData.Angle = Entity.Angle;
		EntityComponent->Initialize();

		if (Actor->GetClass()->ImplementsInterface(UQEntityEvents::StaticClass()) &&
			EntityComponent->EntityData.TargetName != "")
		{
			if (TriggerTargets.Contains(EntityComponent->EntityData.TargetName))
			{
				TriggerTargets[EntityComponent->EntityData.TargetName].Targets.Add(Actor);
			}
			else
			{
				TriggerTargets.Add(EntityComponent->EntityData.TargetName, {{Actor}});
			}
		}
	}
}

void AQWorldSpawnActor::OnQuakeMapUpdated()
{
#ifdef UE_EDITOR
	UE_LOG(LogTemp, Warning, TEXT("map updated event, REROLL!!!"));
	ReloadFromAsset();
#endif
}
