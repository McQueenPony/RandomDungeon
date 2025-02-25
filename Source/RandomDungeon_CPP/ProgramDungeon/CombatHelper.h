// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MasterRoom.h"
#include "GameFramework/Actor.h"
#include "CombatHelper.generated.h"

USTRUCT(BlueprintType)
struct FMonsterInfo
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite)
	TArray<AMonster*> Monsters;
	
	UPROPERTY(BlueprintReadWrite)
	int MonsterCount;
};




UCLASS()
class RANDOMDUNGEON_CPP_API ACombatHelper : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACombatHelper();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly)
	TMap<AMasterRoom*,FMonsterInfo> MonsterList= {}; //房间里的敌怪的列表

	
	DECLARE_MULTICAST_DELEGATE_OneParam(FMonsterAllDied,AMasterRoom*);//声明代理：本房间的怪打完了
	FMonsterAllDied OnMonsterAllDied;
	
	UFUNCTION(BlueprintCallable)
	void MonsterDied(AMasterRoom* Room,AMonster* Monster);//有怪物死亡了

	
	
};
