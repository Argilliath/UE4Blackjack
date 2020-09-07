// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Math/NumericLimits.h"
#include "BlackjackCommon.generated.h"

// Common static values
constexpr int32 DECK_SIZE = 52;
constexpr int32 PER_COLOR_COUNT = DECK_SIZE / 4;
constexpr int32 MAX_NUM_PLAYERS = 1;
constexpr int32 BLACKJACK_VALUE = 21;
constexpr int32 BLACKJACK_CARDS = 2;
constexpr int32 ACE_MINUS_VALUE = 10;
constexpr float BLACKJACK_PAYOUT_MULTIPLIER = 2.5f; 
constexpr float WIN_PAYOUT_MULTIPLIER = 2.0f;

// Save game statics
static FString SAVE_GAME_SLOT = FString(TEXT("SaveGame"));
static int32 SAVE_GAME_INDEX = 0;

class ABlackjackCard;
class UPaperSprite;

UENUM(BlueprintType)
enum class EBlackjackResult : uint8
{
	DealerWin,
	PlayerWin,
	PlayerSurrender,
	Push
};

UENUM(BlueprintType)
enum class EBlackjackPlay : uint8
{
	Hit,
	Stand,
	Double,
	Surrender,
	Split
};

USTRUCT(BlueprintType)
struct FCardInfo
{
	GENERATED_BODY();

	bool IsAce() const { return ValueId == 0; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ValueId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 GameValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPaperSprite* Sprite;
};

// Very similar to FPlayerInfo but with FCardInfos instead of the actual card instances
USTRUCT()
struct FSavePlayerInfo
{
	GENERATED_BODY();

	UPROPERTY()
	float OnHandMoney;

	UPROPERTY()
	int32 OnBetMoney;

	UPROPERTY()
	TArray<FCardInfo> HandCards;

	UPROPERTY()
	EBlackjackPlay ChosenPlay;

	UPROPERTY()
	EBlackjackResult RoundResult;

	UPROPERTY()
	int32 RemainingDealsLeft;
};

USTRUCT(BlueprintType)
struct FPlayerInfo
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float OnHandMoney = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 OnBetMoney = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ABlackjackCard*> HandCards;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EBlackjackPlay ChosenPlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EBlackjackResult RoundResult = EBlackjackResult::DealerWin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RemainingDealsLeft = TNumericLimits<int32>::Max();
};

UENUM(BlueprintType)
enum class EBlackjackGameState : uint8
{
	RoundStart,
	WaitForBet,
	InitialShuffle,
	InitialDeal,
	WaitForPlayerChoice,
	WaitForCardDeal,
	DealerDeal,
	RoundResults,
	Invalid
};
