// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicParent.h"
#include "../Public/A_ArenaGameMode.h"
#include "A_Tile.h"
// Sets default values
ABasicParent::ABasicParent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PlaneMesh = CreateDefaultSubobject<UStaticMeshComponent>("PlaneMesh");
	PlaneMesh->SetupAttachment(RootComponent);
	SetRootComponent(PlaneMesh);
	PlaneMesh->SetRelativeScale3D(FVector(0.195, 0.195, 0.195));
	PlaneMesh->AddRelativeRotation(FRotator(0, -90, 0));
	IsWin = false;
	LaserDirection = 0;
	priority = 4;
}

// Called when the game starts or when spawned
void ABasicParent::BeginPlay()
{
	Super::BeginPlay();
	A_GameMode = Cast<AA_ArenaGameMode>(GetWorld()->GetAuthGameMode());
}

UE_DISABLE_OPTIMIZATION;
void ABasicParent::change_level(int level_t)
{
	if (this->blue && A_GameMode->BlueTile[x][y]->ObjectVector[2] == A_GameMode->currPlayer1) {
		if (this->Next) return this->Next->change_level(level_t);
		return;
	}
	if (!this->blue && A_GameMode->RedTile[x][y]->ObjectVector[2] == A_GameMode->currPlayer2) {
		if (this->Next) return this->Next->change_level(level_t);
		return;
	}

	int prev_level = this->level;
	this->level = level_t;

	if (this->blue) { 
		A_GameMode->BlueTile[x][y]->ObjectVector[level_t] = this;
		A_GameMode->BlueTile[x][y]->ObjectVector[prev_level] = nullptr;
		A_GameMode->Update_Location_blue(A_GameMode->BlueTile[x][y]->ObjectVector[level_t], x, y); }
	else {
		A_GameMode->RedTile[x][y]->ObjectVector[level_t] = this;
		A_GameMode->RedTile[x][y]->ObjectVector[prev_level] = nullptr;
		A_GameMode->Update_Location_red(A_GameMode->RedTile[x][y]->ObjectVector[level_t], x, y);
	}
	if (!this->Next) return;
	this->Next->change_level(level_t);
}
UE_ENABLE_OPTIMIZATION;
void ABasicParent::change_win(bool b)
{
	IsWin = b;

	if (Next == nullptr) return;
	this->Next->change_win(b);
}

void ABasicParent::change_defeat(bool b)
{
	IsDefeat = b;
	if (Next == nullptr) return;
	this->Next->change_defeat(b);
}

void ABasicParent::change_CanPass(bool b)
{
	CanPass = b;

	if (Next == nullptr) return;
	this->Next->change_CanPass(b);
}

// Called every frame
void ABasicParent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

