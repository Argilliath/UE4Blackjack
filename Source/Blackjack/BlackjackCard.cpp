// Fill out your copyright notice in the Description page of Project Settings.

#include "BlackjackCard.h"

#include "PaperSpriteComponent.h"

// Sets default values
ABlackjackCard::ABlackjackCard()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	FrontSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("FrontSprite"));
	FrontSprite->SetupAttachment(SceneRoot);
	BackSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("BackSprite"));
	BackSprite->SetupAttachment(SceneRoot);
}

void ABlackjackCard::SetCardInfo(const FCardInfo& Info, UPaperSprite* Back)
{
	CardInfo = Info;
	FrontSprite->SetSprite(Info.Sprite);
	BackSprite->SetSprite(Back);
}
