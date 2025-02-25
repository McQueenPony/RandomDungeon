// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatHelper.h"
#include "MasterRoom.h"
#include "GameFramework/Actor.h"
#include "RandomDungeon_CPP/Fillings/Filling.h"
#include "RandomDungeon_CPP/Fillings/FRoomFillingDB.h"
#include "RandomDungeon_CPP/Structs/FRoomLinkStruct.h"
#include "Templates/Tuple.h"
#include "DungeonGenerator.generated.h"


UENUM()
enum EFillingClass
{
	Monster,
	Chest
};

UCLASS()
class RANDOMDUNGEON_CPP_API ADungeonGenerator : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADungeonGenerator();
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	
	//*************************************函数**********************************//
protected:
	/*初始化并开始放置房间*/                                                         
	UFUNCTION(BlueprintCallable)
	void Init();
	
	UFUNCTION()
	void SpawnFirstRoom();//放置第一个房间

	UFUNCTION()
	void SpawnNextRoom();//放置下一个房间

	UFUNCTION()
	void SpawnChallengeRoom();//下一个放置挑战房

	UFUNCTION()
	void CheckForOverlaps();//检查是否有重叠的房间

	UFUNCTION()
	void AddOverlapRoomToList();//将重叠的房间添加到列表中

	UFUNCTION()
	void AddRoomToList(AMasterRoom* Room, const USceneComponent* ExistPoint);//将房间节点添加到邻接表中

	UFUNCTION()
	void RoomBFS(AMasterRoom* Begin,AMasterRoom* End);//广度搜索房间列表

	UFUNCTION()
	void BFSFindPath(AMasterRoom* Begin, const AMasterRoom* End);//寻找路径

	UFUNCTION()
	void ReconstructPath(AMasterRoom* End);//回溯并构建路径
	
	UFUNCTION()
	void SetRoomSeed();//设置房间种子

	UFUNCTION()
	void MakeRoomArray();//初始化房间种类列表

	UFUNCTION()
	void StartMaxSpawnTimer();//开启生成地图最大的时间，超过时间将重启地图生成程序
	
	UFUNCTION()
	static UDataTable* LoadDataTable(const FString& Path); //加载数据表

	UFUNCTION()
	void FillUpRooms();//将填充物塞入房间

	UFUNCTION()
	void SpawnNextSpecialRoom();//放置下一个特殊房间

	UFUNCTION()
	void CheckForCompleted();//检查是否是生成完成

	UFUNCTION()
	void ReduceRoom();//增加现有房间数目

	UFUNCTION()
	void ConstructCompleted();//基础房间构建完毕

	UFUNCTION()
	void DrawLinesToBossRoom();//在房间的必经之路上做上标识
	

	UFUNCTION()
	TMap<AMasterRoom*, int> CalculateDispersionDegree();//计算每个房间离最短路径的分散度（即分隔多少个房间）
	
	UFUNCTION()
	static float CalculateRoomDispersionVariance(TMap<AMasterRoom*,int> Map);//算出房间的总分散方差


	UFUNCTION()
	void SpawnChests(float Variance, TArray<TSubclassOf<AFilling>> ChestArray);//开始生成宝箱

	UFUNCTION()
	void SpawnMonsters(float Variance, TArray<TSubclassOf<AFilling>> MonsterArray , TSubclassOf<AFilling> BossClass);//开始生成怪物

	UFUNCTION()
	void SpawnDoors();//开始生成房门
	
	UFUNCTION()
	void CloseHoles();//关闭缺口

	//UFUNCTION()
	static TTuple<TArray<TSubclassOf<AFilling>> , TArray<TSubclassOf<AFilling>>> MakeRoomFillingArray(FRoomFillingDB Row);

	UFUNCTION()
	void SetRoomMaxDispersionDegree();

	UFUNCTION()
	void SpawnTrapsInChallengeRoom();
	public:

protected:
	
	/*创造出来的房间的列表,包含了房间真正的邻接关系*/
	UPROPERTY()
	TMap<AMasterRoom*,FRoomLinkStruct> ActualRoomList = {};

	UPROPERTY()
	TArray<TSubclassOf<AActor>> RoomList = {};//房间的类列表

	UPROPERTY()
	TArray<TSubclassOf<AActor>> ChallengeRoomList = {};//挑战房间的类列表

	UPROPERTY()
	float ChallengeRoomAppearingExpectation = 0.30f;//挑战房的出现期望

	UPROPERTY()
	int ChallengeRoomNum = 0;//挑战房的数量

	UPROPERTY()
	TArray<USceneComponent*> ExistList = {};//房间出口的列表

	UPROPERTY()
	int CurRoomNumber = 0;//当前的房间的数量

	UPROPERTY()
	bool IsSpawning = false;//是否正在生成房间

	UPROPERTY()
	AMasterRoom* FirstRoom = nullptr;//第一个生成的房间

	UPROPERTY()
	AMasterRoom* BossRoom = nullptr;//BOSS房

	UPROPERTY()
	TArray<AMasterRoom*> Path;//从初始房间到Boss房的路径

	UPROPERTY()
	TMap<AMasterRoom*,int> RoomDispersionDegree;//房间的离散度列表
	
	UPROPERTY()
	int InitUseTime = 0;//初始化的时间

	UPROPERTY()
	int MaxSpawnTime = 10000000;//初始化最大的时间

	UPROPERTY()
	TArray<AActor*> OverlappingActors = {};//重叠的actor的数组

	UPROPERTY()
	USceneComponent* SelectedExist = nullptr;//当前选中的出口

	UPROPERTY()
	int CurSpecialRoomIndex = 0;//当前的特殊房间的序号

	UPROPERTY()
	TMap<AMasterRoom*, AMasterRoom*> Predecessors;// 创建一个映射表，用于记录每个房间的前驱节点

	UPROPERTY()
	ACombatHelper* CombatHelper = nullptr;//战斗管理器

public:
	UPROPERTY(EditAnywhere)
	int MaxRoomNumber = 0;//最大的房间数量

	UPROPERTY(EditAnywhere)
	int Seed = -1;//设置的种子

	
protected:
	UPROPERTY()
	FString RoomName = "Default";//房间的名称

	UPROPERTY()
	TArray<TSubclassOf<AActor>> SpecialRoomList = {};//特殊的房间的列表

	UPROPERTY()
	AMasterRoom* LatestRoom = nullptr;//当前的房间指针

	UPROPERTY()
	TArray<int> SpecialRoomAppearingExpectation = {};//特殊房间将在多少个房间出现后出现的期望

	UPROPERTY()
	UDataTable* RoomDataTable = nullptr;//房间的数据表格

	UPROPERTY()
	FName RoomsSettingName = "Default";//房间的配置名称，同时作为表格的行结构

	UPROPERTY()
	FRandomStream SeedStream = NULL;//随机流

	UPROPERTY()
	FTimerHandle TimerHandle;//房间生成的时间柄

	UPROPERTY()
	FTimerHandle SpawnTimerHandle;//放置房间的时间柄

	UPROPERTY()
	TArray<USceneComponent*> DoorSpawnPoints = {};//房门生成点
	
	UPROPERTY()
	UClass* NormalRoom = nullptr;//普通房间

	UPROPERTY()
	UClass* EndWall = nullptr;//尽头墙

	UPROPERTY()
	UClass* RoomDoor = nullptr;//房门

	UPROPERTY()
	float ChestSpawnExpectation = 0.5f;//箱子释放平均期望

	UPROPERTY()
	float MonsterSpawnExpectation = 0.6f;//怪物生成平均期望

	UPROPERTY()
	UDataTable* RoomFillingDataTable = nullptr;//房间生成物的表格

	UPROPERTY()
	int MaxDispersion = 0;
	
	
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
