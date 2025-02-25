// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Filling.h"
#include "Monster.generated.h"

class AMasterRoom;
class ACombatHelper;

UCLASS()
class RANDOMDUNGEON_CPP_API AMonster : public AFilling
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMonster();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void MonsterDie();

	UPROPERTY()
	AMasterRoom* MasterRoom;

	UPROPERTY()
	ACombatHelper* CombatHelper;
};
