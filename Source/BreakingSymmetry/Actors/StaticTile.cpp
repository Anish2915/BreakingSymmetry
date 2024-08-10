// Fill out your copyright notice in the Description page of Project Settings.


#include "StaticTile.h"
#include "../Public/A_ArenaGameMode.h"
#include "A_Tile.h"
// Sets default values
AStaticTile::AStaticTile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PlaneMesh = CreateDefaultSubobject<UStaticMeshComponent>("PlaneMesh");
	SetRootComponent(PlaneMesh);
	//PlaneMesh->SetupAttachment(RootComponent);
	PlaneMesh->SetRelativeScale3D(FVector(0.195, 0.195, 0.195));
	PlaneMesh->AddRelativeRotation(FRotator(0,-90,0));
}

// Called when the game starts or when spawned
void AStaticTile::BeginPlay()
{
	Super::BeginPlay();
	
	GameMode = Cast<AA_ArenaGameMode>(GetWorld()->GetAuthGameMode());
}

// Called every frame
void AStaticTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AStaticTile::action(ABasicParent* DynamicClass)
{
}

void AStaticTile::reaction(ABasicParent* DynamicClass)
{
}





