// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSLaunchPad.generated.h"

class UBoxComponent;

UCLASS()
class FPSGAME_API AFPSLaunchPad : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFPSLaunchPad();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
		UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
		UBoxComponent* OverlapComp;

	// Total impulse added to the character on overlap.
	UPROPERTY(EditInstanceOnly, Category = "LaunchPad")
		float LaunchStrength;

	// Angle added on top of actor rotation to launch the character.
	UPROPERTY(EditInstanceOnly, Category = "LaunchPad")
		float LaunchPitchAngle;

	// Effect to play when activating launch
	UPROPERTY(EditDefaultsOnly, Category = "FX")
		UParticleSystem* LaunchFX;

	void PlayEffect();

	// UFUNCTION() is called for the bind event
	UFUNCTION()
		void OverlapLaunchPad(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	
	
};
