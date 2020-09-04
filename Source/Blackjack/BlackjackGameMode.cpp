// Fill out your copyright notice in the Description page of Project Settings.

#include "BlackjackGameMode.h"

#include "BlackjackCard.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraActor.h"
#include "Engine/TargetPoint.h"
#include "Kismet/GameplayStatics.h"
#pragma optimize("", off)
void ABlackjackGameMode::ChangeGameState(EBlackjackGameState NewState)
{
	if (NewState != CurrentGameState)
	{
		EBlackjackGameState OldState = CurrentGameState;
		CurrentGameState = NewState;
		if (CurrentGameState == EBlackjackGameState::RoundStart)
		{
			CleanupCards();
			CurrentCardId = 0;
			bIsFirstChoice = true;
		}
		else if (CurrentGameState == EBlackjackGameState::InitialShuffle)
		{
			BuildAndShuffleDecks();
		}

		OnGameStateChanged(OldState);
	}
}

void ABlackjackGameMode::ConfirmBet(int32 PlayerId, int32 BetAmount)
{
	check(PlayerId >= 0 && PlayerId < PlayersInfo.Num());

	PlayersInfo[PlayerId].OnBetMoney = BetAmount;
	PlayersInfo[PlayerId].OnHandMoney -= static_cast<float>(BetAmount);
}

void ABlackjackGameMode::ConfirmPlayerChoice(int32 PlayerId, EBlackjackPlay Choice)
{
	check(PlayerId >= 0 && PlayerId < PlayersInfo.Num());

	if (bIsFirstChoice && Choice == EBlackjackPlay::Surrender)
	{
		PlayersInfo[PlayerId].OnHandMoney += static_cast<float>(PlayersInfo[PlayerId].OnBetMoney / 2);
		PlayersInfo[PlayerId].OnBetMoney = 0;
		PlayersInfo[PlayerId].RemainingDealsLeft = 0;
		PlayersInfo[PlayerId].RoundResult = EBlackjackResult::PlayerSurrender;
	}
	else if (Choice == EBlackjackPlay::Stand)
	{
		PlayersInfo[PlayerId].RemainingDealsLeft = 0;
	}
	else if (Choice == EBlackjackPlay::Double)
	{
		PlayersInfo[PlayerId].OnHandMoney -= static_cast<float>(PlayersInfo[PlayerId].OnBetMoney);
		PlayersInfo[PlayerId].OnBetMoney *= 2;
		PlayersInfo[PlayerId].RemainingDealsLeft = 1;
	}

	bIsFirstChoice = false;
}

void ABlackjackGameMode::GiveCard(int32 PlayerId, ABlackjackCard* Card)
{
	check(PlayerId < PlayersInfo.Num() && Card != nullptr);

	// Assume this is a card for the dealer
	if (PlayerId < 0)
	{
		DealerCards.Add(Card);
		Card->SetActorTransform(DealerSpot);

		// In the scene, the dealers spot points backwards, if this is not the first card, rotate it
		if (DealerCards.Num() > 1)
		{
			Card->AddActorLocalRotation(FRotator(0.0f, 180.0f, 0.0f).Quaternion());
		}
		
		SpreadCards(DealerSpot, DealerCards);
	}
	else
	{
		PlayersInfo[PlayerId].HandCards.Add(Card);
		Card->SetActorTransform(PlayerSpot);

		SpreadCards(PlayerSpot, PlayersInfo[PlayerId].HandCards);
		PlayersInfo[PlayerId].RemainingDealsLeft -= 1;
	}
}

void ABlackjackGameMode::CheckRoundResults()
{
	int32 DealerValue = GetPlayerGameValue(-1);
	for (int32 i = 0; i < PlayersInfo.Num(); ++i)
	{
		EBlackjackResult Result = PlayersInfo[i].RoundResult;
		if (Result != EBlackjackResult::PlayerSurrender && Result != EBlackjackResult::PlayerWin)
		{
			int32 PlayerValue = GetPlayerGameValue(i);
			if (PlayerValue <= BLACKJACK_VALUE)
			{
				if (DealerValue > BLACKJACK_VALUE || PlayerValue > DealerValue)
				{
					SetPlayerRoundResult(i, EBlackjackResult::PlayerWin);
				}
				else if (PlayerValue == DealerValue)
				{
					SetPlayerRoundResult(i, EBlackjackResult::Push);
				}
				else
				{
					SetPlayerRoundResult(i, EBlackjackResult::DealerWin);
				}
			}
			else
			{
				SetPlayerRoundResult(i, EBlackjackResult::DealerWin);
			}
		}
	}
}

void ABlackjackGameMode::SetPlayerRoundResult(int32 PlayerId, EBlackjackResult Result)
{
	check(PlayerId >= 0 && PlayerId < PlayersInfo.Num());

	PlayersInfo[PlayerId].RoundResult = Result;
}

void ABlackjackGameMode::ProcessInitialDeal()
{
	for (int32 i = 0; i < PlayersInfo.Num(); ++i)
	{
		GiveCard(i, SpawnNextCard());
		GiveCard(i, SpawnNextCard());
	}

	GiveCard(-1, SpawnNextCard());
	GiveCard(-1, SpawnNextCard());
}

void ABlackjackGameMode::ProcessDealersPlay()
{
	// Flip the first card
	DealerCards[0]->AddActorLocalRotation(FRotator(0.0f, 180.0f, 0.0f).Quaternion());

	while (GetPlayerGameValue(-1) <= DealerStandValue)
	{
		GiveCard(-1, SpawnNextCard());
	}
}

void ABlackjackGameMode::CheckForDealerBlackjack()
{
	if (HasBlackjack(-1))
	{
		for (int32 i = 0; i < PlayersInfo.Num(); ++i)
		{
			bool bHasBlackjack = HasBlackjack(i);
			if (bHasBlackjack)
			{
				PlayersInfo[i].RemainingDealsLeft = 0;
			}
			SetPlayerRoundResult(i, bHasBlackjack ? EBlackjackResult::Push : EBlackjackResult::DealerWin);
		}

		ChangeGameState(EBlackjackGameState::RoundResults);
	}
	else if(AreAllPlayersDone())
	{
		ChangeGameState(EBlackjackGameState::DealerDeal);
	}
	else
	{
		ChangeGameState(EBlackjackGameState::WaitForPlayerChoice);
	}
}

void ABlackjackGameMode::CollectBetMoney()
{
	int32 i = 0;
	for (FPlayerInfo& Info : PlayersInfo)
	{
		if (Info.RoundResult == EBlackjackResult::PlayerWin)
		{
			float Multiplier = HasBlackjack(i) ? BLACKJACK_PAYOUT_MULTIPLIER : WIN_PAYOUT_MULTIPLIER;
			Info.OnHandMoney += static_cast<float>(Info.OnBetMoney) * Multiplier;
		}

		Info.OnBetMoney = 0;
		++i;
	}
}

void ABlackjackGameMode::CheckForPlayersBust()
{
	for (int32 i = 0; i < PlayersInfo.Num(); ++i)
	{
		if (GetPlayerGameValue(i) > BLACKJACK_VALUE)
		{
			PlayersInfo[i].RemainingDealsLeft = 0;
			PlayersInfo[i].RoundResult = EBlackjackResult::DealerWin;
		}
	}
}

// This function is the one that computes the value of a hand
int32 ABlackjackGameMode::GetGameValueFromCards(const TArray<ABlackjackCard*>& Cards) const
{
	int32 Value = 0;
	int32 AcesCount = 0;
	for (ABlackjackCard* Card : Cards)
	{
		FCardInfo& Info = Card->GetCardInfo();
		if (Info.IsAce())
		{
			AcesCount++;
		}
		
		Value += Info.GameValue;
	}

	while (Value > BLACKJACK_VALUE && AcesCount > 0)
	{
		Value -= ACE_MINUS_VALUE;
		AcesCount--;
	}

	return Value;
}

int32 ABlackjackGameMode::GetPlayerGameValue(int32 PlayerId) const
{
	check(PlayerId < PlayersInfo.Num());

	// Assume this is a card for the dealer
	if (PlayerId < 0)
	{
		return GetGameValueFromCards(DealerCards);
	}

	return GetGameValueFromCards(PlayersInfo[PlayerId].HandCards);
}

bool ABlackjackGameMode::HasBlackjack(int32 PlayerId) const
{
	check(PlayerId < PlayersInfo.Num());

	int32 Value = GetPlayerGameValue(PlayerId);
	int32 CardCount = PlayerId < 0 ? DealerCards.Num() : PlayersInfo[PlayerId].HandCards.Num();

	return Value == BLACKJACK_VALUE && CardCount == BLACKJACK_CARDS;
}

bool ABlackjackGameMode::AreAllPlayersDone() const
{
	for (const FPlayerInfo& Info : PlayersInfo)
	{
		if (Info.RemainingDealsLeft > 0)
		{
			return false;
		}
	}

	return true;
}

void ABlackjackGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Set Camera can Interface
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClassWithTag(this, ACameraActor::StaticClass(), MainCameraTag, OutActors);
	if (OutActors.Num() > 0)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
		if (IsValid(PC))
		{
			PC->SetViewTarget(OutActors[0]);
			
			GameUI = CreateWidget<UUserWidget>(PC, GameUIClass);
			if (IsValid(GameUI))
			{
				GameUI->AddToPlayerScreen();
			}
		}
	}

	// Get special positions for the card movement
	OutActors.Empty();
	UGameplayStatics::GetAllActorsOfClass(this, ATargetPoint::StaticClass(), OutActors);
	for (AActor* Target : OutActors)
	{
		if (IsValid(Target))
		{
			if (Target->ActorHasTag(DealerSpotTag))
			{
				DealerSpot = Target->GetActorTransform();
			}
			else if (Target->ActorHasTag(PlayerSpotTag))
			{
				PlayerSpot = Target->GetActorTransform();
			}
			else if (Target->ActorHasTag(CardDealSpotTag))
			{
				CardDealSpot = Target->GetActorTransform();
			}
		}
	}

	// Init players
	for (int32 i = 0; i < MAX_NUM_PLAYERS; ++i)
	{
		FPlayerInfo& Info = PlayersInfo.Emplace_GetRef();
		Info.OnHandMoney = static_cast<float>(InitialPlayerMoney);
	}

	// Start round
	ChangeGameState(EBlackjackGameState::WaitForBet);
}

void ABlackjackGameMode::BuildAndShuffleDecks()
{
	Deck.Empty();

	for (int32 i = 0; i < DECK_SIZE * DeckCount; ++i)
	{
		int32 DeckId = i % DECK_SIZE;
		int32 ValueId = i % PER_COLOR_COUNT;
		int32 Value = ValueId == 0 ? 11 : (ValueId < 9 ? ValueId + 1 : 10);
		FCardInfo& Info = Deck.Emplace_GetRef();
		Info.ValueId = ValueId;
		Info.GameValue = Value;
		Info.Sprite = CardSprites[DeckId];
	}

	// Shuffle the deck
	const int32 NumShuffles = Deck.Num() - 1;
	for (int32 i = 0; i < NumShuffles; ++i)
	{
		int32 SwapId = FMath::RandRange(i, NumShuffles);
		Deck.Swap(i, SwapId);
	}
}

void ABlackjackGameMode::CleanupCards()
{
	// Cleanup cards
	for (ABlackjackCard* Card : DealerCards)
	{
		Card->Destroy();
	}
	DealerCards.Empty();

	for (FPlayerInfo& Info : PlayersInfo)
	{
		for (ABlackjackCard* Card : Info.HandCards)
		{
			Card->Destroy();
		}
		Info.HandCards.Empty();
		Info.RemainingDealsLeft = TNumericLimits<int32>::Max();
		Info.RoundResult = EBlackjackResult::DealerWin;
	}
}

void ABlackjackGameMode::SpreadCards(FTransform MiddleSpot, const TArray<ABlackjackCard*>& Cards)
{
	float CurrentId = 0.0f;
	float HalfTotalSpread = (CardSpread.X * static_cast<float>(Cards.Num() - 1)) * 0.5f;
	for (ABlackjackCard* Card : Cards)
	{
		FVector SpreadLocation;
		FVector InitialLocation = MiddleSpot.GetLocation();
		SpreadLocation.X = (InitialLocation.X + (CardSpread.X * CurrentId)) - HalfTotalSpread;
		SpreadLocation.Y = InitialLocation.Y + (CardSpread.Y * CurrentId);
		SpreadLocation.Z = InitialLocation.Z + (CardSpread.Z * CurrentId);
		Card->SetActorLocation(SpreadLocation);
		CurrentId += 1.0f;
	}
}

ABlackjackCard* ABlackjackGameMode::SpawnNextCard()
{
	if (!IsValid(CardTemplate))
	{
		return nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ABlackjackCard* NewCard = GetWorld()->SpawnActor<ABlackjackCard>(CardTemplate, SpawnParams);
	NewCard->SetCardInfo(Deck[CurrentCardId], BackSprite);
	NewCard->SetActorTransform(CardDealSpot);
	CurrentCardId++;

	return NewCard;
}
#pragma optimize("", on)