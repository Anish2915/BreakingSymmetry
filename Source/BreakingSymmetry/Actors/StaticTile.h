// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Interface/I_ParentStatic.h"
#include "StaticTile.generated.h"

class ABasicParent;

UCLASS()
class BREAKINGSYMMETRY_API AStaticTile : public AActor , public II_ParentStatic
{
	GENERATED_BODY()
	
public:	
	AStaticTile();

protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	class UStaticMeshComponent* PlaneMesh;

	virtual void Tick(float DeltaTime) override;
	virtual void action(ABasicParent* DynamicClass) override;
	virtual void reaction(ABasicParent* DynamicClass) override;


	UPROPERTY(EditAnywhere)
	int x;
	
	UPROPERTY(EditAnywhere)
	int y;

	class AA_ArenaGameMode* GameMode;

	bool Blue;
};
