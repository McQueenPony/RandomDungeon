#pragma once

#include "CoreMinimal.h"
#include "IRoomInterface.generated.h"

class AMasterRoom;

UINTERFACE()
class RANDOMDUNGEON_CPP_API URoomInterface : public UInterface
{
	GENERATED_BODY()
};

class RANDOMDUNGEON_CPP_API IRoomInterface
{
	GENERATED_BODY()
public:
	UFUNCTION()
	virtual  AMasterRoom* GetRoom() =0;
	
};
