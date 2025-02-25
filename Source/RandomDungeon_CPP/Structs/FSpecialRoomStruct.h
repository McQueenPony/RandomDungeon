#pragma once
#include "RandomDungeon_CPP/ProgramDungeon/SpecialRoom.h"
#include "FSpecialRoomStruct.generated.h"

USTRUCT(BlueprintType)
struct FSpecialRoomStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ASpecialRoom> Class;//特殊房间的类

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Percent;//占整个房间数量的比例。以1-100为计，数字越小越早生成
    
};