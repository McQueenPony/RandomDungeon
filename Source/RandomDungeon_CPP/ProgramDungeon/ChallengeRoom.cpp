// Fill out your copyright notice in the Description page of Project Settings.


#include "ChallengeRoom.h"


// Sets default values
AChallengeRoom::AChallengeRoom()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TrapSpawnRoot = CreateDefaultSubobject<USceneComponent>("TrapSpawnRoot");
	TrapSpawnRoot ->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void AChallengeRoom::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AChallengeRoom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AChallengeRoom::MakeTrapMap_Implementation()
{
}

