// Fill out your copyright notice in the Description page of Project Settings.


#include "PushTile.h"
#include "../BasicParent.h"
#include "A_ArenaGameMode.h"

void APushTile::action(ABasicParent* DynamicClass)
{
	if (!DynamicClass) return;
	if (int(DynamicClass->Type) >= 4 && int(DynamicClass->Type) <= 8) {
		if (DynamicClass->Next) {
			DynamicClass->Next->change_level(2);
		}
	}
}

void APushTile::reaction(ABasicParent* DynamicClass)
{
	if (!DynamicClass) return;
	if (int(DynamicClass->Type) >= 4 && int(DynamicClass->Type) <= 8) {
		if (DynamicClass->Next)
			DynamicClass->Next->change_level(1);
	}
}
