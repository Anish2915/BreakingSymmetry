// Fill out your copyright notice in the Description page of Project Settings.


#include "Player2Tile.h"
#include "../BasicParent.h"
#include "A_ArenaGameMode.h"
#include "../A_Tile.h"

void APlayer2Tile::action(ABasicParent* DynamicClass)
{
	if (!DynamicClass) return;
	if (int(DynamicClass->Type) >= 4 && int(DynamicClass->Type) <= 8) {
		// GameMode->currPlayer2->level = 1;
		int prev_l = DynamicClass->Next->level;
		int nx = DynamicClass->Next->x, ny = DynamicClass->Next->y;
		GameMode->RedTile[nx][ny]->ObjectVector[2] = GameMode->RedTile[nx][ny]->ObjectVector[prev_l];
		GameMode->RedTile[nx][ny]->ObjectVector[prev_l] = nullptr;
		GameMode->currPlayer2 = DynamicClass->Next;
		GameMode->currPlayer2->level = 2;
	}
}

void APlayer2Tile::reaction(ABasicParent* DynamicClass)
{
	if (!DynamicClass) return;
	if (int(DynamicClass->Type) >= 4 && int(DynamicClass->Type) <= 8) {
		DynamicClass->Next->level = 1;
		int nx = DynamicClass->Next->x, ny = DynamicClass->Next->y;
		GameMode->RedTile[nx][ny]->ObjectVector[1] = GameMode->RedTile[nx][ny]->ObjectVector[2];
		GameMode->RedTile[nx][ny]->ObjectVector[2] = nullptr;
		GameMode->currPlayer2 = nullptr;
	}
}
