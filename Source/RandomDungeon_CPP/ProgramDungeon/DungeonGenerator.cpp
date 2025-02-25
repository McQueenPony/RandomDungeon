// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonGenerator.h"

#include "Kismet/GameplayStatics.h"
#include "RandomDungeon_CPP/Fillings/FRoomFillingDB.h"
#include "RandomDungeon_CPP/Structs/FDungeonDB.h"



ADungeonGenerator::ADungeonGenerator()
{
	PrimaryActorTick.bCanEverTick = false;

	RoomDataTable = LoadDataTable(TEXT("/Game/ProgramGeneratedDungeon/DT_Dungeon.DT_Dungeon"));
	if (!RoomDataTable) UE_LOG(LogTemp, Error, TEXT("RoomDataTable is null"));
	//初始化表格
	RoomFillingDataTable = LoadDataTable(TEXT("/Game/ProgramGeneratedDungeon/Fillings/DT_RoomFilling.DT_RoomFilling"));
	NormalRoom = ConstructorHelpers::FClassFinder<AActor>(TEXT("/Game/ProgramGeneratedDungeon/ActuallyRooms/NormalRooms/BP_Room_Normal")).Class;
	EndWall = ConstructorHelpers::FClassFinder<AActor>(TEXT("/Game/ProgramGeneratedDungeon/ActuallyRooms/BP_EndWall")).Class;
	RoomDoor = ConstructorHelpers::FClassFinder<AActor>(TEXT("/Game/ProgramGeneratedDungeon/Fillings/BP_Door")).Class;
}

void ADungeonGenerator::BeginPlay()
{
	Super::BeginPlay();
	const auto Transform = GetActorTransform();
	CombatHelper = Cast<ACombatHelper>(GetWorld()->SpawnActor(ACombatHelper::StaticClass(), &Transform));
}

void ADungeonGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}




void ADungeonGenerator::Init()
{
	MakeRoomArray(); //制造房间列表
	SetRoomSeed();
	CurRoomNumber = MaxRoomNumber ;
	SpawnFirstRoom();
	SpawnNextRoom();
	StartMaxSpawnTimer();
}

void ADungeonGenerator::SpawnFirstRoom()
{
	// 检查是否成功找到房间类。
	if (UClass* RoomNormalClass = NormalRoom)
	{
		// 使用找到的房间类在世界中生成一个房间实例。
		// 房间的生成位置和旋转基于当前生成器的根组件（GetRootComponent()->GetComponentTransform()）。
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // 设置为true以忽略碰撞
		AMasterRoom* ThisRoom = Cast<AMasterRoom>(GetWorld()->SpawnActor<AActor>(RoomNormalClass, GetRootComponent()->GetComponentTransform(),SpawnParams));

		FirstRoom = ThisRoom;//赋值给第一个房间的变量

		ActualRoomList.Add(ThisRoom);
		FRoomLinkStruct LinkStruct;
		LinkStruct.LinkedRooms = {};
		ActualRoomList[ThisRoom] = LinkStruct;
		
		// 获取房间的出口组件列表。
		// ExitsFolder 是房间中用于标记出口的容器组件，这里获取其所有子组件。
		TArray<USceneComponent*> ExitsChildren;
		ThisRoom->ExitsFolder->GetChildrenComponents(false, ExitsChildren);
        
		// 将这些出口组件添加到全局的出口列表（ExistList）中，
		// 以便后续生成其他房间时可以连接到这些出口。
		ExistList.Append(ExitsChildren);
		
	}
}

void ADungeonGenerator::SpawnNextRoom()
{
	// 如果正在生成房间，则直接返回，避免重复调用
	if (IsSpawning) return;
	IsSpawning = true;

	// 检查是否存在房间列表和房间模板列表
	if (!ExistList.IsEmpty() && !RoomList.IsEmpty())
	{
		// 从已存在的房间列表中随机选择一个房间作为连接点
		SelectedExist = ExistList[SeedStream.RandRange(0, ExistList.Num() - 1)];

		// 从房间模板列表中随机选择一个房间模板
		TSubclassOf<AActor> SelectedRoomClass = RoomList[SeedStream.RandRange(0, RoomList.Num() - 1)];

		// 获取选中房间的位置和旋转
		FVector Location = SelectedExist->GetComponentLocation();
		FRotator Rotation = SelectedExist->GetComponentRotation();

		// 设置生成参数
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // 设置为true以忽略碰撞，确保房间可以生成

		// 在指定位置和旋转生成房间
		LatestRoom = Cast<AMasterRoom>(GetWorld()->SpawnActor(SelectedRoomClass, &Location, &Rotation, SpawnParams));

		// 设置一个定时器，用于检查新生成的房间是否与其他房间重叠
		GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ADungeonGenerator::CheckForOverlaps, 0.2f, false, 0.0f);
	}
}

void ADungeonGenerator::SpawnChallengeRoom()
{
	// 如果正在生成房间，则直接返回，避免重复调用
	if (IsSpawning) return;
	IsSpawning = true;

	// 检查是否存在房间列表和房间模板列表
	if (!ExistList.IsEmpty() && !ChallengeRoomList.IsEmpty())
	{
		// 从已存在的房间列表中随机选择一个房间作为连接点
		SelectedExist = ExistList[SeedStream.RandRange(0, ExistList.Num() - 1)];

		// 从房间模板列表中随机选择一个房间模板
		TSubclassOf<AActor> SelectedRoomClass = ChallengeRoomList[SeedStream.RandRange(0, ChallengeRoomList.Num() - 1)];

		// 获取选中房间的位置和旋转
		FVector Location = SelectedExist->GetComponentLocation();
		FRotator Rotation = SelectedExist->GetComponentRotation();

		// 设置生成参数
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // 设置为true以忽略碰撞，确保房间可以生成

		// 在指定位置和旋转生成房间
		LatestRoom = Cast<AMasterRoom>(GetWorld()->SpawnActor(SelectedRoomClass, &Location, &Rotation, SpawnParams));

		// 设置一个定时器，用于检查新生成的房间是否与其他房间重叠
		GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ADungeonGenerator::CheckForOverlaps, 0.2f, false, 0.0f);
	}
}

void ADungeonGenerator::CheckForOverlaps()
{
    // 检测最新生成的房间是否与其他房间重叠
    AddOverlapRoomToList(); // 调用函数将重叠的房间添加到OverlappingActors列表
	FDungeonDB* Row = RoomDataTable->FindRow<FDungeonDB>(RoomsSettingName,TEXT("Finding Row"),true);
	if (!Row) Row = RoomDataTable->FindRow<FDungeonDB>("Default",TEXT("Finding Row"),true);
    // 如果存在重叠的房间
    if (!OverlappingActors.IsEmpty())
    {
    	
        IsSpawning = false; // 设置生成标志为false，表示当前没有生成房间
        // 检测最新生成的房间是否是特殊房间
        if (LatestRoom->GetClass()->IsChildOf(ASpecialRoom::StaticClass()))
        {
            // 如果是特殊房间，清空重叠列表，销毁当前房间，并尝试生成下一个特殊房间
            OverlappingActors.Empty();
            LatestRoom->Destroy();
            SpawnNextSpecialRoom();
        }
        else
        {
            // 如果不是特殊房间，清空重叠列表，销毁当前房间，并尝试生成下一个普通房间   可能为挑战房
            OverlappingActors.Empty();
        	TSubclassOf<AMasterRoom> SelectedRoomClass = LatestRoom->GetClass();
            LatestRoom->Destroy();
        	
        	if (SelectedRoomClass->IsChildOf(AChallengeRoom::StaticClass()) ||(SeedStream.FRandRange(0.0f,1.0f) >= ChallengeRoomAppearingExpectation && Row->ChallengeRoomMaxNum >= ChallengeRoomNum))
        	{
	            SpawnChallengeRoom();
        	}
        	else
        	{
	            SpawnNextRoom();
        	}
            
        }
    }
    else
    {
        // 如果没有重叠的房间
        OverlappingActors.Empty(); // 清空重叠列表
        ReduceRoom(); //更改房间现有数目

    	AddRoomToList(LatestRoom, SelectedExist);//将房间加入邻接表
    	
        // 移除当前选中的房间组件
        ExistList.Remove(SelectedExist);

    	//加入房门生成点中
		DoorSpawnPoints.Add(SelectedExist);
    	
        // 获取最新生成房间的子组件（用于生成新的房间连接点）
        TArray<USceneComponent*> SpawnChildren;
        LatestRoom->FloorSpawnFolder->GetChildrenComponents(false, SpawnChildren);
    	TArray<USceneComponent*> SpawnChildren1;
    	LatestRoom->FloorSpawnFolder->GetChildrenComponents(true, SpawnChildren1);

    	SpawnChildren.Append(SpawnChildren1);
        for (const USceneComponent* EachComponent : SpawnChildren)
        {
            TArray<USceneComponent*> SpawnChildren2;
            EachComponent->GetChildrenComponents(false, SpawnChildren2);
        }

        // 获取最新生成房间的出口组件（用于后续房间的连接）
        TArray<USceneComponent*> ExistChildren;
        LatestRoom->ExitsFolder->GetChildrenComponents(false, ExistChildren);
        ExistList.Append(ExistChildren);

        // 如果最新生成的房间是特殊房间
        if (LatestRoom->GetClass()->IsChildOf(ASpecialRoom::StaticClass()))
        {
            // 从特殊房间列表中移除当前特殊房间
            SpecialRoomList.RemoveAt(CurSpecialRoomIndex);
            SpecialRoomAppearingExpectation.RemoveAt(CurSpecialRoomIndex);
        }

        // 检查是否还有剩余的房间需要生成
        if (CurRoomNumber > 0)
        {
            IsSpawning = false; // 设置生成标志为false
            bool IsBreak = false; // 标志变量，用于检查是否找到特殊房间生成点
            for (int i = 0; i < SpecialRoomAppearingExpectation.Num(); i++)
            {
                // 检查当前房间数量是否达到特殊房间的生成条件
                if (CurRoomNumber == SpecialRoomAppearingExpectation[i])
                {
                    CurSpecialRoomIndex = i; // 更新当前特殊房间索引
                    IsBreak = true; // 设置标志变量为true
                }
            }
            if (IsBreak)
            {
                // 如果找到特殊房间生成点，生成下一个特殊房间
                SpawnNextSpecialRoom();
            }
            else
            {
                // 如果没有找到特殊房间生成点，继续生成普通房间
            	if (!LatestRoom->GetClass()->IsChildOf(AChallengeRoom::StaticClass()) && SeedStream.FRandRange(0.0f,1.0f) >= ChallengeRoomAppearingExpectation && Row->ChallengeRoomMaxNum >= ChallengeRoomNum)
            	{
            		SpawnChallengeRoom();
            	}
            	else
            	{
            		SpawnNextRoom();
            	}

                if (CurRoomNumber / MaxRoomNumber <= 0.7 && ChallengeRoomNum / Row->ChallengeRoomMaxNum <= 4 * CurRoomNumber * (MaxRoomNumber - CurRoomNumber) / pow(MaxRoomNumber,2))
                {
	                ChallengeRoomAppearingExpectation += 0.05f;
                }
            }
        }
        else
        {
            // 如果房间数量已经生成完毕，调用构造完成函数
            ConstructCompleted();
        }
    }
}

void ADungeonGenerator::AddOverlapRoomToList()
{
	// 定义一个数组，用于存储与最新生成的房间（LatestRoom）的OverlapFolder组件重叠的子组件
	TArray<USceneComponent*> OverlapChildren;
	// 获取LatestRoom的OverlapFolder组件的所有子组件
	LatestRoom->OverlapFolder->GetChildrenComponents(false, OverlapChildren);

	// 遍历所有子组件
	for (USceneComponent* EachComponent : OverlapChildren)
	{
		// 将子组件强制转换为UBoxComponent类型
		UBoxComponent* Box = Cast<UBoxComponent>(EachComponent);
		Box->UpdateComponentToWorld(); // 更新组件到世界空间
		// 定义一个数组，用于存储与当前Box组件重叠的Actor
		
		TArray<AActor*> BoxOverlapActors;
		Box->GetOverlappingActors(BoxOverlapActors,AMasterRoom::StaticClass());
		BoxOverlapActors.Remove(LatestRoom);
		
		OverlappingActors.Append(BoxOverlapActors);
	}
}

void ADungeonGenerator::AddRoomToList(AMasterRoom* Room, const USceneComponent* ExistPoint)
{
	if (!Room)
	{
		return; // 如果房间为空，直接返回
	}

	if (!ExistPoint)
	{
		// 如果连接点为空，不进行任何操作，直接返回
		return;
	}

	AMasterRoom* ParentRoom = Cast<AMasterRoom>(ExistPoint->GetOwner());
	if (!ParentRoom)
	{
		// 如果连接点的父房间为空，不进行任何操作，直接返回
		return;
	}

	// 将房间添加到父房间的连接列表中
	ActualRoomList[ParentRoom].LinkedRooms.Add(Room);

	// 如果房间尚未在 ActualRoomList 中，初始化其连接列表
	if (!ActualRoomList.Contains(Room))
	{
		FRoomLinkStruct RoomLinkStruct;
		ActualRoomList.Add(Room);
		ActualRoomList[Room] = RoomLinkStruct;
	}

	// 将父房间添加到当前房间的连接列表中
	ActualRoomList[Room].LinkedRooms.Add(ParentRoom);
}

void ADungeonGenerator::RoomBFS(AMasterRoom* Begin, AMasterRoom* End)
{
	// 检查起始房间是否为空，如果为空则直接返回，避免无效操作
	if (!Begin || !End) return;
	
	// 调用 BFSFindPath 函数，从起始房间到目标房间进行路径搜索
	// 注意：这里需要将 Predecessors 作为参数传递给 BFSFindPath 函数
	BFSFindPath(Begin, End);

	// 检查是否找到路径，即目标房间是否在 Predecessors 映射表中
	if (Predecessors.Contains(End))
	{
		// 如果找到路径，调用 ReconstructPath 函数重建路径
		ReconstructPath(End);
	}
	else
	{
// 		// 如果没有找到路径，打印警告信息
// 		UE_LOG(LogTemp, Warning, TEXT("No path found between rooms."));
// 		GEngine->AddOnScreenDebugMessage(
// 	-1,          // 消息的唯一标识，-1表示添加新消息，非-1则更新现有消息
// 	20.0f,// 消息显示的时间（秒）
// 	FColor::Red ,// 消息显示的颜色
// 	"No path found between rooms." // 要显示的消息内容
// );
	}
}

void ADungeonGenerator::BFSFindPath(AMasterRoom* Begin, const AMasterRoom* End)
{
	if (!Begin || !End) return;

	Predecessors.Empty(); // 清空前驱列表
	TQueue<AMasterRoom*> Queue;
	TSet<AMasterRoom*> Visited;

	Queue.Enqueue(Begin);
	Visited.Add(Begin);
	Predecessors.Add(Begin, nullptr);

	while (!Queue.IsEmpty())
	{
		AMasterRoom* CurrentRoom = nullptr;
		Queue.Dequeue(CurrentRoom);

		// 提前终止条件：找到目标房间
		if (CurrentRoom == End)
			break;

		// 获取当前房间的邻接房间
		if (ActualRoomList.Contains(CurrentRoom))
		{
			TArray<AMasterRoom*>& ConnectedRooms = ActualRoomList[CurrentRoom].LinkedRooms;
			for (AMasterRoom* Neighbor : ConnectedRooms)
			{
				if (!Visited.Contains(Neighbor))
				{
					Visited.Add(Neighbor);
					Predecessors.Add(Neighbor, CurrentRoom);
					Queue.Enqueue(Neighbor);

					// 若邻居是目标，提前结束
					if (Neighbor == End)
						return;
				}
			}
		}
	}
}

void ADungeonGenerator::ReconstructPath(AMasterRoom* End)
{
	Path.Empty(); // 清空路径列表

	// 从目标房间开始，沿着前驱节点回溯到起始房间
	AMasterRoom* CurrentRoom = End;
	while (CurrentRoom)
	{
		Path.Add(CurrentRoom); // 将当前房间添加到路径列表
		CurrentRoom = Predecessors[CurrentRoom]; // 获取当前房间的前驱节点
	}

	// 翻转路径列表，因为回溯是从目标房间到起始房间，需要翻转以得到从起始房间到目标房间的路径
	int PathLength = Path.Num();
	for (int i = 0; i < PathLength / 2; ++i)
	{
		AMasterRoom* Temp = Path[i]; 
		Path[i] = Path[PathLength - 1 - i]; // 交换路径的前半部分和后半部分
		Path[PathLength - 1 - i] = Temp;
	}
}

void ADungeonGenerator::SetRoomSeed()
{
	// 如果种子值为-1，则随机生成一个种子值
	if (Seed == -1)
	{
		// 使用FMath::RandRange生成一个范围在1到1000000000之间的随机数作为种子
		SeedStream.Initialize(FMath::RandRange(1, 1000000000));
	}
	else
	{
		// 如果种子值不为-1，则使用用户指定的种子值初始化SeedStream
		SeedStream.Initialize(Seed);
	}
}

void ADungeonGenerator::MakeRoomArray()
{
	RoomList.Empty();//清空RoomList
	
	FDungeonDB* Row = RoomDataTable->FindRow<FDungeonDB>(RoomsSettingName,TEXT("Finding Row"),true);
	if (!Row) Row = RoomDataTable->FindRow<FDungeonDB>("Default",TEXT("Finding Row"),true);
	//获取行结构
	for(TTuple<TSubclassOf<ANormalRoom>,int> EachRow : Row->NormalPercent)
	{
		for(int i = 0;i < EachRow.Value; ++i)
		{
			if (EachRow.Key)
			{
				RoomList.Add(StaticCast<TSubclassOf<AActor>>(EachRow.Key));
			}
			
		}
	}//填充普通房间的数组
	for (FSpecialRoomStruct EachRow : Row->SpecialPercent)
	{
		SpecialRoomList.Add(StaticCast<TSubclassOf<AActor>>(EachRow.Class));
		SpecialRoomAppearingExpectation.Add((100-EachRow.Percent)*MaxRoomNumber*0.01);
	}//填充特殊房间的数组并且完成期望的计算

	for (auto EachRow : Row->ChallengeRoomPercent)
	{
		for (int i = 0; i < EachRow.Value; ++i)
		{
			if (EachRow.Key)
			{
				ChallengeRoomList.Add(StaticCast<TSubclassOf<AActor>>(EachRow.Key));
			}
		}
	}//填充挑战房的数组
	
}

void ADungeonGenerator::StartMaxSpawnTimer()
{
	GetWorldTimerManager().SetTimer(TimerHandle,this,&ADungeonGenerator::CheckForCompleted,1.0f,true,0.0f);	
}

UDataTable* ADungeonGenerator::LoadDataTable(const FString& Path)
{
	// 使用FSoftObjectPath来加载资源
	FSoftObjectPath DataTablePath(Path);
	UDataTable* DataTable = Cast<UDataTable>(DataTablePath.TryLoad());
	if (DataTable)
	{
		UE_LOG(LogTemp, Log, TEXT("DataTable loaded successfully!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load DataTable from path: %s"), *Path);
	}
	return DataTable;
}

void ADungeonGenerator::SpawnNextSpecialRoom()
{
	// 如果正在生成房间，则直接返回，避免重复调用
	if (IsSpawning) return;
	IsSpawning = true;

	// 检查是否存在房间列表和房间模板列表
	if (!ExistList.IsEmpty() && !RoomList.IsEmpty())
	{
		// 从已存在的房间列表中随机选择一个房间作为连接点
		SelectedExist = ExistList[SeedStream.RandRange(0, ExistList.Num() - 1)];

		// 从房间模板列表中随机选择一个房间模板
		TSubclassOf<AActor> SelectedRoomClass = SpecialRoomList[CurSpecialRoomIndex];

		// 获取选中房间的位置和旋转
		FVector Location = SelectedExist->GetComponentLocation();
		FRotator Rotation = SelectedExist->GetComponentRotation();

		// 设置生成参数
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // 设置为true以忽略碰撞，确保房间可以生成

		// 在指定位置和旋转生成房间
		LatestRoom = Cast<AMasterRoom>(GetWorld()->SpawnActor(SelectedRoomClass, &Location, &Rotation, SpawnParams));
		
		if (Cast<ASpecialRoom>(LatestRoom)->GetIsBOSSRoom())
		{
			BossRoom = LatestRoom;//寻找这是不是BOSS房，如果是的话就设置变量   一张地图目前限定为只有一个BOSS房
		}

		// 设置一个定时器，用于检查新生成的房间是否与其他房间重叠
		GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ADungeonGenerator::CheckForOverlaps, 0.2f, false, 0.0f);
	
	}
}

void ADungeonGenerator::CheckForCompleted()
{
	// 每次调用该函数时，初始化时间加1
	if (++InitUseTime >= MaxSpawnTime)
	{
		// 如果初始化时间达到最大生成限制
		// 重新加载当前关卡，用于重置关卡
		UGameplayStatics::OpenLevel(GetWorld(), FName(GetLevel()->GetName()));
	}
}

void ADungeonGenerator::ReduceRoom()
{
	if (!LatestRoom->GetClass()->IsChildOf(ASpecialRoom::StaticClass()))
	{
		if (LatestRoom->GetClass()->IsChildOf(AChallengeRoom::StaticClass()))
		{
			ChallengeRoomNum++;
		}
		CurRoomNumber--;
	}

}

void ADungeonGenerator::ConstructCompleted()
{
	UE_LOG(LogTemp,Log,TEXT("ConstructCompleted"));
	GEngine->AddOnScreenDebugMessage(
	-1,          // 消息的唯一标识，-1表示添加新消息，非-1则更新现有消息
	20.0f,// 消息显示的时间（秒）
	FColor::Red ,// 消息显示的颜色
	"ConstructCompleted" // 要显示的消息内容
);
	RoomBFS(FirstRoom,BossRoom);//遍历房间
	CloseHoles();//关闭缺口
	DrawLinesToBossRoom();//画出到boss房的路
	TimerHandle.Invalidate();//关闭房间生成计时器
	FillUpRooms();//填充房间
}

void ADungeonGenerator::DrawLinesToBossRoom()
{
	for (int i = 0 ; i < Path.Num() - 1 ; i++)
	{
		Path[i]->LightTheRoad();
	}
}

void ADungeonGenerator::FillUpRooms()
{
	SpawnDoors();
	
	// 从房间填充数据表中查找指定的房间设置行
	// RoomsSettingName 是要查找的行名，如果找不到则使用默认行 "Default"
	FRoomFillingDB* Row = RoomFillingDataTable->FindRow<FRoomFillingDB>(RoomsSettingName, TEXT("Finding Row"), true);
	if (!Row) 
		Row = RoomFillingDataTable->FindRow<FRoomFillingDB>("Default", TEXT("Finding Row"), true);

	// 根据找到的房间设置行生成填充物的数组
	// 包括宝藏和怪物的子类数组
	const TTuple<TArray<TSubclassOf<AFilling>>, TArray<TSubclassOf<AFilling>>> Arrays = MakeRoomFillingArray(*Row);

	// 计算房间的分散度
	RoomDispersionDegree = CalculateDispersionDegree();

	// 设置房间的最大分散度
	SetRoomMaxDispersionDegree();

	// 根据房间的分散度方差生成宝箱
	// 使用 Arrays.Key 中的宝藏子类数组
	SpawnChests(CalculateRoomDispersionVariance(RoomDispersionDegree), Arrays.Key);

	//放置陷阱
	SpawnTrapsInChallengeRoom();

	// 根据房间的分散度方差生成怪物
	// 使用 Arrays.Value 中的怪物子类数组和房间设置行中的 Boss 数据
	SpawnMonsters(CalculateRoomDispersionVariance(RoomDispersionDegree), Arrays.Value, Row->Boss);

	// 在屏幕上显示调试信息，表示房间填充完成
	GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, "Fill Up Completed");
}

TMap<AMasterRoom*, int> ADungeonGenerator::CalculateDispersionDegree()
{
	// 定义一个队列用于广度优先搜索（BFS）
	TQueue<AMasterRoom*> Queue;

	// 定义一个集合用于记录已访问过的房间
	TSet<AMasterRoom*> Visit;

	// 定义一个映射表用于存储每个房间的分散度
	TMap<AMasterRoom*, int> DispersionDegree;

	// 遍历路径上的所有房间（Path 是一个包含 AMasterRoom* 的集合）
	for (AMasterRoom* PathRoom : Path)
	{
		// 初始化路径房间的分散度为 0
		PathRoom->DispersionDegree = 0;

		// 将路径房间加入队列
		Queue.Enqueue(PathRoom);

		// 将路径房间标记为已访问
		Visit.Add(PathRoom);

		// 在屏幕上显示调试信息，表示当前房间的分散度为 0
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, "0");

		// 将路径房间及其分散度加入映射表
		DispersionDegree.Add(PathRoom);
		DispersionDegree[PathRoom] = 0;
	}

	// 开始广度优先搜索（BFS）
	while (!Queue.IsEmpty())
	{
		// 从队列中取出一个房间
		AMasterRoom* NowRoom = nullptr;
		Queue.Dequeue(NowRoom);

		// 将当前房间标记为已访问
		Visit.Add(NowRoom);

		// 遍历当前房间的所有连接房间（LinkedRooms 是一个包含 AMasterRoom* 的数组）
		for (auto Element : ActualRoomList[NowRoom].LinkedRooms)
		{
			// 如果连接房间尚未访问
			if (!Visit.Contains(Element))
			{
				// 设置连接房间的分散度为当前房间的分散度 + 1
				Element->DispersionDegree = NowRoom->DispersionDegree + 1;

				// 将连接房间加入队列
				Queue.Enqueue(Element);

				// 在屏幕上显示调试信息，显示连接房间的分散度
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::FromInt(Element->DispersionDegree));

				// 将连接房间及其分散度加入映射表
				DispersionDegree.Add(Element);
				DispersionDegree[Element] = Element->DispersionDegree;
			}
		}
	}

	// 返回包含所有房间及其分散度的映射表
	return DispersionDegree;
}

float ADungeonGenerator::CalculateRoomDispersionVariance(TMap<AMasterRoom*, int> Map) 
{
	// 初始化总和变量，用于计算所有房间分散度的总和
	float Sum = 0.0f;

	// 遍历映射表中的每个键值对（房间及其分散度）
	for (TTuple<AMasterRoom*, int> Pair : Map)
	{
		// 将当前房间的分散度累加到总和中
		Sum += Pair.Value;
	}

	// 计算房间分散度的平均值
	// Map.Num() 返回映射表中键值对的数量，即房间总数
	float Average = Sum / Map.Num();

	// 初始化方差变量
	float Variance = 0.0f;

	// 再次遍历映射表中的每个键值对
	for (TTuple<AMasterRoom*, int> Pair : Map)
	{
		// 计算每个房间分散度与平均值的差的平方，并累加到方差中
		// 最后除以房间总数，得到方差
		Variance += (Pair.Value - Average) * (Pair.Value - Average) / Map.Num();
	}

	// 返回计算得到的方差
	return Variance;
}

void ADungeonGenerator::SpawnChests(float Variance, TArray<TSubclassOf<AFilling>> ChestArray)
{
    // 定义平均生成期望值
    float Average = ChestSpawnExpectation;

    // 定义已访问房间集合，用于避免重复访问
    TSet<AMasterRoom*> Visit;

    // 定义队列，用于广度优先搜索（BFS）
    TQueue<AMasterRoom*> Queue;

    // 定义一个 Lambda 函数，用于计算每个房间的生成点概率
    auto CalculateRoomSpawnPointProbability = [&](const AMasterRoom* Element) -> bool
    {
        // 使用 Box-Muller 变换生成正态分布随机数
        const float U1 = SeedStream.FRandRange(0.01f, 1.0f); // 随机数 U1
        const float U2 = SeedStream.FRandRange(0.01f, 1.0f); // 随机数 U2
        const float Z = FMath::Clamp(sqrt(-2.0f * log(U1)) * cos(2.0f * PI * U2), -3.0f, 3.0f); // 标准正态分布随机数

        // 计算当前房间的生成概率
        const float Probability = Average * (1 - Element->DispersionDegree / 8 / MaxDispersion) +
                                  Variance * (1 + Element->DispersionDegree / MaxDispersion) * Z;

        // 计算概率的上下限
        const float Max = Average * (1 - Element->DispersionDegree / 8 / MaxDispersion) + 3 * Variance * (1 + Element->DispersionDegree / MaxDispersion);
        const float Min = Average * (1 - Element->DispersionDegree / 8 / MaxDispersion) - 3 * Variance * (1 + Element->DispersionDegree / MaxDispersion);

        // 生成一个随机数，判断是否满足生成条件
        return SeedStream.FRandRange(Min, Max) >= Probability;
    };

    // 遍历路径上的房间，初始化队列和访问集合
    for (auto Element : Path)
    {
        Visit.Add(Element); // 标记为已访问
        Queue.Enqueue(Element); // 加入队列

        // 获取当前房间的所有宝箱生成点
        TArray<USceneComponent*> SpawnPoints;
        Element->ChestSpawnRoot->GetChildrenComponents(false, SpawnPoints);

        // 遍历每个生成点
        for (const auto SpawnPoint : SpawnPoints)
        {
            // 根据概率判断是否生成宝箱
            if (CalculateRoomSpawnPointProbability(Element))
            {
                // 随机选择一个宝箱类
                TSubclassOf<AFilling> Class = ChestArray[SeedStream.RandRange(0, ChestArray.Num() - 1)];
                FActorSpawnParameters SpawnParams;
                SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // 忽略碰撞，强制生成
                GetWorld()->SpawnActor(Class, &SpawnPoint->GetComponentTransform(), SpawnParams); // 生成宝箱
            }
        }
    }

    // 使用 BFS 遍历所有房间
    while (!Queue.IsEmpty())
    {
        AMasterRoom* CurrentRoom = nullptr;
        Queue.Dequeue(CurrentRoom); // 取出当前房间

        // 获取当前房间的邻接房间
        TArray<AMasterRoom*>& ConnectedRooms = ActualRoomList[CurrentRoom].LinkedRooms;
        for (AMasterRoom* Neighbor : ConnectedRooms)
        {
            if (!Visit.Contains(Neighbor)) // 如果邻接房间未访问
            {
                Visit.Add(Neighbor); // 标记为已访问
                Queue.Enqueue(Neighbor); // 加入队列

                // 获取邻接房间的所有宝箱生成点
                TArray<USceneComponent*> SpawnPoints;
                Neighbor->ChestSpawnRoot->GetChildrenComponents(false, SpawnPoints);

                // 遍历每个生成点
                for (const auto SpawnPoint : SpawnPoints)
                {
                    // 根据概率判断是否生成宝箱
                    if (CalculateRoomSpawnPointProbability(Neighbor))
                    {
                        // 随机选择一个宝箱类
                        TSubclassOf<AFilling> Class = ChestArray[SeedStream.RandRange(0, ChestArray.Num() - 1)];
                        FActorSpawnParameters SpawnParams;
                        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // 忽略碰撞，强制生成
                        GetWorld()->SpawnActor(Class, &SpawnPoint->GetComponentTransform(), SpawnParams); // 生成宝箱
                    }
                }
            }
        }
    }
}

void ADungeonGenerator::SpawnMonsters(float Variance, TArray<TSubclassOf<AFilling>> MonsterArray, TSubclassOf<AFilling> BossClass)
{
    // 定义平均生成期望值
    float Average = MonsterSpawnExpectation;

    // 定义已访问房间集合，用于避免重复访问
    TSet<AMasterRoom*> Visit;

    // 定义队列，用于广度优先搜索（BFS）
    TQueue<AMasterRoom*> Queue;

    // 定义一个 Lambda 函数，用于计算每个房间的生成点概率
    auto CalculateRoomSpawnPointProbability = [&](const AMasterRoom* Element) -> bool
    {
        // 使用 Box-Muller 变换生成正态分布随机数
        const float U1 = SeedStream.FRandRange(0.01f, 1.0f); // 随机数 U1
        const float U2 = SeedStream.FRandRange(0.01f, 1.0f); // 随机数 U2
        const float Z = FMath::Clamp(sqrt(-2.0f * log(U1)) * cos(2.0f * PI * U2), -3.0f, 3.0f); // 标准正态分布随机数

        // 计算当前房间的生成概率
        const float Probability = Average * (1 - Element->DispersionDegree / 8 / MaxDispersion) +
                                  Variance * (1 + Element->DispersionDegree / MaxDispersion) * Z;

        // 计算概率的上下限
        const float Max = Average * (1 - Element->DispersionDegree / 8 / MaxDispersion) + 3 * Variance * (1 + Element->DispersionDegree / MaxDispersion);
        const float Min = Average * (1 - Element->DispersionDegree / 8 / MaxDispersion) - 3 * Variance * (1 + Element->DispersionDegree / MaxDispersion);

        // 生成一个随机数，判断是否满足生成条件
        return SeedStream.FRandRange(Min, Max) >= Probability;
    };

	/*
		正态分布奖励生成逻辑
		为了实现房间的离散程度越高即（距离越远），奖励数量越少的效果，可以通过调整正态分布的参数来实现。
		1. 调整均值 μ
		根据房间的距离 d 调整每个房间的均值 μd：
		μd=μ⋅(1−d/8D)
		其中：
		μ 是所有房间的平均奖励数量。
		d 是当前房间的距离。
		D 是最远房间的距离。
		2. 调整标准差 σ
		根据房间的距离 d 调整每个房间的标准差 σd：
		σd=σ⋅(1+d/D)
		其中：
		σ 是所有房间的平均标准差。
		d 是当前房间的距离。
		D 是最远房间的距离。
		3. 正态分布公式应用
		每个房间的奖励数量 Xd 可以通过以下公式计算：
		Xd=μd+σd*Z
		其中 Z 是标准正态分布的随机变量。
	*/
	
    // 遍历路径上的房间，初始化队列和访问集合
    for (auto Element : Path)
    {
        Visit.Add(Element); // 标记为已访问
        Queue.Enqueue(Element); // 加入队列

        // 如果当前房间是 Boss 房间，则跳过
        if (Element == BossRoom)
        {
            continue;
        }

        // 获取当前房间的所有怪物生成点
        TArray<USceneComponent*> SpawnPoints;
        Element->MonsterSpawnRoot->GetChildrenComponents(false, SpawnPoints);

        // 遍历每个生成点
        for (const auto SpawnPoint : SpawnPoints)
        {
            // 根据概率判断是否生成怪物
            if (Element->GetClass()->IsChildOf(AChallengeRoom::StaticClass()) || CalculateRoomSpawnPointProbability(Element))
            {
                // 随机选择一个怪物类
                TSubclassOf<AFilling> Class = MonsterArray[SeedStream.RandRange(0, MonsterArray.Num() - 1)];
                FActorSpawnParameters SpawnParams;
                SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // 忽略碰撞，强制生成

            	//处理战斗管理器逻辑
            	AMonster* Monster = Cast<AMonster>(GetWorld()->SpawnActor(Class, &SpawnPoint->GetComponentTransform(), SpawnParams));// 生成怪物
				Monster->MasterRoom = Element;
                if (CombatHelper->MonsterList.Find(Element) == nullptr)
                {
                	CombatHelper->MonsterList.Add(Element);
                }
            	CombatHelper->MonsterList[Element].Monsters.Add(Monster);
            	CombatHelper->MonsterList[Element].MonsterCount++;
            }
        }
    }

    // 使用 BFS 遍历所有房间
    while (!Queue.IsEmpty())
    {
        AMasterRoom* CurrentRoom = nullptr;
        Queue.Dequeue(CurrentRoom); // 取出当前房间

        // 获取当前房间的邻接房间
        TArray<AMasterRoom*>& ConnectedRooms = ActualRoomList[CurrentRoom].LinkedRooms;
        for (AMasterRoom* Neighbor : ConnectedRooms)
        {
            if (!Visit.Contains(Neighbor)) // 如果邻接房间未访问
            {
                Visit.Add(Neighbor); // 标记为已访问
                Queue.Enqueue(Neighbor); // 加入队列

                // 获取邻接房间的所有怪物生成点
                TArray<USceneComponent*> SpawnPoints;
                Neighbor->MonsterSpawnRoot->GetChildrenComponents(false, SpawnPoints);

                // 遍历每个生成点
                for (const auto SpawnPoint : SpawnPoints)
                {
                    // 根据概率判断是否生成怪物
                    if (Neighbor->GetClass()->IsChildOf(AChallengeRoom::StaticClass())|| CalculateRoomSpawnPointProbability(Neighbor))
                    {
                        // 随机选择一个怪物类
                        TSubclassOf<AFilling> Class = MonsterArray[SeedStream.RandRange(0, MonsterArray.Num() - 1)];
                        FActorSpawnParameters SpawnParams;
                        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // 忽略碰撞，强制生成
                    	
                    	AMonster* Monster = Cast<AMonster>(GetWorld()->SpawnActor(Class, &SpawnPoint->GetComponentTransform(), SpawnParams));// 生成怪物
                    	Monster->MasterRoom = Neighbor;
                    	if (CombatHelper->MonsterList.Find(Neighbor) == nullptr)
                    	{
                    		CombatHelper->MonsterList.Add(Neighbor);
                    	}
                    	CombatHelper->MonsterList[Neighbor].Monsters.Add(Monster);
                    	CombatHelper->MonsterList[Neighbor].MonsterCount++;
                    }
                }
            }
        }
    }

    // 在 Boss 房间生成 Boss
    USceneComponent* SpawnPoint = BossRoom->MonsterSpawnRoot->GetChildComponent(0); // 获取 Boss 生成点
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // 忽略碰撞，强制生成
	
    AMonster* Monster = Cast<AMonster>(GetWorld()->SpawnActor(BossClass, &SpawnPoint->GetComponentTransform(), SpawnParams)); // 生成 Boss
	Monster->MasterRoom = BossRoom;
	if (CombatHelper->MonsterList.Find(BossRoom) == nullptr)
	{
		CombatHelper->MonsterList.Add(BossRoom);
	}
	CombatHelper->MonsterList[BossRoom].Monsters.Add(Monster);
	CombatHelper->MonsterList[BossRoom].MonsterCount++;
}

void ADungeonGenerator::SpawnDoors()
{
	for (auto Element : DoorSpawnPoints)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // 忽略碰撞，强制生成
		ADoor* Door = Cast<ADoor>(GetWorld()->SpawnActor(RoomDoor, &Element->GetComponentTransform(), SpawnParams));
		AMasterRoom* Room = Cast<AMasterRoom>(Element->GetOwner());
		Room->DoorExistList.Add(Door);
	}
}

void ADungeonGenerator::CloseHoles()
{

	// 遍历所有已存在的房间连接点（ExistList）
	for (USceneComponent* Exist : ExistList)
	{
		// 获取当前连接点的变换（位置、旋转、缩放）
		FTransform ExistTransform = Exist->GetComponentTransform();

		// 设置生成参数
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // 忽略碰撞，强制生成

		// 在当前连接点的位置生成尽头墙（BP_EndWall）
		GetWorld()->SpawnActor<AActor>(EndWall, ExistTransform, SpawnParams);
	}
}

TTuple<TArray<TSubclassOf<AFilling>>, TArray<TSubclassOf<AFilling>>> ADungeonGenerator::MakeRoomFillingArray(
	FRoomFillingDB Row)
{
	// 定义宝箱填充数组，用于存储宝箱的类引用
	TArray<TSubclassOf<AFilling>> ChestFillingArray;

	// 遍历房间填充数据表中的宝箱部分
	for (auto Element : Row.Chest)
	{
		// 根据宝箱的权重（Element.Value）重复添加宝箱类到数组中
		for (int i = 0; i < Element.Value; i++)
		{
			ChestFillingArray.Add(Element.Key); // 将宝箱类添加到数组
		}
	}

	// 定义怪物填充数组，用于存储怪物的类引用
	TArray<TSubclassOf<AFilling>> MonsterFillingArray;

	// 遍历房间填充数据表中的怪物部分
	for (auto Element : Row.Monster)
	{
		// 根据怪物的权重（Element.Value）重复添加怪物类到数组中
		for (int i = 0; i < Element.Value; i++)
		{
			MonsterFillingArray.Add(Element.Key); // 将怪物类添加到数组
		}
	}

	// 返回包含宝箱和怪物填充数组的元组
	return {ChestFillingArray, MonsterFillingArray};
}

void ADungeonGenerator::SetRoomMaxDispersionDegree()
{
	//寻找最大房间离散度
	for (auto Element : RoomDispersionDegree)
	{
		if (Element.Value > MaxDispersion)
		{
			MaxDispersion = Element.Value;
		}
	}
}

void ADungeonGenerator::SpawnTrapsInChallengeRoom()
{
	for (auto Room : ActualRoomList)
	{
		if (Room.Key->GetClass()->IsChildOf(AChallengeRoom::StaticClass()))
		{
			AChallengeRoom* ThisRoom = Cast<AChallengeRoom>(Room.Key);

			ThisRoom->MakeTrapMap();
			
			for (const auto& Trap : ThisRoom->Traps)
			{
				for (const auto& SpawnPoint : Trap.Value.Components)
				{
					if (Trap.Key != nullptr && SpawnPoint != nullptr)
					{
						FActorSpawnParameters SpawnParams;
						SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
						GetWorld()->SpawnActor(Trap.Key, &SpawnPoint->GetComponentTransform(), SpawnParams);
					}
					
				}
			}
		}
	}
}




