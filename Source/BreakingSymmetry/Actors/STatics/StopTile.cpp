// Fill out your copyright notice in the Description page of Project Settings.


#include "StopTile.h"
#include "../BasicParent.h"

void AStopTile::action(ABasicParent* DynamicClass)
{
	if (!DynamicClass) return;
	if (int(DynamicClass->Type) >= 4 && int(DynamicClass->Type) <= 8) {
		if (DynamicClass->Next)
			DynamicClass->Next->change_level(3);
	}
}

void AStopTile::reaction(ABasicParent* DynamicClass)
{
	if (!DynamicClass) return;
	if (int(DynamicClass->Type) >= 4 && int(DynamicClass->Type) <= 8) {
		if (DynamicClass->Next)
			DynamicClass->Next->change_level(1);
	}
}
