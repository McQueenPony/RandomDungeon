// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster.h"

#include "Kismet/GameplayStatics.h"
#include "RandomDungeon_CPP/ProgramDungeon/CombatHelper.h"


// Sets default values
AMonster::AMonster()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMonster::BeginPlay()
{
	Super::BeginPlay();
	CombatHelper = Cast<ACombatHelper>(UGameplayStatics::GetActorOfClass(GetWorld(),ACombatHelper::StaticClass()));
}

// Called every frame
void AMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMonster::MonsterDie()
{
	CombatHelper->MonsterDied(MasterRoom,this);
	this->Destroy();
}

