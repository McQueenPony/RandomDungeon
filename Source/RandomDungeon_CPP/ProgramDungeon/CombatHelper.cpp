// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatHelper.h"


// Sets default values
ACombatHelper::ACombatHelper()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	auto HandleMonsterDieAll = [&](AMasterRoom* Room)
	{
		Room->OpenTheDoor();
	};

	auto DeclareMonsterDie = [](AMasterRoom* Room)
	{
		GEngine->AddOnScreenDebugMessage(-1,3.0f,FColor::Red,"One Room's Monsters All Died!");
	};
	
	OnMonsterAllDied.AddLambda(HandleMonsterDieAll);
	OnMonsterAllDied.AddLambda(DeclareMonsterDie);
	
}

// Called when the game starts or when spawned
void ACombatHelper::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACombatHelper::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACombatHelper::MonsterDied(AMasterRoom* Room, AMonster* Monster)
{
	MonsterList[Room].Monsters.Remove(Monster);
	MonsterList[Room].MonsterCount--;
	if (MonsterList[Room].MonsterCount <= 0)
	{
		OnMonsterAllDied.Broadcast(Room);
	}
}

