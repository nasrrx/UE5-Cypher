// Copyright Epic Games, Inc. All Rights Reserved.

#include "LinearProjectGameMode.h"
#include "LinearProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

ALinearProjectGameMode::ALinearProjectGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
