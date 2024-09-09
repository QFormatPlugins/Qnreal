#include "Entities/QEntityActor.h"

void UQEntityComponent::Initialize()
{
	if (EntityData.Properties.Contains("targetname"))
	{
		EntityData.TargetName = EntityData.Properties["targetname"];	
	}
	if (EntityData.Properties.Contains("message"))
	{
		EntityData.Message = EntityData.Properties["message"];	
	}
}

void AQEntityActor::Setup()
{
	
}

AQEntityActor::AQEntityActor()
{
	EntityComponent = CreateDefaultSubobject<UQEntityComponent>(TEXT("QUnrealEntityComponent"));
}
