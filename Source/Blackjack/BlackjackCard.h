// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "BlackjackCommon.h"
#include "BlackjackCard.generated.h"

class UPaperSprite;
class UPaperSpriteComponent;

UCLASS()
class BLACKJACK_API ABlackjackCard : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABlackjackCard();

	UFUNCTION(BlueprintCallable, Category = Gameplay)
	void SetCardInfo(const FCardInfo& Info, UPaperSprite* Back);

	UFUNCTION(BlueprintCallable, Category = Gameplay)
	FORCEINLINE FCardInfo& GetCardInfo() { return CardInfo; }

	FORCEINLINE USceneComponent* GetSceneRoot() const { return SceneRoot; }

	FORCEINLINE UPaperSpriteComponent* GetFrontSprite() const { return FrontSprite; }

	FORCEINLINE UPaperSpriteComponent* GetBackSprite() const { return BackSprite; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CardInfo)
	FCardInfo CardInfo;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Sprite, meta = (AllowPrivateAccess = "true"))
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Sprite, meta = (AllowPrivateAccess = "true"))
	UPaperSpriteComponent* FrontSprite;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Sprite, meta = (AllowPrivateAccess = "true"))
	UPaperSpriteComponent* BackSprite;
};
