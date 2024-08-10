// Fill out your copyright notice in the Description page of Project Settings.


#include "RedMoveTile.h"
#include "../BasicParent.h"
#include "A_ArenaGameMode.h"
UE_DISABLE_OPTIMIZATION
void ARedMoveTile::action(ABasicParent* DynamicClass)
{
	if (!DynamicClass) return;
	if (int(DynamicClass->Type) >= 0 && int(DynamicClass->Type) <= 3) {
		GameMode->Func_Mapping[1][int(DynamicClass->Type)] = int(ArrowDir) + 1;
		UE_LOG(LogTemp, Warning, TEXT("RedMove action type %d"), int(DynamicClass->Type));
	}
}

void ARedMoveTile::reaction(ABasicParent* DynamicClass)
{
	UE_LOG(LogTemp, Warning, TEXT("RedMove Reaction start"));
	if (!DynamicClass) return;
	UE_LOG(LogTemp, Warning, TEXT("RedMove reaction type %d"), int(DynamicClass->Type));
	if (int(DynamicClass->Type) >= 0 && int(DynamicClass->Type) <= 3) {
		GameMode->Func_Mapping[1][int(DynamicClass->Type)] = 0;
	}
}
UE_ENABLE_OPTIMIZATION
