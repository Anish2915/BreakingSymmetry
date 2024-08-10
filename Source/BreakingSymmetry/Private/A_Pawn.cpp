// Fill out your copyright notice in the Description page of Project Settings.


#include "A_Pawn.h"
#include "A_ArenaGameMode.h"
#include "../KeyEnum.h"
// Sets default values
AA_Pawn::AA_Pawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AA_Pawn::BeginPlay()
{
	Super::BeginPlay();
	GameMode = Cast<AA_ArenaGameMode>(GetWorld()->GetAuthGameMode());
	EnableInput(GetWorld()->GetFirstPlayerController());
}

// Called every frame
void AA_Pawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AA_Pawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UE_LOG(LogTemp, Warning, TEXT("Check"));
	PlayerInputComponent->BindAction("Up", EInputEvent::IE_Pressed , this, &AA_Pawn::W_Pressed);
	PlayerInputComponent->BindAction("Down", EInputEvent::IE_Pressed, this, &AA_Pawn::S_Pressed);
	PlayerInputComponent->BindAction("Left", EInputEvent::IE_Pressed, this, &AA_Pawn::A_Pressed);
	PlayerInputComponent->BindAction("Right", EInputEvent::IE_Pressed, this, &AA_Pawn::D_Pressed);

}

void AA_Pawn::W_Pressed() {
	UE_LOG(LogTemp, Warning, TEXT("W_Pressed"));
	if(GameMode->currPlayer1)
	GameMode->MovementDelegates[GameMode->Func_Mapping[0][0]].ExecuteIfBound(GameMode->currPlayer1);
	if(GameMode->currPlayer2)
	GameMode->MovementDelegates[GameMode->Func_Mapping[1][0]].ExecuteIfBound(GameMode->currPlayer2);
}

void AA_Pawn::S_Pressed() {
	UE_LOG(LogTemp, Warning, TEXT("s_Pressed"));
	if (GameMode->currPlayer1)
	GameMode->MovementDelegates[GameMode->Func_Mapping[0][1]].ExecuteIfBound(GameMode->currPlayer1);
	if (GameMode->currPlayer2)
	GameMode->MovementDelegates[GameMode->Func_Mapping[1][1]].ExecuteIfBound(GameMode->currPlayer2);
}

void AA_Pawn::A_Pressed() {
	UE_LOG(LogTemp, Warning, TEXT("a_Pressed"));
	if (GameMode->currPlayer1)
	GameMode->MovementDelegates[GameMode->Func_Mapping[0][2]].ExecuteIfBound(GameMode->currPlayer1);
	if (GameMode->currPlayer2)
	GameMode->MovementDelegates[GameMode->Func_Mapping[1][2]].ExecuteIfBound(GameMode->currPlayer2);
}

void AA_Pawn::D_Pressed() {
	UE_LOG(LogTemp, Warning, TEXT("d_Pressed"));
	if (GameMode->currPlayer1)
	GameMode->MovementDelegates[GameMode->Func_Mapping[0][3]].ExecuteIfBound(GameMode->currPlayer1);
	if (GameMode->currPlayer2)
	GameMode->MovementDelegates[GameMode->Func_Mapping[1][3]].ExecuteIfBound(GameMode->currPlayer2);
}

