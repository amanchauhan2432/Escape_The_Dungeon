// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/AudioComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "OpenDoor.h"
#include "GameFramework/Actor.h"


#define OUT

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();

	FindAudioComponent();
}

void UOpenDoor::FindAudioComponent()
{
	AudioComponent = GetOwner()->FindComponentByClass<UAudioComponent>();
	if(!AudioComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("No Audio Component Found"));
	}
}

// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(MassOfOverlappedActor() >= MassToOpenDoors)
	{
		OpenDoor(DeltaTime);
		DoorLastOpened = GetWorld()->GetTimeSeconds();
	}
	else
		if(GetWorld()->GetTimeSeconds() - DoorLastOpened > DoorCloseDelay)
		{
			CloseDoor(DeltaTime);
		}
}

void UOpenDoor::OpenDoor(float DeltaTime)
{
	FVector Current = GetOwner()->GetActorLocation();
	FVector OpenDoor(Current.X, Current.Y, OpenTarget);
	OpenDoor.Z = FMath::Lerp(Current.Z, OpenTarget, DeltaTime * DoorOpenSpeed);
	GetOwner()->SetActorLocation(OpenDoor);

	CloseDoorSound = false;
	if(!AudioComponent) {return;}
	if(!OpenDoorSound)
	{
		AudioComponent->Play();
		OpenDoorSound = true;
	}
}

void UOpenDoor::CloseDoor(float DeltaTime)
{
	FVector Current = GetOwner()->GetActorLocation();
	FVector CloseDoor(Current.X, Current.Y, CloseTarget);
	CloseDoor.Z = FMath::Lerp(Current.Z, CloseTarget, DeltaTime * DoorCloseSpeed);
	GetOwner()->SetActorLocation(CloseDoor);

	OpenDoorSound = false;
	if(!AudioComponent) {return;}
	if(!CloseDoorSound)
	{
		AudioComponent->Play();
		CloseDoorSound = true;
	}
}

float UOpenDoor::MassOfOverlappedActor() const
{
	float Mass = 0.f;
	if(PressurePlate)
	{
		TArray<AActor*> OverlappingActors;
		PressurePlate->GetOverlappingActors(OverlappingActors);

		if (OverlappingActors.GetData())
		{
			AActor* OverlappedActor = OverlappingActors[0];
			Mass = OverlappedActor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
		}
	}
	return Mass;
}