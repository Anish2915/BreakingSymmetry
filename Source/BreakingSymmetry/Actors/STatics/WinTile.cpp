// Fill out your copyright notice in the Description page of Project Settings.


#include "WinTile.h"
#include "../BasicParent.h"

void AWinTile::action(ABasicParent* DynamicClass)
{
	if (!DynamicClass) return;
	//if (DynamicClass->Type != DynamicType::TextRock && DynamicClass->Type != DynamicType::TextFood && DynamicClass->Type != DynamicType::Laser) return;
	if (int(DynamicClass->Type) >= 4 && int(DynamicClass->Type) <= 8) {
		if (DynamicClass->Next)
			DynamicClass->Next->change_win(true);
	}
}

void AWinTile::reaction(ABasicParent* DynamicClass)
{
	if (!DynamicClass) return;
	//if (DynamicClass->Type != DynamicType::TextRock && DynamicClass->Type != DynamicType::TextFood && DynamicClass->Type != DynamicType::Laser) return;
	if (int(DynamicClass->Type) >= 4 && int(DynamicClass->Type) <= 8) {
		if (DynamicClass->Next)
			DynamicClass->Next->change_win(false);
	}
}
