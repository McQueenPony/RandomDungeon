// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Door.h"
#include "IRoomInterface.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "RandomDungeon_CPP/Fillings/Monster.h"
#include "MasterRoom.generated.h"

UCLASS(Blueprintable)
class RANDOMDUNGEON_CPP_API AMasterRoom : public AActor , public IRoomInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMasterRoom();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void SetComponentRelationship() ;

	UFUNCTION()
	void InitializeRoomComponents() ; 

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	USceneComponent* Root;//根组件

	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly)
	USceneComponent* GeometryFolder; //静态网格体组件的文件夹

	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly)
	UArrowComponent* Arrow;//主箭头，指示房间的正方向

	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly)
	USceneComponent* OverlapFolder;//下放盒型碰撞箱

	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly)
	USceneComponent* ExitsFolder;//下放房间的出口

	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly)
	USceneComponent* Floor;//应该在地板上生成的生成物

	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly)
	USceneComponent* Air;//应该在空中生成的生成物

	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly)
	UBoxComponent* BoxCollision;//房间的盒型碰撞

	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly)
	int ChestNumber;//金币数量

	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly)
	USceneComponent* AirSpawnFolder;//空中的生成物

	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly)
	USceneComponent* FloorSpawnFolder;//地面上的生成物

	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly)
	USceneComponent* MonsterSpawnRoot;//怪物生成点

	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly)
	USceneComponent* ChestSpawnRoot;//宝箱生成点

	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly)
	int DispersionDegree = INT_MAX;//这个房间的分散程度;

	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly)
	TArray<ADoor*> DoorExistList = {}; //放置房间的门的列表

	UFUNCTION(BlueprintCallable)
	void OpenTheDoor();//打开所有房门

	UFUNCTION()
	virtual AMasterRoom* GetRoom() override;//获取这个房间

	
	UFUNCTION(BlueprintNativeEvent)
	void LightTheRoad();//点亮必经之路
};
