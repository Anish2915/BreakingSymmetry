// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "A_Tile.generated.h"

class UStaticMeshComponent;

UENUM(BlueprintType)
enum class TileColor : uint8 {
	Blue ,
	Red
};

UCLASS()
class BREAKINGSYMMETRY_API AA_Tile : public AActor
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

public:	
	AA_Tile();
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	class UStaticMeshComponent* PlaneMesh;
	
	UPROPERTY(VisibleAnywhere)
	class AStaticTile* StaticTile;

	std::vector<class ABasicParent*> ObjectVector;

	UPROPERTY(EditAnywhere)
	bool blue;

	UPROPERTY(EditAnywhere)
	bool bIsInLaser;

	
};
