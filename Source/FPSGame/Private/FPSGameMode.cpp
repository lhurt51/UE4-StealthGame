// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FPSGameMode.h"

#include "FPSHUD.h"
#include "FPSCharacter.h"
#include "FPSGameState.h"

#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

AFPSGameMode::AFPSGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/BP_Player"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFPSHUD::StaticClass();
	GameStateClass = AFPSGameState::StaticClass();
}

void AFPSGameMode::CompleteMission(APawn* InstigatorPawn, bool bMissionSuccess)
{
	if (InstigatorPawn)
	{
		if (SpectatingViewpointClass) {
			AActor* NewViewTarget = nullptr;
			TArray<AActor*> ReturnedActors;

			UGameplayStatics::GetAllActorsOfClass(this, SpectatingViewpointClass, ReturnedActors);
			// Change view target if any valid actor is found
			if (ReturnedActors.Num() > 0)
			{
				NewViewTarget = ReturnedActors[0];

				APlayerController* PC = Cast<APlayerController>(InstigatorPawn->GetController());
				if (PC) PC->SetViewTargetWithBlend(NewViewTarget, 0.5f, EViewTargetBlendFunction::VTBlend_Cubic);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("SpectatingViewpointClass is nullptr. Please update GameMode class with valid subclass. Cannot change spectating view target."));
		}
	}

	AFPSGameState* GS = GetGameState<AFPSGameState>();
	if (GS) GS->MulticastOnMissionComplete(InstigatorPawn, bMissionSuccess);

	OnMissionCompleted(InstigatorPawn, bMissionSuccess);
}
