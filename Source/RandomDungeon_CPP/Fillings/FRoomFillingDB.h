#pragma once
#include "Filling.h"
#include "FRoomFillingDB.generated.h"

USTRUCT(BlueprintType)
struct FRoomFillingDB : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TMap<TSubclassOf<AFilling>, int> Chest;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TMap<TSubclassOf<AFilling>, int> Monster;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<AFilling> Boss;
};