// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "BlackjackCommon.h"

#include "BlackjackSaveGame.generated.h"

USTRUCT()
struct FBlackjackSaveData
{
	GENERATED_BODY()

	UPROPERTY()
	EBlackjackGameState CurrentGameState;

	UPROPERTY()
	TArray<FCardInfo> Deck;
	
	UPROPERTY()
	TArray<FCardInfo> DealerCards;
	
	UPROPERTY()
	TArray<FSavePlayerInfo> PlayersInfo;
	
	UPROPERTY()
	bool bIsFirstChoice;
	
	UPROPERTY()
	int32 CurrentCardId = 0;
};

/**
 * 
 */
UCLASS()
class BLACKJACK_API UBlackjackSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	FBlackjackSaveData SaveData;
};
