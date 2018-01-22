// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSAIGuard.h"

#include "FPSGameMode.h"

#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "AI/Navigation/NavigationSystem.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AFPSAIGuard::AFPSAIGuard()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));

	GuardState = EAIState::Idle;

	CurrentPatrolPointIndex = -1;
}

// Called when the game starts or when spawned
void AFPSAIGuard::BeginPlay()
{
	Super::BeginPlay();
	
	if (!ensure(PawnSensingComp != nullptr)) return;
	PawnSensingComp->OnSeePawn.AddDynamic(this, &AFPSAIGuard::OnPawnSee);
	PawnSensingComp->OnHearNoise.AddDynamic(this, &AFPSAIGuard::OnNoiseHeard);

	OriginalRotation = GetActorRotation();

	if (bPatrol) MoveToNextPatrolPoint();
}

void AFPSAIGuard::OnPawnSee(APawn* SeenPawn)
{
	if (SeenPawn == nullptr) return;

	DrawDebugSphere(GetWorld(), SeenPawn->GetActorLocation(), 32.0f, 12, FColor::Red, false, 10.0f);

	AFPSGameMode* GM = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
	if (GM) GM->CompleteMission(SeenPawn, false);

	SetGuardState(EAIState::Alerted);
	GetWorldTimerManager().ClearTimer(TimerHandle_ResetOrientation);

	AController* Controller = GetController();
	if (Controller) Controller->StopMovement();
}

void AFPSAIGuard::OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume)
{
	if (GuardState == EAIState::Alerted) return;

	DrawDebugSphere(GetWorld(), Location, 32.0f, 12, FColor::Green, false, 10.0f);

	FVector Direction = Location - GetActorLocation();
	Direction.Normalize();

	FRotator NewLookAt = FRotationMatrix::MakeFromX(Direction).Rotator();
	NewLookAt.Pitch = 0.0f;
	NewLookAt.Roll = 0.0f;

	SetActorRotation(NewLookAt);

	GetWorldTimerManager().ClearTimer(TimerHandle_ResetOrientation);
	GetWorldTimerManager().SetTimer(TimerHandle_ResetOrientation, this, &AFPSAIGuard::ResetOrientation, 3.0f);

	SetGuardState(EAIState::Suspicious);

	AController* Controller = GetController();
	if (Controller) Controller->StopMovement();
}

void AFPSAIGuard::ResetOrientation()
{
	if (GuardState == EAIState::Alerted) return;

	SetActorRotation(OriginalRotation);
	SetGuardState(EAIState::Idle);

	if (bPatrol) MoveToNextPatrolPoint();
}

void AFPSAIGuard::SetGuardState(EAIState NewState)
{
	if (GuardState == NewState) return;
	GuardState = NewState;
	OnRep_GuardState();
}

void AFPSAIGuard::OnRep_GuardState()
{
	OnStateChanged(GuardState);
}

void AFPSAIGuard::MoveToNextPatrolPoint()
{
	static int8 increment = 0;

	if (CurrentPatrolPointIndex >= PatrolPoints.Num() - 1)
		increment = -1;
	else if (CurrentPatrolPointIndex <= 0)
		increment = 1;

	CurrentPatrolPointIndex += increment;
	if (CurrentPatrolPointIndex > -1 && CurrentPatrolPointIndex < PatrolPoints.Num()) UNavigationSystem::SimpleMoveToActor(GetController(), PatrolPoints[CurrentPatrolPointIndex]);
}

// Called every frame
void AFPSAIGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentPatrolPointIndex > -1 && CurrentPatrolPointIndex < PatrolPoints.Num())
	{
		FVector Delta = GetActorLocation() - PatrolPoints[CurrentPatrolPointIndex]->GetActorLocation();
		float DistanceToGoal = Delta.Size();

		if (DistanceToGoal < 50.0f && PatrolPoints.Num() > 1) MoveToNextPatrolPoint();
	}
}

void AFPSAIGuard::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSAIGuard, GuardState);
}
