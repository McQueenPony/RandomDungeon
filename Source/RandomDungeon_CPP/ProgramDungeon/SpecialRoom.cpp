// Fill out your copyright notice in the Description page of Project Settings.


#include "SpecialRoom.h"


// Sets default values
ASpecialRoom::ASpecialRoom()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASpecialRoom::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpecialRoom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ASpecialRoom::GetIsBOSSRoom()
{
	return IsBOSSRoom;
}

