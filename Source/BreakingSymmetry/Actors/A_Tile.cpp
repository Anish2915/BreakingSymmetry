// Fill out your copyright notice in the Description page of Project Settings.


#include "A_Tile.h"
#include "BasicParent.h"
// Sets default values
AA_Tile::AA_Tile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PlaneMesh = CreateDefaultSubobject<UStaticMeshComponent>("PlaneMesh");
	PlaneMesh->SetupAttachment(RootComponent);
	//RootComponent = PlaneMesh;
	ObjectVector.resize(4);
	bIsInLaser = false;
}

// Called when the game starts or when spawned
void AA_Tile::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AA_Tile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

