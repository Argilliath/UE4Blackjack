// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "BlackjackCommon.h"
#include "BlackjackGameMode.generated.h"

class ABlackjackCard;
class UPaperSprite;
class UUserWidget;

UCLASS()
class BLACKJACK_API ABlackjackGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = Gameplay)
	void ChangeGameState(EBlackjackGameState NewState);

	UFUNCTION(BlueprintCallable, Category = Gameplay)
	void ConfirmBet(int32 PlayerId, int32 BetAmount);

	UFUNCTION(BlueprintCallable, Category = Gameplay)
	void ConfirmPlayerChoice(int32 PlayerId, EBlackjackPlay Choice);

	UFUNCTION(BlueprintCallable, Category = Gameplay)
	int32 GetPlayerGameValue(int32 PlayerId) const;

	UFUNCTION(BlueprintCallable, Category = Gameplay)
	bool HasBlackjack(int32 PlayerId) const;

	UFUNCTION(BlueprintCallable, Category = Gameplay)
	bool AreAllPlayersDone() const;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent, Category = Gameplay)
	void OnGameStateChanged(EBlackjackGameState OldState);

	void BuildAndShuffleDecks();
	void CleanupCards();
	void SpreadCards(FTransform MiddleSpot, const TArray<ABlackjackCard*>& Cards);

	UFUNCTION(BlueprintCallable, Category = Gameplay)
	void GiveCard(int32 PlayerId, ABlackjackCard* Card);

	UFUNCTION(BlueprintCallable, Category = Gameplay)
	void CheckRoundResults();

	UFUNCTION(BlueprintCallable, Category = Gameplay)
	void SetPlayerRoundResult(int32 PlayerId, EBlackjackResult Result);

	UFUNCTION(BlueprintCallable, Category = Gameplay)
	void ProcessInitialDeal();

	UFUNCTION(BlueprintCallable, Category = Gameplay)
	void ProcessDealersPlay();

	UFUNCTION(BlueprintCallable, Category = Gameplay)
	void CheckForDealerBlackjack();

	UFUNCTION(BlueprintCallable, Category = Gameplay)
	void CollectBetMoney();

	UFUNCTION(BlueprintCallable, Category = Gameplay)
	void CheckForPlayersBust();

	UFUNCTION(BlueprintCallable, Category = Gameplay)
	ABlackjackCard* SpawnNextCard();
	
	// Config variables
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = GameConfig)
	int32 InitialPlayerMoney = 1000;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = GameConfig)
	int32 MinBetAmount = 100;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = GameConfig)
	int32 DeckCount = 8;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = GameConfig)
	int32 DealerStandValue = 17;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = GameConfig)
	FVector CardSpread = FVector(100.0f, 5.0f, 2.0f);

	// Game interface class
	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<UUserWidget> GameUIClass;

	// Card template
	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	TSubclassOf<ABlackjackCard> CardTemplate;

	// Back sprite for the cards, this one will be shared
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Sprite)
	UPaperSprite* BackSprite = nullptr;

	// Front sprites for the cards
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Sprite)
	TArray<UPaperSprite*> CardSprites;

	// Special target actors and tags so that cards can move to specific positions
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera)
	FName MainCameraTag = FName(TEXT("MainCamera"));

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = CardMovement)
	FName DealerSpotTag = FName(TEXT("DealerSpot"));

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = CardMovement)
	FName PlayerSpotTag = FName(TEXT("PlayerSpot"));

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = CardMovement)
	FName CardDealSpotTag = FName(TEXT("CardDealSpot"));

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CardMovement)
	FTransform DealerSpot;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CardMovement)
	FTransform PlayerSpot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CardMovement)
	FTransform CardDealSpot;

	// Game variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gameplay)
	EBlackjackGameState CurrentGameState = EBlackjackGameState::RoundStart;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gameplay)
	TArray<FCardInfo> Deck;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gameplay)
	TArray<ABlackjackCard*> DealerCards;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gameplay)
	TArray<FPlayerInfo> PlayersInfo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gameplay)
	bool bIsFirstChoice = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = UI)
	UUserWidget* GameUI;

private:
	int32 GetGameValueFromCards(const TArray<ABlackjackCard*>& Cards) const;

	int32 CurrentCardId = 0;
};
