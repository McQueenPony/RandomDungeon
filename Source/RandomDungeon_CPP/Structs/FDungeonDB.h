#pragma once
#include "FSpecialRoomStruct.h"
#include "RandomDungeon_CPP/ProgramDungeon/ChallengeRoom.h"
#include "RandomDungeon_CPP/ProgramDungeon/NormalRoom.h"
#include "FDungeonDB.generated.h"



USTRUCT(BlueprintType)
struct FDungeonDB : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FString Name;//一整套房间设置的名称，如丛林，冰雪

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TMap<TSubclassOf<ANormalRoom>, int> NormalPercent;//普通房间的比例
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<FSpecialRoomStruct> SpecialPercent;//特殊房间的比例

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TMap<TSubclassOf<AChallengeRoom>, int> ChallengeRoomPercent;//挑战房的比例

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int ChallengeRoomMaxNum = 0;//挑战房的最大数量
};