// Fill out your copyright notice in the Description page of Project Settings.


#include "PassTile.h"
#include "../BasicParent.h"

void APassTile::action(ABasicParent* DynamicClass)
{
	if (!DynamicClass) return;
	if (int(DynamicClass->Type) >= 4 && int(DynamicClass->Type) <= 8) {
		if (DynamicClass->Next)
			DynamicClass->Next->change_CanPass(true);
	}
}

void APassTile::reaction(ABasicParent* DynamicClass)
{
	if (!DynamicClass) return;
	if (int(DynamicClass->Type) >= 4 && int(DynamicClass->Type) <= 8) {
		if (DynamicClass->Next)
			DynamicClass->Next->change_CanPass(true);
	}
}
