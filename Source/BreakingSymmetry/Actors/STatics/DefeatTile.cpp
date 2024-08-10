// Fill out your copyright notice in the Description page of Project Settings.


#include "DefeatTile.h"
#include "../BasicParent.h"

void ADefeatTile::action(ABasicParent* DynamicClass)
{
	if (!DynamicClass) return;
	//if (DynamicClass->Type != DynamicType::TextRock && DynamicClass->Type != DynamicType::TextFood && DynamicClass->Type != DynamicType::Laser) return;
	if (int(DynamicClass->Type) >= 4 && int(DynamicClass->Type) <= 8) {
		if (DynamicClass->Next)
			DynamicClass->Next->change_defeat(true);
	}
}

void ADefeatTile::reaction(ABasicParent* DynamicClass)
{
	if (!DynamicClass) return;
	//if (DynamicClass->Type != DynamicType::TextRock && DynamicClass->Type != DynamicType::TextFood && DynamicClass->Type != DynamicType::Laser) return;
	if (int(DynamicClass->Type) >= 4 && int(DynamicClass->Type) <= 8) {
	if (DynamicClass->Next)
		DynamicClass->Next->change_defeat(false);
	}
}
