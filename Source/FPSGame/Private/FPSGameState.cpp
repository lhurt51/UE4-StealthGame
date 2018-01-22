// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSGameState.h"


void AFPSGameState::MulticastOnMissionComplete_Implementation(APawn* InstigatorPawn, bool bMissionSuccess)
{
	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; It++) // This includes enemies remember!
	{
		APawn* Pawn = It->Get();
		if (Pawn && Pawn->IsLocallyControlled())
		{
			Pawn->DisableInput(nullptr);
		}
	}
}
