// Fill out your copyright notice in the Description page of Project Settings.


#include "Player1Tile.h"
#include "../BasicParent.h"
#include "A_ArenaGameMode.h"
#include "../A_Tile.h"

void APlayer1Tile::action(ABasicParent* DynamicClass)
{
	if (!DynamicClass) return;
	if (int(DynamicClass->Type) >= 4 && int(DynamicClass->Type)<=8) {
		// GameMode->currPlayer1->level = 1;
		int prev_l = DynamicClass->Next->level;
		int nx = DynamicClass->Next->x, ny = DynamicClass->Next->y;
		GameMode->BlueTile[nx][ny]->ObjectVector[2] = GameMode->BlueTile[nx][ny]->ObjectVector[prev_l];
		GameMode->BlueTile[nx][ny]->ObjectVector[prev_l] = nullptr;
		GameMode->currPlayer1 = DynamicClass->Next;
		GameMode->currPlayer1->level = 2;
	}
}

void APlayer1Tile::reaction(ABasicParent* DynamicClass)
{
	if (!DynamicClass) return;
	if (int(DynamicClass->Type) >= 4 && int(DynamicClass->Type) <= 8) {
		DynamicClass->Next->level = 1;
		int nx = DynamicClass->Next->x, ny = DynamicClass->Next->y;
		GameMode->BlueTile[nx][ny]->ObjectVector[1] = GameMode->BlueTile[nx][ny]->ObjectVector[2];
		GameMode->BlueTile[nx][ny]->ObjectVector[2] = nullptr;
		GameMode->currPlayer1 = nullptr;
	}
}
