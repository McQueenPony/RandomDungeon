// Fill out your copyright notice in the Description page of Project Settings.


#include "MasterRoom.h"


// Sets default values
AMasterRoom::AMasterRoom()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	InitializeRoomComponents();
	SetComponentRelationship();
	
}

// Called when the game starts or when spawned
void AMasterRoom::BeginPlay()
{
	Super::BeginPlay();

	
	
	
}

void AMasterRoom::SetComponentRelationship() 
{
	GeometryFolder->SetupAttachment(Root);
	Arrow->SetupAttachment(Root);
	OverlapFolder->SetupAttachment(Root);
	ExitsFolder->SetupAttachment(Root);
	Floor->SetupAttachment(Root);
	Air->SetupAttachment(Root);
	BoxCollision->SetupAttachment(Root);
	FloorSpawnFolder->SetupAttachment(Root);
	AirSpawnFolder->SetupAttachment(Root);
	BoxCollision->SetupAttachment(OverlapFolder);
	FloorSpawnFolder->SetupAttachment(Floor);
	AirSpawnFolder->SetupAttachment(Air);
	MonsterSpawnRoot->SetupAttachment(Floor);
	ChestSpawnRoot->SetupAttachment(Floor);
}

void AMasterRoom::InitializeRoomComponents()
{
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);
	GeometryFolder = CreateDefaultSubobject<USceneComponent>(TEXT("GeometryFolder"));
	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	OverlapFolder = CreateDefaultSubobject<USceneComponent>(TEXT("OverlapFolder"));
	ExitsFolder = CreateDefaultSubobject<USceneComponent>(TEXT("ExitsFolder"));
	FloorSpawnFolder = CreateDefaultSubobject<USceneComponent>(TEXT("FloorSpawnFolder"));
	AirSpawnFolder = CreateDefaultSubobject<USceneComponent>(TEXT("AirSpawnFolder"));
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	Floor = CreateDefaultSubobject<USceneComponent>(TEXT("Floor"));
	Air = CreateDefaultSubobject<USceneComponent>(TEXT("Air"));
	MonsterSpawnRoot = CreateDefaultSubobject<USceneComponent>(TEXT("MonsterSpawn"));
	ChestSpawnRoot = CreateDefaultSubobject<USceneComponent>(TEXT("ChestSpawn"));
}

// Called every frame
void AMasterRoom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AMasterRoom::OpenTheDoor()
{
	for (auto Element : DoorExistList)
	{
		Element->OpenDoor();
	}
}

AMasterRoom* AMasterRoom::GetRoom()
{
	return this;
}

void AMasterRoom::LightTheRoad_Implementation()
{}



