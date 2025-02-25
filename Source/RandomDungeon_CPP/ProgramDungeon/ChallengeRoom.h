// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MasterRoom.h"
#include "TrapBase.h"
#include "ChallengeRoom.generated.h"

USTRUCT(BlueprintType)
struct FTrapMap
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<USceneComponent*> Components;
};



UCLASS()
class RANDOMDUNGEON_CPP_API AChallengeRoom : public AMasterRoom
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AChallengeRoom();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly)
	USceneComponent* TrapSpawnRoot; // 陷阱的生成点

	
	
	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly)
	TMap<TSubclassOf<ATrapBase>,FTrapMap> Traps;//实际的陷阱的数组

	UFUNCTION(BlueprintNativeEvent)
	void MakeTrapMap();
};
