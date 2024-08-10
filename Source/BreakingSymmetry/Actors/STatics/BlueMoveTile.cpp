// Fill out your copyright notice in the Description page of Project Settings.


#include "BlueMoveTile.h"
#include "../BasicParent.h"
#include "A_ArenaGameMode.h"
UE_DISABLE_OPTIMIZATION
void ABlueMoveTile::action(ABasicParent* DynamicClass)
{
	if (!DynamicClass) return;
	if (int(DynamicClass->Type) >= 0 && int(DynamicClass->Type) <= 3) {
		GameMode->Func_Mapping[0][int(DynamicClass->Type)] = int(ArrowDir) + 1;
	}
}

void ABlueMoveTile::reaction(ABasicParent* DynamicClass)
{
	if (!DynamicClass) return;
	if (int(DynamicClass->Type) >= 0 && int(DynamicClass->Type) <= 3) {
		GameMode->Func_Mapping[0][int(DynamicClass->Type)] = 0;
	}
}
UE_ENABLE_OPTIMIZATION
