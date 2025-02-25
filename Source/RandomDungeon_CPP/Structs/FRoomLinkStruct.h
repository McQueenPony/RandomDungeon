#pragma once


#include "FRoomLinkStruct.generated.h"

class AMasterRoom;

USTRUCT(BlueprintType)
struct FRoomLinkStruct
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AMasterRoom*> LinkedRooms;

	
};
