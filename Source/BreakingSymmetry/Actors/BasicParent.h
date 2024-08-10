// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../DynamicEnumHeader.h"
#include "BasicParent.generated.h"

UCLASS()
class BREAKINGSYMMETRY_API ABasicParent : public AActor
{
	GENERATED_BODY()
	
public:	
	ABasicParent();
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	class UStaticMeshComponent* PlaneMesh;

	UPROPERTY(EditAnywhere)
	int priority;

	UPROPERTY(EditAnywhere)
	int level;

	UPROPERTY(EditAnywhere)
	DynamicType Type;

	ABasicParent* Next;
	ABasicParent* Prev;

	UPROPERTY(EditAnywhere)
	int x;

	UPROPERTY(EditAnywhere)
	int y;

	UPROPERTY(EditAnywhere)
	bool blue;

	void change_level(int level);
	void change_win(bool b);
	void change_defeat(bool b);
	void change_CanPass(bool b);

	bool IsWin;
	bool IsDefeat;
	bool CanPass;
	int LaserDirection;

	class AA_ArenaGameMode* A_GameMode;
};
