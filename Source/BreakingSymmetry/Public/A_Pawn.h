// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "A_Pawn.generated.h"

UCLASS()
class BREAKINGSYMMETRY_API AA_Pawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AA_Pawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void W_Pressed();
	
	UFUNCTION(BlueprintCallable)
	void S_Pressed();
	
	UFUNCTION(BlueprintCallable)
	void A_Pressed();
	
	UFUNCTION(BlueprintCallable)
	void D_Pressed();

	class AA_ArenaGameMode* GameMode;


};
