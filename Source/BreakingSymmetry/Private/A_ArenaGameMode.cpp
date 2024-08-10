// Fill out your copyright notice in the Description page of Project Settings.


#include "A_ArenaGameMode.h"
#include "../PlayerController/A_PCArena.h"
#include "Kismet/GameplayStatics.h"
#include "../Actors/A_Tile.h"
#include "../Actors/BasicParent.h"
#include "../Actors/StaticTile.h"
#include "../Interface/I_ParentStatic.h"

void AA_ArenaGameMode::BeginPlay()
{
	Super::BeginPlay();
	ArenPlayerController = Cast<AA_PCArena>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	SpawnActor();
	Func_Mapping.push_back({ 0,0,0,0 });
	Func_Mapping.push_back({ 0,0,0,0 });
	MovementDelegates.Add(MovementDelegate::CreateUObject(this, &AA_ArenaGameMode::do_nothing));
	MovementDelegates.Add(MovementDelegate::CreateUObject(this, &AA_ArenaGameMode::Up));
	MovementDelegates.Add(MovementDelegate::CreateUObject(this, &AA_ArenaGameMode::Down));
	MovementDelegates.Add(MovementDelegate::CreateUObject(this, &AA_ArenaGameMode::left));
	MovementDelegates.Add(MovementDelegate::CreateUObject(this, &AA_ArenaGameMode::right));
	//CreateDummyLevel();
}

void AA_ArenaGameMode::SpawnActor()
{
	FVector SpawnLocation = FVector(260.f, 140.f, 50.f);
	FRotator SpawnRotation = FRotator(-90.f, 0.f, 0.f);
	ArenPlayerController = ArenPlayerController ? ArenPlayerController : Cast<AA_PCArena>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (ArenPlayerController && BPActorClass) {
		for (int i = 0; i < 12; i++) {
			std::vector<AA_Tile*> temp;
			temp.clear();
			for (int j = 0; j < 12; j++) {
				AA_Tile* TileTemp = GetWorld()->SpawnActor<AA_Tile>(BPActorClass, SpawnLocation, SpawnRotation);
				temp.push_back(TileTemp);
				TileTemp->PlaneMesh->SetMaterial(0, StaticMaterialArray[0]);
				SpawnLocation.Z = SpawnLocation.Z + 20.f;
			}
			BlueTile.push_back(temp);
			SpawnLocation.Z = 50.f;
			SpawnLocation.Y = SpawnLocation.Y - 20.f;
		}
		SpawnLocation = FVector(260.f, -105.f, 50.f);
		for (int i = 0; i < 12; i++) {
			std::vector<AA_Tile*> temp;
			temp.clear();
			for (int j = 0; j < 12; j++) {
				AA_Tile* TileTemp = GetWorld()->SpawnActor<AA_Tile>(BPActorClass, SpawnLocation, SpawnRotation);
				temp.push_back(TileTemp);
				TileTemp->PlaneMesh->SetMaterial(0, StaticMaterialArray[17]);
				SpawnLocation.Z = SpawnLocation.Z + 20.f;
			}
			RedTile.push_back(temp);
			SpawnLocation.Z = 50.f;
			SpawnLocation.Y = SpawnLocation.Y - 20.f;
		}
	}
	//laser_material_blue(5, 5, 16);
}

void AA_ArenaGameMode::Up(class ABasicParent* currPlayer)
{
	update(currPlayer->x, currPlayer->y + 1, currPlayer->blue, currPlayer->x, currPlayer->y);
}

void AA_ArenaGameMode::Down(ABasicParent* currPlayer)
{
	update(currPlayer->x, currPlayer->y - 1, currPlayer->blue, currPlayer->x, currPlayer->y);
}

void AA_ArenaGameMode::left(ABasicParent* currPlayer)
{
	update(currPlayer->x - 1, currPlayer->y, currPlayer->blue, currPlayer->x, currPlayer->y);
}

void AA_ArenaGameMode::right(ABasicParent* currPlayer)
{
	update(currPlayer->x + 1, currPlayer->y, currPlayer->blue, currPlayer->x, currPlayer->y);
}

void AA_ArenaGameMode::do_nothing(ABasicParent* currPlayer)
{
	return;
}

UE_DISABLE_OPTIMIZATION
void AA_ArenaGameMode::update(int x, int y, bool Blue, int Din_x, int Din_y)
{
	if (Blue) {
		int i = 0;
		if (x - Din_x > 0) {
			// Checking how much far the objects can move
			
			while ((x + i < 12) && BlueTile[x + i][y]->ObjectVector[2]) i++;
			if ((x + i >= 12 && (BlueTile[x + i - 1][y]->ObjectVector[2] == currPlayer1 || !BlueTile[x + i - 1][y]->ObjectVector[2]->CanPass)) || (x+i < 12 && BlueTile[x + i][y]->ObjectVector[3] && BlueTile[x + i - 1][y]->ObjectVector[2]->Type != DynamicType::TNT)) return;
			if (x + i >= 12 && BlueTile[11][y]->ObjectVector[2]->CanPass) {
				while (x + i < 24 && (RedTile[x + i - 12][y]->ObjectVector[2])) { 
					i++; 
				}
				if (x + i >= 24 || (RedTile[x + i - 12][y]->ObjectVector[3] && (x + i == 12 ? BlueTile[11][y]->ObjectVector[2]->Type != DynamicType::TNT : RedTile[x + i - 12 - 1][y]->ObjectVector[2]->Type != DynamicType::TNT))) return;
			}

			// Updating for the blue tile
			ABasicParent* temp1 = BlueTile[Din_x][y]->ObjectVector[2], * temp2 = BlueTile[Din_x + 1][y]->ObjectVector[2];

			// Updating in BlueTile 2D vector
			int itr;
			for (itr = Din_x + 1; itr <= x + i && itr < 12; itr++) {
				// Checking for TNT
				if (temp1->Type == DynamicType::TNT && TNT_blast_blue(temp1, itr, y)) {
					temp1 = nullptr;
					BlueTile[Din_x][y]->ObjectVector[2] = nullptr;
					return;
				}

				// Updating the position and DynamicObject
				BlueTile[itr][y]->ObjectVector[2] = temp1;
				temp1->x = itr;
				temp1->y = y;

				// Updating the front_end location of the DynamicObject
				Update_Location_blue(temp1, itr, y);

				// Checking if the laser moved
				if (temp1->Type == DynamicType::Laser) laser_moved_blue(itr - 1, y, itr, y);

				// Checking whether comes in the path of laser
				if (BlueTile[itr][y]->bIsInLaser) check_IsInLaser_blue(itr, y, 2);

				// Removing the controls from the previous static tile
				check_reaction_blue(temp2);

				// Updating the temps
				temp1 = temp2;
				if (itr + 1 < 12) temp2 = BlueTile[itr + 1][y]->ObjectVector[2];
				else temp2 = RedTile[itr + 1 - 12][y]->ObjectVector[2];

				// Updating Controls
				check_static_blue(itr, y);
			}

			// Handling the pass
			if (x + i >= 12) {
				// Checking for TNT
				if (temp1->Type == DynamicType::TNT) {
					bool is_blasted = false;
					for (int j = 1; j < 4; j++) {
						if (RedTile[itr - 12][y]->ObjectVector[j]) {
							is_blasted = true;
							check_IsInLaser_red(itr - 12, y, j);
						}
					}

					if (is_blasted) {
						temp1->Destroy();
						temp1 = nullptr;
						BlueTile[Din_x][y]->ObjectVector[2] = nullptr;
						return;
					}
				}

				// Updating positions and temps and laser
				RedTile[itr - 12][y]->ObjectVector[2] = temp1;
				temp1->blue = false;
				temp1 = temp2;

				// Handling reaction
				check_reaction_blue(RedTile[itr - 12][y]->ObjectVector[2]);

				RedTile[itr - 12][y]->ObjectVector[2]->x = itr - 12;
				RedTile[itr - 12][y]->ObjectVector[2]->y = y;

				// Updating the front_end position of the object
				FVector Loc = RedTile[itr - 12][y]->GetActorLocation();
				Loc.X = Loc.X + (RedTile[itr - 12][y]->ObjectVector[2]->priority * 0.2);
				RedTile[itr - 12][y]->ObjectVector[2]->SetActorLocation(Loc);

				// Checking if the laser moved
				if (RedTile[itr - 12][y]->ObjectVector[2]->Type == DynamicType::Laser) laser_blue_to_red(y);

				// Updating the temps
				temp2 = RedTile[itr - 12 + 1][y]->ObjectVector[2];

				// Checking whether comes in the path of laser
				if (RedTile[itr - 12][y]->bIsInLaser) check_IsInLaser_red(itr - 12, y, 2);

				// Updating Controls
				check_static_red(itr - 12, y);

				// Updating in RedTile 2D vector
				for (itr = itr + 1; itr <= x + i; itr++) {
					// Checking for TNT
					if (temp1->Type == DynamicType::TNT && TNT_blast_red(temp1, itr - 12, y)) {
						temp1 = nullptr;
						BlueTile[Din_x][y]->ObjectVector[2] = nullptr;
						return;
					}

					// Updating the position and DynamicObject
					RedTile[itr - 12][y]->ObjectVector[2] = temp1;
					temp1->x = itr - 12;
					temp1->y = y;

					// Updating the front_end location of the DynamicObject
					Update_Location_red(temp1, itr - 12, y);

					// Checking if the laser moved
					if (temp1->Type == DynamicType::Laser) laser_moved_red(itr - 1 - 12, y, itr - 12, y);

					// Checking whether comes in the path of laser
					if (RedTile[itr - 12][y]->bIsInLaser) check_IsInLaser_red(itr - 12, y, 2);

					// Removing the controls from the previous static tile
					check_reaction_red(temp2);

					// Updating the temps
					temp1 = temp2;
					if (itr + 1 < 24) temp2 = RedTile[itr + 1 - 12][y]->ObjectVector[2];

					// Updating Controls
					check_static_red(itr - 12, y);
				}
			}

			// Removing the first object from original position
			BlueTile[Din_x][y]->ObjectVector[2] = nullptr;

			// Checking Win
			for (itr = Din_x + 1; itr <= x + i && itr < 12; itr++) {
				win_blue(itr - 1, y, itr, y);
				defeat_blue(itr - 1, y, itr, y);
			}

			// Counting Movement and spawn sound
			MovementSuccessfull();
		}

		else if (x - Din_x < 0) {
			// Checking how much far the objects can move
			while ((x + i >= 0) && BlueTile[x + i][y]->ObjectVector[2]) i--;
			if (x + i < 0 || (BlueTile[x + i][y]->ObjectVector[3] && BlueTile[x + i + 1][y]->ObjectVector[2]->Type != DynamicType::TNT)) return;

			// Updating the positions
			ABasicParent* temp1 = BlueTile[Din_x][y]->ObjectVector[2], * temp2 = BlueTile[Din_x - 1][y]->ObjectVector[2];
			for (int itr = Din_x - 1; itr >= x + i; itr--) {
				// Checking for TNT
				if (temp1->Type == DynamicType::TNT && TNT_blast_blue(temp1, itr, y)) {
					temp1 = nullptr;
					BlueTile[Din_x][y]->ObjectVector[2] = nullptr;
					return;
				}

				// Updating position and DynamicObeject
				BlueTile[itr][y]->ObjectVector[2] = temp1;
				temp1->x = itr;
				temp1->y = y;

				// Updating the front_end location of the DynamicObject
				Update_Location_blue(temp1, itr, y);

				// Checking if the laser moved
				if (temp1->Type == DynamicType::Laser) laser_moved_blue(itr + 1, y, itr, y);

				// Checking if something comes in the path of laser
				if (BlueTile[itr][y]->bIsInLaser) check_IsInLaser_blue(itr, y, 2);

				// Removing the controls from the previous static tile
				check_reaction_blue(temp2);

				// Updating the temps
				temp1 = temp2;
				if (itr - 1 >= 0) temp2 = BlueTile[itr - 1][y]->ObjectVector[2];

				// Updating Controls
				check_static_blue(itr, y);
			}
			BlueTile[Din_x][y]->ObjectVector[2] = nullptr;

			// Checking Win
			for (int itr = Din_x - 1; itr >= x + i; itr--) {
				win_blue(itr + 1, y, itr, y);
				defeat_blue(itr + 1, y, itr, y);
			}

			// Counting Movement and spawn sound
			MovementSuccessfull();
		}

		else if (y - Din_y > 0) {
			// Checking how much far the objects can move
			while ((y + i < 12) && BlueTile[x][y + i]->ObjectVector[2]) i++;
			if (y + i >= 12 || (BlueTile[x][y + i]->ObjectVector[3] && BlueTile[x][y + i - 1]->ObjectVector[2]->Type != DynamicType::TNT)) return;

			// Updating the positions
			ABasicParent* temp1 = BlueTile[x][Din_y]->ObjectVector[2], * temp2 = BlueTile[x][Din_y + 1]->ObjectVector[2];
			for (int itr = Din_y + 1; itr <= y + i; itr++) {
				// Checking for TNT
				if (temp1->Type == DynamicType::TNT && TNT_blast_blue(temp1, x, itr)) {
					temp1 = nullptr;
					BlueTile[x][Din_y]->ObjectVector[2] = nullptr;
					return;
				}

				// Updating position and DynamicObject
				BlueTile[x][itr]->ObjectVector[2] = temp1;
				temp1->x = x;
				temp1->y = itr;

				// Updating the front_end loacation of the DynamicObject
				Update_Location_blue(temp1, x, itr);

				// Checking if the laser moved
				if (temp1->Type == DynamicType::Laser) laser_moved_blue(x, itr - 1, x, itr);

				// Checking whether comes in the path of laser
				if (BlueTile[x][itr]->bIsInLaser) check_IsInLaser_blue(x, itr, 2);

				// Removing the controls from the previous static tile
				check_reaction_blue(temp2);

				// Updating the temps
				temp1 = temp2;
				if (itr + 1 < 12) temp2 = BlueTile[x][itr + 1]->ObjectVector[2];

				// Updating Controls
				check_static_blue(x, itr);
			}

			// Removing the first object from original position
			BlueTile[x][Din_y]->ObjectVector[2] = nullptr;

			// Checking Win
			for (int itr = Din_y + 1; itr <= y + i; itr++) {
				win_blue(x, itr - 1, x, itr);
				defeat_blue(x, itr - 1, x, itr);
			}

			// Counting Movement and spawn sound
			MovementSuccessfull();
		}

		else if (y - Din_y < 0) {
			// Checking how much far the objects can move
			while ((y + i >= 0) && BlueTile[x][y + i]->ObjectVector[2]) i--;
			if (y + i < 0 || (BlueTile[x][y + i]->ObjectVector[3] && BlueTile[x][y + i + 1]->ObjectVector[2]->Type != DynamicType::TNT)) return;

			// Updating the positions
			ABasicParent* temp1 = BlueTile[x][Din_y]->ObjectVector[2], * temp2 = BlueTile[x][Din_y - 1]->ObjectVector[2];
			for (int itr = Din_y - 1; itr >= y + i; itr--) {
				// Checking for TNT
				if (temp1->Type == DynamicType::TNT && TNT_blast_blue(temp1, x, itr)) {
					temp1 = nullptr;
					BlueTile[x][Din_y]->ObjectVector[2] = nullptr;
					return;
				}

				// Updating position and DynamicObject
				BlueTile[x][itr]->ObjectVector[2] = temp1;
				temp1->x = x;
				temp1->y = itr;

				// Updating the front_end loacation of the DynamicObject
				Update_Location_blue(temp1, x, itr);

				// Checking if the laser moved
				if (temp1->Type == DynamicType::Laser) laser_moved_blue(x, itr + 1, x, itr);

				// Checking if something comes in the path of laser
				if (BlueTile[x][itr]->bIsInLaser) check_IsInLaser_blue(x, itr, 2);

				// Removing the controls from the previous static tile
				check_reaction_blue(temp2);

				// Updating the temps
				temp1 = temp2;
				if (itr - 1 >= 0) temp2 = BlueTile[x][itr - 1]->ObjectVector[2];

				// Updating Controls
				check_static_blue(x, itr);
			}
			BlueTile[x][Din_y]->ObjectVector[2] = nullptr;

			// Checking Win
			for (int itr = Din_y - 1; itr >= y + i; itr--) {
				win_blue(x, itr + 1, x, itr);
				defeat_blue(x, itr + 1, x, itr);
			}

			// Counting Movement and spawn sound
			MovementSuccessfull();
		}
	}
	else {
		int i = 0;
		if (x - Din_x > 0) {
			// Checking how much far the objects can move
			while ((x + i < 12) && RedTile[x + i][y]->ObjectVector[2])  i++;
			if (x + i >= 12 || (RedTile[x + i][y]->ObjectVector[3] && RedTile[x + i - 1][y]->ObjectVector[2]->Type != DynamicType::TNT)) return;

			// Updating the positions
			ABasicParent* temp1 = RedTile[Din_x][y]->ObjectVector[2], * temp2 = RedTile[Din_x + 1][y]->ObjectVector[2];
			for (int itr = Din_x + 1; itr <= x + i; itr++) {
				// Checking for TNT
				if (temp1->Type == DynamicType::TNT && TNT_blast_red(temp1, itr, y)) {
					temp1 = nullptr;
					RedTile[Din_x][y]->ObjectVector[2] = nullptr;
					return;
				}

				// Updating position and DynamicObject
				RedTile[itr][y]->ObjectVector[2] = temp1;
				temp1->x = itr;
				temp1->y = y;

				// Updating the front_end loacation of the DynamicObject
				Update_Location_red(temp1, itr, y);

				// Checking if the laser moved
				if (temp1->Type == DynamicType::Laser) laser_moved_red(itr - 1, y, itr, y);

				// Checking whether comes in the path of laser
				if (RedTile[itr][y]->bIsInLaser) check_IsInLaser_red(itr, y, 2);

				// Removing the controls from the previous static tile
				check_reaction_red(temp2);

				// Updating the temps
				temp1 = temp2;
				if (itr + 1 < 12) temp2 = RedTile[itr + 1][y]->ObjectVector[2];

				// Updating Controls
				check_static_red(itr, y);
			}
			RedTile[Din_x][y]->ObjectVector[2] = nullptr;

			// Checking Win
			for (int itr = Din_x + 1; itr <= x + i; itr++) {
				win_red(itr - 1, y, itr, y);
				defeat_red(itr - 1, y, itr, y);
			}

			// Counting Movement and spawn sound
			MovementSuccessfull();
		}

		else if (x - Din_x < 0) {
			// Checking how much far the objects can move
			while ((x + i >= 0) && RedTile[x + i][y]->ObjectVector[2]) i--;
			if ((x + i < 0 && (RedTile[x + i + 1][y]->ObjectVector[2] == currPlayer2 || !RedTile[x + i + 1][y]->ObjectVector[2]->CanPass)) || (x+i>=0 && RedTile[x + i][y]->ObjectVector[3] && RedTile[x + i + 1][y]->ObjectVector[2]->Type != DynamicType::TNT)) return;
			if (x + i < 0 && RedTile[0][y]->ObjectVector[2]->CanPass) {
				while (x + i >= -12 && BlueTile[x + i + 12][y]->ObjectVector[2]) i--;
				if (x + i < -12 || (BlueTile[x + i + 12][y]->ObjectVector[3] && (x + i == -1 ? RedTile[0][y]->ObjectVector[2]->Type != DynamicType::TNT : BlueTile[x + i + 12 + 1][y]->ObjectVector[2]->Type != DynamicType::TNT))) return;
			}

			// Updating for RedTile as well as for BlueTile
			ABasicParent* temp1 = RedTile[Din_x][y]->ObjectVector[2], * temp2 = RedTile[Din_x - 1][y]->ObjectVector[2];

			// Updating in RedTile 2D vector
			int itr;
			for (itr = Din_x - 1; itr >= x + i && itr >= 0; itr--) {
				// Checking for TNT
				if (temp1->Type == DynamicType::TNT && TNT_blast_red(temp1, itr, y)) {
					temp1 = nullptr;
					RedTile[Din_x][y]->ObjectVector[2] = nullptr;
					return;
				}

				// Updating position and DynamicObject
				RedTile[itr][y]->ObjectVector[2] = temp1;
				temp1->x = itr;
				temp1->y = y;

				// Updating the front_end loacation of the DynamicObject
				Update_Location_red(temp1, itr, y);

				// Checking if the laser moved
				if (temp1->Type == DynamicType::Laser) laser_moved_red(itr + 1, y, itr, y);

				// Checking whether comes in the path of the laser
				if (RedTile[itr][y]->bIsInLaser) check_IsInLaser_red(itr, y, 2);

				// Removing the controls from the previous static tile
				check_reaction_red(temp2);

				// Updating the temps
				temp1 = temp2;
				if (itr - 1 >= 0) temp2 = RedTile[itr - 1][y]->ObjectVector[2];
				else temp2 = BlueTile[itr - 1 + 12][y]->ObjectVector[2];

				// Updating Controls
				check_static_red(itr, y);
			}

			// Handling the base case
			if (x + i <= -1) {
				// Checking for TNT
				if (temp1->Type == DynamicType::TNT) {
					bool is_blasted = false;
					for (int j = 1; j < 4; j++) {
						is_blasted = true;
						check_IsInLaser_blue(itr + 12, y, j);
					}
					if (is_blasted) {
						temp1->Destroy();
						temp1 = nullptr;
						RedTile[Din_x][y]->ObjectVector[2] = nullptr;
						return;
					}
				}

				// Updating positions and temps and laser
				BlueTile[itr + 12][y]->ObjectVector[2] = temp1;
				temp1->blue = true;
				temp1 = temp2;

				// Removing the controls from the previous static tile
				check_reaction_red(BlueTile[itr + 12][y]->ObjectVector[2]);

				BlueTile[itr + 12][y]->ObjectVector[2]->x = itr + 12;
				BlueTile[itr + 12][y]->ObjectVector[2]->y = y;

				// Updating the front_end position of the object
				FVector Loc = BlueTile[itr + 12][y]->GetActorLocation();
				Loc.X = Loc.X + (BlueTile[itr + 12][y]->ObjectVector[2]->priority * 0.2);
				BlueTile[itr + 12][y]->ObjectVector[2]->SetActorLocation(Loc);

				// Checking if the laser moved
				if (BlueTile[itr + 12][y]->ObjectVector[2]->Type == DynamicType::Laser) laser_red_to_blue(y);

				// Updating the temps
				temp2 = BlueTile[itr + 12 - 1][y]->ObjectVector[2];

				// Checking whether comes in the path of laser
				if (BlueTile[itr + 12][y]->bIsInLaser) check_IsInLaser_blue(itr + 12, y, 2);

				// Updating Controls
				check_static_blue(itr + 12, y);

				// Updating in BlueTile 2D vector
				for (itr = itr - 1; itr >= x + i; itr--) {
					// Checking for TNT
					if (temp1->Type == DynamicType::TNT && TNT_blast_blue(temp1, itr + 12, y)) {
						temp1 = nullptr;
						RedTile[Din_x][y]->ObjectVector[2] = nullptr;
						return;
					}

					// Updating position and DynamicObject
					BlueTile[itr + 12][y]->ObjectVector[2] = temp1;
					temp1->x = itr + 12;
					temp1->y = y;

					// Updating the front_end loacation of the DynamicObject
					Update_Location_blue(temp1, itr + 12, y);

					// Checking if the laser moved
					if (temp1->Type == DynamicType::Laser) laser_moved_blue(itr + 1 + 12, y, itr + 12, y);

					// Checking whether comes in the path of laser
					if (BlueTile[itr + 12][y]->bIsInLaser) check_IsInLaser_blue(itr + 12, y, 2);

					// Removing the controls from the previous static tile
					check_reaction_blue(temp2);

					// Updating the temps
					temp1 = temp2;
					if (itr - 1 >= -12) temp2 = BlueTile[itr - 1 + 12][y]->ObjectVector[2];

					// Updating Controls
					check_static_blue(itr + 12, y);
				}
			}

			// Removing the first object from original position
			RedTile[Din_x][y]->ObjectVector[2] = nullptr;

			// Checking Win
			for (itr = Din_x - 1; itr >= x + i && itr >= 0; itr--) {
				win_red(itr + 1, y, itr, y);
				defeat_red(itr + 1, y, itr, y);
			}

			// Counting Movement and spawn sound
			MovementSuccessfull();
		}

		else if (y - Din_y > 0) {
			// Checking how much far the objects can move
			while ((y + i < 12) && RedTile[x][y + i]->ObjectVector[2]) i++;
			if (y + i >= 12 || (RedTile[x][y + i]->ObjectVector[3] && RedTile[x][y + i - 1]->ObjectVector[2]->Type != DynamicType::TNT)) return;

			// Updating the positions
			ABasicParent* temp1 = RedTile[x][Din_y]->ObjectVector[2], * temp2 = RedTile[x][Din_y + 1]->ObjectVector[2];
			for (int itr = Din_y + 1; itr <= y + i; itr++) {
				// Checking for TNT
				if (temp1->Type == DynamicType::TNT && TNT_blast_red(temp1, x, itr)) {
					temp1 = nullptr;
					RedTile[x][Din_y]->ObjectVector[2] = nullptr;
					return;
				}

				// Updating position and DynamicObject
				RedTile[x][itr]->ObjectVector[2] = temp1;
				temp1->x = x;
				temp1->y = itr;

				// Updating the front_end loacation of the DynamicObject
				Update_Location_red(temp1, x, itr);

				// Checking if the laser moved
				if (temp1->Type == DynamicType::Laser) laser_moved_red(x, itr - 1, x, itr);

				// Checking whether comes in the path of laser
				if (RedTile[x][itr]->bIsInLaser) check_IsInLaser_red(x, itr, 2);

				// Removing the controls from the previous static tile
				check_reaction_red(temp2);

				// Updating the temps
				temp1 = temp2;
				if (itr + 1 < 12) temp2 = RedTile[x][itr + 1]->ObjectVector[2];

				// Updating Controls
				check_static_red(x, itr);
			}
			RedTile[x][Din_y]->ObjectVector[2] = nullptr;

			// Checking Win
			for (int itr = Din_y + 1; itr <= y + i; itr++) {
				win_red(x, itr - 1, x, itr);
				defeat_red(x, itr - 1, x, itr);
			}

			// Counting Movement and spawn sound
			MovementSuccessfull();
		}

		else if (y - Din_y < 0) {
			// Checking how much far the objects can move
			while ((y + i >= 0) && RedTile[x][y + i]->ObjectVector[2]) i--;
			if (y + i < 0 || (RedTile[x][y + i]->ObjectVector[3] && RedTile[x][y + i + 1]->ObjectVector[2]->Type != DynamicType::TNT)) return;

			// Updating the positions
			ABasicParent* temp1 = RedTile[x][Din_y]->ObjectVector[2], * temp2 = RedTile[x][Din_y - 1]->ObjectVector[2];
			for (int itr = Din_y - 1; itr >= y + i; itr--) {
				// Checking for TNT
				if (temp1->Type == DynamicType::TNT && TNT_blast_red(temp1, x, itr)) {
					temp1 = nullptr;
					RedTile[x][Din_y]->ObjectVector[2] = nullptr;
					return;
				}

				// Updating position and DynamicObject
				RedTile[x][itr]->ObjectVector[2] = temp1;
				temp1->x = x;
				temp1->y = itr;

				// Updating the front_end loacation of the DynamicObject
				Update_Location_red(temp1, x, itr);

				// Checking if the laser moved
				if (temp1->Type == DynamicType::Laser) laser_moved_red(x, itr + 1, x, itr);

				// Checking whether comes in the path of laser
				if (RedTile[x][itr]->bIsInLaser) check_IsInLaser_red(x, itr, 2);

				// Removing the controls from the previous static tile
				check_reaction_red(temp2);

				// Updating the temps
				temp1 = temp2;
				if (itr - 1 >= 0) temp2 = RedTile[x][itr - 1]->ObjectVector[2];

				// Updating Controls
				check_static_red(x, itr);
			}
			RedTile[x][Din_y]->ObjectVector[2] = nullptr;

			// Checking Win
			for (int itr = Din_y - 1; itr >= y + i; itr--) {
				win_red(x, itr + 1, x, itr);
				defeat_red(x, itr + 1, x, itr);
			}

			// Counting Movement and spawn sound
			MovementSuccessfull();
		}
	}
}
UE_ENABLE_OPTIMIZATION

void AA_ArenaGameMode::check_static_red(int x, int y)
{
	if (RedTile[x][y]->StaticTile == nullptr) return;

	II_ParentStatic* Inter = Cast<II_ParentStatic>(RedTile[x][y]->StaticTile);
	if (Inter) {
		Inter->action(RedTile[x][y]->ObjectVector[2]);
	}
}

void AA_ArenaGameMode::check_static_blue(int x, int y)
{
	if (BlueTile[x][y]->StaticTile == nullptr) return;

	II_ParentStatic* Inter = Cast<II_ParentStatic>(BlueTile[x][y]->StaticTile);
	if (Inter) {
		Inter->action(BlueTile[x][y]->ObjectVector[2]);
	}
}

void AA_ArenaGameMode::check_reaction_red(ABasicParent* DynamicObject)
{
	if (!DynamicObject) return;
	int x = DynamicObject->x, y = DynamicObject->y;
	if (RedTile[x][y]->StaticTile == nullptr) return;

	II_ParentStatic* Inter = Cast<II_ParentStatic>(RedTile[x][y]->StaticTile);
	if (Inter) {
		Inter->reaction(DynamicObject);
	};
}

void AA_ArenaGameMode::check_reaction_blue(ABasicParent* DynamicObject)
{
	if (!DynamicObject) return;
	int x = DynamicObject->x, y = DynamicObject->y;
	if (BlueTile[x][y]->StaticTile == nullptr) return;

	II_ParentStatic* Inter = Cast<II_ParentStatic>(BlueTile[x][y]->StaticTile);
	if (Inter) {
		Inter->reaction(DynamicObject);
	}
}

void AA_ArenaGameMode::check_IsInLaser_red(int x, int y, int ObjectLevel)
{
	if (!RedTile[x][y]->ObjectVector[ObjectLevel]) return;

	if (RedTile[x][y]->ObjectVector[ObjectLevel] == currPlayer2) {
		Loose();
		return;
		//loose condition
	}

	if (RedTile[x][y]->ObjectVector[ObjectLevel]->Prev) {
		RedTile[x][y]->ObjectVector[ObjectLevel]->Prev->Next = RedTile[x][y]->ObjectVector[ObjectLevel]->Next;
	}
	if (RedTile[x][y]->ObjectVector[ObjectLevel]->Next) {
		RedTile[x][y]->ObjectVector[ObjectLevel]->Next->Prev = RedTile[x][y]->ObjectVector[ObjectLevel]->Prev;
	}
	check_reaction_red(RedTile[x][y]->ObjectVector[ObjectLevel]);
	if (RedTile[x][y]->ObjectVector[ObjectLevel]->Type == DynamicType::Laser) remove_laser_light_red(RedTile[x][y]->ObjectVector[ObjectLevel]);
	RedTile[x][y]->ObjectVector[ObjectLevel]->Destroy();
	RedTile[x][y]->ObjectVector[ObjectLevel] = nullptr;
	UGameplayStatics::PlaySound2D(GetWorld(), LaserBlastSound);
}

void AA_ArenaGameMode::check_IsInLaser_blue(int x, int y, int ObjectLevel)
{
	if (!BlueTile[x][y]->ObjectVector[ObjectLevel]) return;

	if (BlueTile[x][y]->ObjectVector[ObjectLevel] == currPlayer1) {
		Loose();
		//loose condition
	}
	if (BlueTile[x][y]->ObjectVector[ObjectLevel]->Prev) {
		BlueTile[x][y]->ObjectVector[ObjectLevel]->Prev->Next = BlueTile[x][y]->ObjectVector[ObjectLevel]->Next;
	}
	if (BlueTile[x][y]->ObjectVector[ObjectLevel]->Next) {
		BlueTile[x][y]->ObjectVector[ObjectLevel]->Next->Prev = BlueTile[x][y]->ObjectVector[ObjectLevel]->Prev;
	}
	check_reaction_blue(BlueTile[x][y]->ObjectVector[ObjectLevel]);
	if (BlueTile[x][y]->ObjectVector[ObjectLevel]->Type == DynamicType::Laser) remove_laser_light_blue(BlueTile[x][y]->ObjectVector[ObjectLevel]);
	BlueTile[x][y]->ObjectVector[ObjectLevel]->Destroy();
	BlueTile[x][y]->ObjectVector[ObjectLevel] = nullptr;
	UGameplayStatics::PlaySound2D(GetWorld(), LaserBlastSound);
}

void AA_ArenaGameMode::laser_moved_red(int prev_x, int prev_y, int x, int y)
{
	// if (int(RedTile[x][y]->ObjectVector[2]->Type) != 12) return;

	ABasicParent* Laser = RedTile[x][y]->ObjectVector[2];
	if (Laser->LaserDirection == 1 && prev_x != x) {
		for (int i = prev_y + 1; i < 12; i++) {
			// Updating whether there is laser or not
			RedTile[prev_x][i]->bIsInLaser = false;
			RedTile[x][i]->bIsInLaser = true;
			laser_material_red(prev_x, i, 17);
			laser_material_red(x, i, 16);

			// Removing the obstacles in the way of the laser for 2
			check_IsInLaser_red(x, i, 2);

			// Removing the obstacles in the way of the laser for 3
			check_IsInLaser_red(x, i, 3);
		}
	}
	else if (Laser->LaserDirection == 2 && prev_x != x) {
		for (int i = prev_y - 1; i >= 0; i--) {
			// Updating whether there is laser or not
			RedTile[prev_x][i]->bIsInLaser = false;
			RedTile[x][i]->bIsInLaser = true;
			laser_material_red(prev_x, i, 17);
			laser_material_red(x, i, 16);

			// Removing the obstacles in the way of the laser for 2
			check_IsInLaser_red(x, i, 2);

			// Removing the obstacles in the way of the laser for 3
			check_IsInLaser_red(x, i, 3);
		}
	}
	else if (Laser->LaserDirection == 3 && prev_y != y) {
		for (int i = prev_x - 1; i >= 0; i--) {
			// Updating whether there is laser or not
			RedTile[i][prev_y]->bIsInLaser = false;
			RedTile[i][y]->bIsInLaser = true;
			laser_material_red(i, prev_y, 17);
			laser_material_red(i, y, 16);

			// Removing the obstacles in the way of the laser for 2
			check_IsInLaser_red(i, y, 2);

			// Removing the obstacles in the way of the laser for 3
			check_IsInLaser_red(i, y, 3);
		}
	}
	else if (Laser->LaserDirection == 4 && prev_y != y) {
		for (int i = prev_x + 1; i < 12; i++) {
			// Updating whether there is laser or not
			RedTile[i][prev_y]->bIsInLaser = false;
			RedTile[i][y]->bIsInLaser = true;
			laser_material_red(i, prev_y, 17);
			laser_material_red(i, y, 16);

			// Removing the obstacles in the way of the laser for 2
			check_IsInLaser_red(i, y, 2);

			// Removing the obstacles in the way of the laser for 3
			check_IsInLaser_red(i, y, 3);
		}
	}

	// Handling the base case for movement of laser in the direction of attack
	else {
		RedTile[x][y]->bIsInLaser = false;
		if (x - prev_x >= 0 && y - prev_y >= 0) {
			if (x + (x - prev_x) < 12 && x + (x - prev_x) >= 0 && y + (y - prev_y) < 12 && y + (y - prev_y) >= 0) {
				RedTile[x - (x - prev_x)][y - (y - prev_y)]->bIsInLaser = true;
				laser_material_red(x - (x - prev_x), y - (y - prev_y), 16);
			}
		}
		else {
			if (x >= 0 && x < 12 && y >= 0 && y < 12) {
				RedTile[prev_x][prev_y]->bIsInLaser = false;
				laser_material_red(prev_x, prev_y, 0);
			}
		}
		laser_material_red(x, y, 17);
	}
}

void AA_ArenaGameMode::laser_moved_blue(int prev_x, int prev_y, int x, int y)
{
	// if (int(BlueTile[x][y]->ObjectVector[2]->Type) != 12) return;

	ABasicParent* Laser = BlueTile[x][y]->ObjectVector[2];
	if (Laser->LaserDirection == 1 && prev_x != x) {
		for (int i = prev_y + 1; i < 12; i++) {
			// Updating whether there is laser or not
			BlueTile[prev_x][i]->bIsInLaser = false;
			BlueTile[x][i]->bIsInLaser = true;
			laser_material_blue(prev_x, i, 0);
			laser_material_blue(x, i, 16);

			// Removing the obstacles in the way of the laser for 2
			check_IsInLaser_blue(x, i, 2);

			// Removing the obstacles in the way of the laser for 3
			check_IsInLaser_blue(x, i, 3);
		}
	}
	else if (Laser->LaserDirection == 2 && prev_x != x) {
		for (int i = prev_y - 1; i >= 0; i--) {
			// Updating whether there is laser or not
			BlueTile[prev_x][i]->bIsInLaser = false;
			BlueTile[x][i]->bIsInLaser = true;
			laser_material_blue(prev_x, i, 0);
			laser_material_blue(x, i, 16);

			// Removing the obstacles in the way of the laser for 2
			check_IsInLaser_blue(x, i, 2);

			// Removing the obstacles in the way of the laser for 3
			check_IsInLaser_blue(x, i, 3);
		}
	}
	else if (Laser->LaserDirection == 3 && prev_y != y) {
		for (int i = prev_x - 1; i >= 0; i--) {
			// Updating whether there is laser or not
			BlueTile[i][prev_y]->bIsInLaser = false;
			BlueTile[i][y]->bIsInLaser = true;
			laser_material_blue(i, prev_y, 0);
			laser_material_blue(i, y, 16);

			// Removing the obstacles in the way of the laser for 2
			check_IsInLaser_blue(i, y, 2);

			// Removing the obstacles in the way of the laser for 3
			check_IsInLaser_blue(i, y, 3);
		}
	}
	else if (Laser->LaserDirection == 4 && prev_y != y) {
		for (int i = prev_x + 1; i < 12; i++) {
			// Updating whether there is laser or not
			BlueTile[i][prev_y]->bIsInLaser = false;
			BlueTile[i][y]->bIsInLaser = true;
			laser_material_blue(i, prev_y, 0);
			laser_material_blue(i, y, 16);

			// Removing the obstacles in the way of the laser for 2
			check_IsInLaser_blue(i, y, 2);

			// Removing the obstacles in the way of the laser for 3
			check_IsInLaser_blue(i, y, 3);
		}
	}

	// Handling the base case for movement of laser in the direction of attack
	else {
		BlueTile[x][y]->bIsInLaser = false;
		if (x - prev_x >= 0 && y - prev_y >= 0) {
			if (x + (x - prev_x) < 12 && x + (x - prev_x) >= 0 && y + (y - prev_y) < 12 && y + (y - prev_y) >= 0) {
				BlueTile[x - (x - prev_x)][y - (y - prev_y)]->bIsInLaser = true;
				laser_material_blue(x - (x - prev_x), y - (y - prev_y), 16);
			}
		}
		else {
			if (x >= 0 && x < 12 && y >= 0 && y < 12) {
				BlueTile[prev_x][prev_y]->bIsInLaser = false;
				laser_material_blue(prev_x, prev_y, 0);
			}
		}
		laser_material_blue(x, y, 0);
	}
}

bool AA_ArenaGameMode::TNT_blast_red(ABasicParent* TNT_blaster, int x, int y)
{
	bool is_blasted = false;
	for (int i = 1; i < 4; i++) {
		if (RedTile[x][y]->ObjectVector[i]) {
			is_blasted = true;
			check_IsInLaser_red(x, y, i);
		}
	}
	if (is_blasted) {
		TNT_blaster->Destroy();
		TNT_blaster = nullptr;
		UGameplayStatics::PlaySound2D(GetWorld(), TNTBlastSound);
	}

	return is_blasted;
}

bool AA_ArenaGameMode::TNT_blast_blue(ABasicParent* TNT_blaster, int x, int y)
{
	bool is_blasted = false;
	for (int i = 1; i < 4; i++) {
		if (BlueTile[x][y]->ObjectVector[i]) {
			is_blasted = true;
			check_IsInLaser_blue(x, y, i);
		}
	}
	if (is_blasted) {
		TNT_blaster->Destroy();
		TNT_blaster = nullptr;
		UGameplayStatics::PlaySound2D(GetWorld(), TNTBlastSound);
	}

	return is_blasted;
}

void AA_ArenaGameMode::win_blue(int prev_x, int prev_y, int x, int y)
{
	if (BlueTile[x][y]->ObjectVector[2] == currPlayer1) {
		for (int i = 1; i < 4; i++) {
			if (BlueTile[x][y]->ObjectVector[i] && BlueTile[x][y]->ObjectVector[i]->IsWin) {
				Win();
				return;
			}
		}
	}
}

void AA_ArenaGameMode::win_red(int prev_x, int prev_y, int x, int y)
{
	if (RedTile[x][y]->ObjectVector[2] == currPlayer2) {
		for (int i = 1; i < 4; i++) {
			if (RedTile[x][y]->ObjectVector[i] && RedTile[x][y]->ObjectVector[i]->IsWin) {
				Win();
				return;
			}
		}
	}
}

void AA_ArenaGameMode::defeat_blue(int prev_x, int prev_y, int x, int y)
{
	if (BlueTile[x][y]->ObjectVector[2] == currPlayer1) {
		for (int i = 1; i < 4; i++) {
			if (BlueTile[x][y]->ObjectVector[i] && BlueTile[x][y]->ObjectVector[i]->IsDefeat) {
				Loose();
				return;
			}
		}
	}
}

void AA_ArenaGameMode::defeat_red(int prev_x, int prev_y, int x, int y)
{
	if (RedTile[x][y]->ObjectVector[2] == currPlayer2) {
		for (int i = 1; i < 4; i++) {
			if (RedTile[x][y]->ObjectVector[i] && RedTile[x][y]->ObjectVector[i]->IsDefeat) {
				Loose();
				return;
			}
		}
	}
}

void AA_ArenaGameMode::laser_blue_to_red(int y)
{
	ABasicParent* Laser = RedTile[0][y]->ObjectVector[2];
	if (Laser->LaserDirection == 1) {
		for (int i = y + 1; i < 12; i++) {
			// Updating whether there is laser or not
			BlueTile[11][i]->bIsInLaser = false;
			RedTile[0][i]->bIsInLaser = true;
			laser_material_blue(11, i, 0);
			laser_material_red(0, i, 16);

			// Removing the obstacles in the way of the laser for 2
			check_IsInLaser_red(0, i, 2);

			// Removing the obstacles in the way of the laser for 3
			check_IsInLaser_red(0, i, 3);
		}
	}
	else if (Laser->LaserDirection == 2) {
		for (int i = y - 1; i >= 0; i--) {
			// Updating whether there is laser or not
			BlueTile[11][i]->bIsInLaser = false;
			RedTile[0][i]->bIsInLaser = true;
			laser_material_blue(11, i, 0);
			laser_material_red(0, i, 16);

			// Removing the obstacles in the way of the laser for 2
			check_IsInLaser_red(0, i, 2);

			// Removing the obstacles in the way of the laser for 3
			check_IsInLaser_red(0, i, 3);
		}
	}
	else if (Laser->LaserDirection == 3) {
		for (int i = 0; i < 12; i++) {
			BlueTile[i][y]->bIsInLaser = false;
			laser_material_blue(i, y, 0);
		}
	}
	else if (Laser->LaserDirection == 4) {
		for (int i = 1; i < 12; i++) {
			RedTile[i][y]->bIsInLaser = true;
			laser_material_red(i, y, 16);
			check_IsInLaser_red(i, y, 2);
			check_IsInLaser_red(i, y, 3);
		}
	}
}

void AA_ArenaGameMode::laser_red_to_blue(int y)
{
	ABasicParent* Laser = BlueTile[11][y]->ObjectVector[2];
	if (Laser->LaserDirection == 1) {
		for (int i = y + 1; i < 12; i++) {
			// Updating whether there is laser or not
			RedTile[0][i]->bIsInLaser = false;
			BlueTile[11][i]->bIsInLaser = true;
			laser_material_red(0, i, 17);
			laser_material_blue(11, i, 16);

			// Removing the obstacles in the way of the laser for 2
			check_IsInLaser_blue(11, i, 2);

			// Removing the obstacles in the way of the laser for 3
			check_IsInLaser_blue(11, i, 3);
		}
	}
	else if (Laser->LaserDirection == 2) {
		for (int i = y - 1; i >= 0; i--) {
			// Updating whether there is laser or not
			RedTile[0][i]->bIsInLaser = false;
			BlueTile[11][i]->bIsInLaser = true;
			laser_material_red(0, i, 17);
			laser_material_blue(11, i, 16);

			// Removing the obstacles in the way of the laser for 2
			check_IsInLaser_blue(11, i, 2);

			// Removing the obstacles in the way of the laser for 3
			check_IsInLaser_blue(11, i, 3);
		}
	}
	else if (Laser->LaserDirection == 3) {
		for (int i = 10; i >= 0; i--) {
			BlueTile[i][y]->bIsInLaser = true;
			laser_material_blue(i, y, 16);
			check_IsInLaser_blue(i, y, 2);
			check_IsInLaser_blue(i, y, 3);
		}
	}
	else if (Laser->LaserDirection == 4) {
		for (int i = 0; i < 12; i++) {
			RedTile[i][y]->bIsInLaser = false;
			laser_material_red(i, y, 17);
		}
	}
}

void AA_ArenaGameMode::Update_Location_blue(ABasicParent* object, int x, int y)
{
	FVector Loc = BlueTile[x][y]->GetActorLocation();
	Loc.X = Loc.X + object->priority * 0.1;
	object->SetActorLocation(Loc);
}

void AA_ArenaGameMode::Update_Location_red(ABasicParent* object, int x, int y)
{
	FVector Loc = RedTile[x][y]->GetActorLocation();
	Loc.X = Loc.X + object->priority * 0.1;
	object->SetActorLocation(Loc);
}

void AA_ArenaGameMode::laser_material_blue(int x, int y, int material_idx)
{
	BlueTile[x][y]->PlaneMesh->SetMaterial(0, StaticMaterialArray[material_idx]);
}

void AA_ArenaGameMode::laser_material_red(int x, int y, int material_idx)
{
	RedTile[x][y]->PlaneMesh->SetMaterial(0, StaticMaterialArray[material_idx]);
}

ABasicParent* AA_ArenaGameMode::Create_Dynamic_blue(int level, int x, int y, ABasicParent* prev_dynamic, int DynamicHeaderInt, int LaserDirection)
{
	FVector TempLoc;
	TempLoc = BlueTile[x][y]->GetActorLocation();
	TempLoc.X = TempLoc.X + level * 0.1;
	ABasicParent* TempDynamic = GetWorld()->SpawnActor<ABasicParent>(ActorToSpawn[DynamicHeaderInt], TempLoc, FRotator(-90.f, 0.f, 0.f));
	BlueTile[x][y]->ObjectVector[level] = TempDynamic;
	TempDynamic->x = x;
	TempDynamic->y = y;
	TempDynamic->blue = true;
	TempDynamic->Prev = prev_dynamic;
	TempDynamic->Next = nullptr;
	TempDynamic->LaserDirection = LaserDirection;
	TempDynamic->level = level;
	return TempDynamic;
}

ABasicParent* AA_ArenaGameMode::Create_Dynamic_red(int level, int x, int y, ABasicParent* prev_dynamic, int DynamicHeaderInt, int LaserDirection)
{
	FVector TempLoc;
	TempLoc = RedTile[x][y]->GetActorLocation();
	TempLoc.X = TempLoc.X + level * 0.1;
	ABasicParent* TempDynamic = GetWorld()->SpawnActor<ABasicParent>(ActorToSpawn[DynamicHeaderInt], TempLoc, FRotator(-90.f, 0.f, 0.f));
	RedTile[x][y]->ObjectVector[level] = TempDynamic;
	TempDynamic->x = x;
	TempDynamic->y = y;
	TempDynamic->blue = false;
	TempDynamic->Prev = prev_dynamic;
	TempDynamic->Next = nullptr;
	TempDynamic->LaserDirection = LaserDirection;
	TempDynamic->level = level;
	return TempDynamic;
}

ABasicParent* AA_ArenaGameMode::CreateMany_Blue(TArray<TArray<int>> positions, int DynHeaderText, int DynHeaderObject, ABasicParent* others)
{
	ABasicParent* dynamic1;
	dynamic1 = others;

	// If there is no text for the object
	if (DynHeaderText == DynHeaderObject) {
		while (dynamic1 && dynamic1->Next) dynamic1 = dynamic1->Next;
		if (!dynamic1) {
			ABasicParent* Dynamic = Create_Dynamic_blue(1, positions[0][0], positions[0][1], dynamic1, DynHeaderObject, positions[0][2]);
			if (DynHeaderObject == 12) Initialise_laser_light_blue(positions[0][0], positions[0][1]);
			dynamic1 = Dynamic;
			for (int i = 1; i < positions.Num(); i++) {
				dynamic1->Next = Create_Dynamic_blue(1, positions[i][0], positions[i][1], dynamic1, DynHeaderObject, positions[i][2]);
				dynamic1 = dynamic1->Next;
				if (DynHeaderObject == 12) Initialise_laser_light_blue(positions[i][0], positions[i][1]);
			}
			return Dynamic;
		}
		for (int i = 0; i < positions.Num(); i++) {
			dynamic1->Next = Create_Dynamic_blue(1, positions[i][0], positions[i][1], dynamic1, DynHeaderObject, positions[i][2]);
			dynamic1 = dynamic1->Next;
			if (DynHeaderObject == 12) Initialise_laser_light_blue(positions[i][0], positions[i][1]);
		}
		return others;
	}

	// If there is text for the object
	ABasicParent* Dynamic = Create_Dynamic_blue(2, positions[0][0], positions[0][1], nullptr, DynHeaderText, 0);
	if (others) {
		while (dynamic1->Next) dynamic1 = dynamic1->Next;
		for (int i = 1; i < positions.Num(); i++) {
			dynamic1->Next = Create_Dynamic_blue(1, positions[i][0], positions[i][1], dynamic1, DynHeaderObject, positions[i][2]);
			dynamic1 = dynamic1->Next;
			if (DynHeaderObject == 12) Initialise_laser_light_blue(positions[i][0], positions[i][1]);
		}
		Dynamic->Next = others->Next;
	}

	else {
		dynamic1 = Dynamic;
		for (int i = 1; i < positions.Num(); i++) {
			dynamic1->Next = Create_Dynamic_blue(1, positions[i][0], positions[i][1], dynamic1, DynHeaderObject, positions[i][2]);
			dynamic1 = dynamic1->Next;
			if (DynHeaderObject == 12) Initialise_laser_light_blue(positions[i][0], positions[i][1]);
		}
	}

	return Dynamic;
}

ABasicParent* AA_ArenaGameMode::CreateMany_Red(TArray<TArray<int>> positions, int DynHeaderText, int DynHeaderObject, ABasicParent* others)
{
	ABasicParent* dynamic1;
	dynamic1 = others;

	// If there is no text for the object
	if (DynHeaderText == DynHeaderObject) {
		while (dynamic1 && dynamic1->Next) dynamic1 = dynamic1->Next;
		if (!dynamic1) {
			ABasicParent* Dynamic = Create_Dynamic_red(1, positions[0][0], positions[0][1], dynamic1, DynHeaderObject, positions[0][2]);
			if (DynHeaderObject == 12) Initialise_laser_light_red(positions[0][0], positions[0][1]);
			dynamic1 = Dynamic;
			for (int i = 1; i < positions.Num(); i++) {
				dynamic1->Next = Create_Dynamic_red(1, positions[i][0], positions[i][1], dynamic1, DynHeaderObject, positions[i][2]);
				dynamic1 = dynamic1->Next;
				if (DynHeaderObject == 12) Initialise_laser_light_red(positions[i][0], positions[i][1]);
			}
			return Dynamic;
		}
		for (int i = 0; i < positions.Num(); i++) {
			dynamic1->Next = Create_Dynamic_red(1, positions[i][0], positions[i][1], dynamic1, DynHeaderObject, positions[i][2]);
			dynamic1 = dynamic1->Next;
			if (DynHeaderObject == 12) Initialise_laser_light_red(positions[i][0], positions[i][1]);
		}
		return others;
	}

	// If there is text for the object
	ABasicParent* Dynamic = Create_Dynamic_red(2, positions[0][0], positions[0][1], nullptr, DynHeaderText, 0);
	if (others) {
		while (dynamic1->Next) dynamic1 = dynamic1->Next;
		for (int i = 1; i < positions.Num(); i++) {
			dynamic1->Next = Create_Dynamic_red(1, positions[i][0], positions[i][1], dynamic1, DynHeaderObject, positions[i][2]);
			dynamic1 = dynamic1->Next;
			if (DynHeaderObject == 12) Initialise_laser_light_red(positions[i][0], positions[i][1]);
		}
		Dynamic->Next = others->Next;
	}

	else {
		dynamic1 = Dynamic;
		for (int i = 1; i < positions.Num(); i++) {
			dynamic1->Next = Create_Dynamic_red(1, positions[i][0], positions[i][1], dynamic1, DynHeaderObject, positions[i][2]);
			dynamic1 = dynamic1->Next;
			if (DynHeaderObject == 12) Initialise_laser_light_red(positions[i][0], positions[i][1]);
		}
	}

	return Dynamic;
}

void AA_ArenaGameMode::Create_Static_blue(int x, int y, int StaticHeaderInt)
{
	FVector TempLoc;
	TempLoc = BlueTile[x][y]->GetActorLocation();
	TempLoc.X = TempLoc.X + 0.1;
	BlueTile[x][y]->StaticTile = GetWorld()->SpawnActor<AStaticTile>(StaticToSpawn[StaticHeaderInt], TempLoc, FRotator(-90.f, 0.f, 0.f));
	BlueTile[x][y]->StaticTile->Blue = true;
}

void AA_ArenaGameMode::Create_Static_red(int x, int y, int StaticHeaderInt)
{
	FVector TempLoc;
	TempLoc = RedTile[x][y]->GetActorLocation();
	TempLoc.X = TempLoc.X + 0.1;
	RedTile[x][y]->StaticTile = GetWorld()->SpawnActor<AStaticTile>(StaticToSpawn[StaticHeaderInt], TempLoc, FRotator(-90.f, 0.f, 0.f));
	RedTile[x][y]->StaticTile->Blue = false;
}

void AA_ArenaGameMode::CreateDummyLevel()
{
	// Dynamic blue positions
	TArray<TArray<int>> Rock_pos_blue = { {8,10,0}, {0,5,0}, {0,6,0}, {0,7,0}, {1,8,0}, {2,9,0}, {3,8,0}, {4,7,0}, {4,6,0}, {4,5,0}, {1,7,0}, {2,7,0}, {3,7,0}, {6,0,0}, {6,1,0}, {6,2,0}, {6,3,0}, {7,1,0}, {7,2,0}, {8,0,0}, {8,3,0} };
	TArray<TArray<int>> TNT_pos_blue = { {9,8,0}, {6,5,0} };
	TArray<TArray<int>> Blue_pos = { {1,11,0}, {2,6,0} };
	TArray<TArray<int>> Movement_pos_blue = { {10,1,0}, {11,1,0}, {10,0,0}, {11,0,0} };

	// Dynamic red positions
	TArray<TArray<int>> Rock_pos_red = { {10,6,0}, {7,5,0}, {8,5,0}, {9,5,0}, {10,5,0}, {11,5,0}, {7,6,0}, {7,7,0}, {7,8,0}, {8,8,0}, {9,8,0}, {10,8,0}, {11,8,0} };
	TArray<TArray<int>> Laser_pos_red = { {11,11,0}, {8,10,4}, {9,9,1}, {2,2,4}, {3,3,2} };
	TArray<TArray<int>> Red_pos = { {1,10,0}, {9,7,0} };
	TArray<TArray<int>> Movement_pos_red = { {3,11,0}, {4,11,0}, {5,11,0}, {6,11,0} };
	TArray<TArray<int>> Food_pos_red = { {9,1,0}, {5,1,0} };

	// Dynamic for blue
	ABasicParent* Rock_blue = CreateMany_Blue(Rock_pos_blue, 4, 10, nullptr);
	ABasicParent* TNT_blue = CreateMany_Blue(TNT_pos_blue, 9, 9, nullptr);
	ABasicParent* Blue = CreateMany_Blue(Blue_pos, 7, 13, nullptr);

	// Dynamic for red
	ABasicParent* Rock_red = CreateMany_Red(Rock_pos_red, 4, 10, Rock_blue);
	ABasicParent* Laser_red = CreateMany_Red(Laser_pos_red, 6, 12, nullptr);
	ABasicParent* Food_red = CreateMany_Red(Food_pos_red, 5, 11, nullptr);
	ABasicParent* Red = CreateMany_Red(Red_pos, 8, 14, nullptr);

	// Movement textboxes
	ABasicParent* W_blue = Create_Dynamic_blue(2, Movement_pos_blue[0][0], Movement_pos_blue[0][1], nullptr, 0, 0);
	ABasicParent* S_blue = Create_Dynamic_blue(2, Movement_pos_blue[1][0], Movement_pos_blue[1][1], nullptr, 1, 0);
	ABasicParent* A_blue = Create_Dynamic_blue(2, Movement_pos_blue[2][0], Movement_pos_blue[2][1], nullptr, 2, 0);
	ABasicParent* D_blue = Create_Dynamic_blue(2, Movement_pos_blue[3][0], Movement_pos_blue[3][1], nullptr, 3, 0);
	ABasicParent* W_red = Create_Dynamic_red(2, Movement_pos_red[0][0], Movement_pos_red[0][1], nullptr, 0, 0);
	ABasicParent* S_red = Create_Dynamic_red(2, Movement_pos_red[1][0], Movement_pos_red[1][1], nullptr, 1, 0);
	ABasicParent* A_red = Create_Dynamic_red(2, Movement_pos_red[2][0], Movement_pos_red[2][1], nullptr, 2, 0);
	ABasicParent* D_red = Create_Dynamic_red(2, Movement_pos_red[3][0], Movement_pos_red[3][1], nullptr, 3, 0);

	// Statics for blue
	Create_Static_blue(1, 11, 6);
	Create_Static_blue(8, 10, 2);
	Create_Static_blue(10, 10, 1);
	Create_Static_blue(10, 1, 12);
	Create_Static_blue(11, 1, 14);
	Create_Static_blue(10, 0, 13);
	Create_Static_blue(11, 0, 15);

	// Statics for red
	Create_Static_red(1, 10, 7);
	Create_Static_red(11, 6, 3);
	Create_Static_red(9, 1, 4);
	Create_Static_red(3, 11, 8);
	Create_Static_red(4, 11, 9);
	Create_Static_red(5, 11, 10);
	Create_Static_red(6, 11, 11);

	// Spawning player1 with level
	currPlayer1 = Blue->Next;
	currPlayer1->change_level(2);

	// Spawning player2 with level
	currPlayer2 = Red->Next;
	currPlayer2->change_level(2);


	// Initialising the controls
	Food_red->Next->change_win(true);
	Rock_blue->Next->change_level(3);
	TNT_blue->change_level(2);
	TNT_blue->change_CanPass(true);
	Laser_red->Next->change_CanPass(false);

	// Initialising Func_Mapping
	Func_Mapping[0] = { 1,2,3,4 };
	Func_Mapping[1] = { 1,2,3,4 };
}

void AA_ArenaGameMode::Level1()
{

	
	TArray<TArray<int>> Rock_pos_blue = { {4,3,0},{4,5,0},{5,1,0},{5,2,0},{5,5,0},{6,0,0},{6,5,0},{6,6,0},{6,8,0},{7,0,0},{7,7,0},{8,0,0},{8,6,0},{8,8,0},{9,4,0},{9,6,0},{10,3,0},{10,5,0},{11,4,0} }; // saare real
	TArray<TArray<int>> Rock_pos_red = { {2,4,0},{2,5,0},{3,3,0},{3,5,0},{4,3,0},{4,5,0},{5,3,0},{5,5,0},{6,3,0},{6,4,0},{6,5,0},{8,8,0},{9,7,0},{9,9,0},{10,8,0} };// first pe likha hua , baaki real 
	TArray<TArray<int>> Laser_pos_blue = { {0,2,0},{3,10,2} };//likha hua laser fir real 
	TArray<TArray<int>> Food_pos_blue = { {0,0,0} };//likha hua food //likha hua blue 
	TArray<TArray<int>> Food_pos_red = { {9,8,0} };//real food 
	TArray<TArray<int>> Movement_pos_blue = { {7,4,0},{7,2,0},{11,11,0},{11,0,0} }; // WSAD blue side ki position
	TArray<TArray<int>> Movement_pos_red = { {5,0,0},{6,0,0},{1,0,0},{11,0,0} }; // WSAD Red side ki position0
	TArray<TArray<int>> Blue_pos = { {0,1,0},{5,4,0} };// pahle likha hua fir real
	TArray<TArray<int>> Red_pos = { {0,0,0},{5,4,0} };// pahle likha hua fir real

	//win PASS STOP
	Create_Static_blue(0, 0, 4);
	Create_Static_red(1, 3, 1);
	Create_Static_red(2, 4, 2);

	//player 1 player 2 
	Create_Static_blue(0, 1, 6);
	Create_Static_red(0, 0, 7);

	Create_Static_blue(11, 11, 12);
	Create_Static_blue(11, 0, 13);
	Create_Static_blue(7, 4, 14);
	Create_Static_blue(7, 2, 15);

	Create_Static_red(11, 0, 8);
	Create_Static_red(1, 0, 9);
	Create_Static_red(5, 0, 10);
	Create_Static_red(6, 0, 11);

	// Dynamic for blue
	ABasicParent* Rock_Red = CreateMany_Red(Rock_pos_red, 4, 10, nullptr);
	ABasicParent* Rock_blue = CreateMany_Blue(Rock_pos_blue, 10, 10, Rock_Red);
	ABasicParent* Laser_blue = CreateMany_Blue(Laser_pos_blue, 6, 12, nullptr);
	ABasicParent* Food_blue = CreateMany_Blue(Food_pos_blue, 5, 11, nullptr);
	ABasicParent* Food_red = CreateMany_Red(Food_pos_red, 11, 11, nullptr);
	ABasicParent* Blue = CreateMany_Blue(Blue_pos, 7, 13, nullptr);
	ABasicParent* Red = CreateMany_Red(Red_pos, 8, 14, nullptr);


	ABasicParent* W_blue = Create_Dynamic_blue(2, Movement_pos_blue[0][0], Movement_pos_blue[0][1], nullptr, 0, 0);
	ABasicParent* S_blue = Create_Dynamic_blue(2, Movement_pos_blue[1][0], Movement_pos_blue[1][1], nullptr, 1, 0);
	ABasicParent* A_blue = Create_Dynamic_blue(2, Movement_pos_blue[2][0], Movement_pos_blue[2][1], nullptr, 2, 0);
	ABasicParent* D_blue = Create_Dynamic_blue(2, Movement_pos_blue[3][0], Movement_pos_blue[3][1], nullptr, 3, 0);
	ABasicParent* W_red = Create_Dynamic_red(2, Movement_pos_red[0][0], Movement_pos_red[0][1], nullptr, 0, 0);
	ABasicParent* S_red = Create_Dynamic_red(2, Movement_pos_red[1][0], Movement_pos_red[1][1], nullptr, 1, 0);
	ABasicParent* A_red = Create_Dynamic_red(2, Movement_pos_red[2][0], Movement_pos_red[2][1], nullptr, 2, 0);
	ABasicParent* D_red = Create_Dynamic_red(2, Movement_pos_red[3][0], Movement_pos_red[3][1], nullptr, 3, 0);

	currPlayer1 = Blue->Next;
	currPlayer1->change_level(2);
	currPlayer2 = Red->Next;
	currPlayer2->change_level(2);
	Rock_Red->Next->change_level(3);
	Food_red->change_win(true);
	Func_Mapping[0] = { 3,4,2,1 };
	Func_Mapping[1] = { 3,4,1,2 };
}

void AA_ArenaGameMode::Level2()
{
	TArray<TArray<int>> Rock_pos_blue = { {8,0,0},{1,5,0},{1,6,0}, {1,7,0}, {2,5,0} ,{2,8,0},{3,5,0},{3,8,0},{4,5,0},{4,8,0},{5,8,0}
								,{6,8,0},{7,8,0},{8,8,0},{9,8,0},{10,8,0},{11,8,0} };
	TArray<TArray<int>> Rock_pos_red = { {6,2,0},{6,4,0},{7,1,0},{7,5,0},{8,2,0},{8,4,0} };
	TArray<TArray<int>> Laser_pos_blue = { {2,10,0},{5,7,2} };
	TArray<TArray<int>> Laser_pos_red = { {7,4,0}, {9,11,4} };
	TArray<TArray<int>> Food_pos_blue = { {3,10,0} };
	TArray<TArray<int>> Food_pos_red = { {3,10,0}, {5,7,0},{6,6,0},{6,8,0},{7,7,0} };

	TArray<TArray<int>> Blue_pos = { {11,0,0},{2,7,0}};
	TArray<TArray<int>> Red_pos = { {7,3,0},{6,7,0} };

	TArray<TArray<int>> Movement_pos_blue = { {0,0,0},{0,1,0},{0,2,0},{0,3,0} };
	TArray<TArray<int>> Movement_pos_red = { {0,0,0},{0,1,0},{0,2,0},{0,3,0} };

	//static spawn
	Create_Static_blue(2, 10, 1);
	Create_Static_blue(3, 10, 2);
	Create_Static_blue(8, 0, 2);
	Create_Static_blue(11, 0, 6);
	Create_Static_red(7, 3, 7);
	Create_Static_red(7, 2, 4);

	ABasicParent* W_blue = Create_Dynamic_blue(2, Movement_pos_blue[0][0], Movement_pos_blue[0][1], nullptr, 0, 0);
	ABasicParent* S_blue = Create_Dynamic_blue(2, Movement_pos_blue[1][0], Movement_pos_blue[1][1], nullptr, 1, 0);
	ABasicParent* A_blue = Create_Dynamic_blue(2, Movement_pos_blue[2][0], Movement_pos_blue[2][1], nullptr, 2, 0);
	ABasicParent* D_blue = Create_Dynamic_blue(2, Movement_pos_blue[3][0], Movement_pos_blue[3][1], nullptr, 3, 0);
	ABasicParent* W_red = Create_Dynamic_red(2, Movement_pos_red[0][0], Movement_pos_red[0][1], nullptr, 0, 0);
	ABasicParent* S_red = Create_Dynamic_red(2, Movement_pos_red[1][0], Movement_pos_red[1][1], nullptr, 1, 0);
	ABasicParent* A_red = Create_Dynamic_red(2, Movement_pos_red[2][0], Movement_pos_red[2][1], nullptr, 2, 0);
	ABasicParent* D_red = Create_Dynamic_red(2, Movement_pos_red[3][0], Movement_pos_red[3][1], nullptr, 3, 0);

	Create_Static_blue(0, 0, 12);
	Create_Static_blue(0, 1, 13);
	Create_Static_blue(0, 2, 15);
	Create_Static_blue(0, 3, 14);

	Create_Static_red(0, 0, 8);
	Create_Static_red(0, 1, 9);
	Create_Static_red(0, 2, 11);
	Create_Static_red(0, 3, 10);

	ABasicParent* Rock_blue = CreateMany_Blue(Rock_pos_blue, 4, 10, nullptr);
	ABasicParent* Rock_Red = CreateMany_Red(Rock_pos_red, 10, 10, Rock_blue);
	ABasicParent* Laser_red = CreateMany_Red(Laser_pos_red, 6, 12, nullptr);
	ABasicParent* Laser_blue = CreateMany_Blue(Laser_pos_blue, 6, 12, Laser_red);
	ABasicParent* Food_blue = CreateMany_Blue(Food_pos_blue, 5, 11, nullptr);
	ABasicParent* Food_red = CreateMany_Red(Food_pos_red, 5, 11, Food_blue);
	ABasicParent* Blue = CreateMany_Blue(Blue_pos, 7, 13, nullptr);
	ABasicParent* Red = CreateMany_Red(Red_pos, 8, 14, nullptr);

	currPlayer2 = Red->Next;
	currPlayer2->change_level(2);

	currPlayer1 = Blue->Next;
	currPlayer1->change_level(2);

	Food_blue->Next->change_level(3);
	Laser_blue->Next->change_level(2);
	Rock_blue->Next->change_level(3);

	

	Func_Mapping[0] = { 1,2,4,3 };
	Func_Mapping[1] = { 1,2,4,3 };
}

void AA_ArenaGameMode::Level3()
{
	// Blue Positions
	TArray<TArray<int>> Rock_pos_blue = { {9, 3, 0}, {5, 1, 0}, {5, 2, 0}, {5, 3, 0}, {5, 4, 0}, {5, 5, 0}, {5, 7, 0}, {5, 8, 0}, {5, 9, 0}, {1, 8, 0}, {1, 9, 0}, {1, 10, 0}, {2, 9, 0}, {3, 8, 0}, {3, 9, 0}, {3, 10, 0}, {6, 9, 0}, {7, 6, 0}, {7, 9, 0}, {8, 8, 0}, {1,7,0}, {3,7,0} };
	TArray<TArray<int>> Blue_pos = { {3, 3, 0}, {6, 8, 0} };
	TArray<TArray<int>> Laser_pos_blue = { {0, 0, 0} };
	TArray<TArray<int>> TNT_pos_blue = { {1, 4, 0}, {3, 4, 0}, {4, 4, 0}, {3, 5, 0} };
	TArray<TArray<int>> Movement_pos_blue = { {5, 6, 0}, {1, 5, 0}, {4, 5, 0}, {8, 7, 0} };

	// Red Positions
	TArray<TArray<int>> Rock_pos_red = { {5, 4, 0}, {2, 6, 0}, {3, 5, 0}, {4, 3, 0}, {4, 5, 0}, {5, 3, 0}, {5, 5, 0}, {6, 3, 0}, {6, 5, 0}, {7, 3, 0}, {7, 5, 0}, {7, 6, 0}, {7, 7, 0}, {7, 8, 0}, {7, 9, 0}, {8, 3, 0}, {9, 3, 0}, {9, 4, 0}, {9, 5, 0}, {9, 6, 0}, {9, 7, 0}, {9, 8, 0}, {9, 9, 0}, {9, 10, 0}, {9, 11, 0} };
	TArray<TArray<int>> Red_pos = { {1, 5, 0}, {2, 11, 0} };
	TArray<TArray<int>> Laser_pos_red = { {0, 3, 1}, {3, 8, 3} };
	TArray<TArray<int>> TNT_pos_red = { {0, 0, 0} };
	TArray<TArray<int>> Movement_pos_red = { {5, 0, 0}, {6, 0, 0}, {7, 0, 0}, {8, 0, 0} };
	TArray<TArray<int>> Food_pos_red = { {2, 7, 0}, {1, 7, 0} };

	// Dynamic for Red
	ABasicParent* Rock_red = CreateMany_Red(Rock_pos_red, 4, 10, nullptr);
	ABasicParent* Laser_red = CreateMany_Red(Laser_pos_red, 12, 12, nullptr);
	ABasicParent* TNT_red = CreateMany_Red(TNT_pos_red, 9, 9, nullptr);
	ABasicParent* Red = CreateMany_Red(Red_pos, 8, 14, nullptr);
	ABasicParent* Food_red = CreateMany_Red(Food_pos_red, 5, 11, nullptr);

	// Dynamic for Blue
	ABasicParent* Rock_blue = CreateMany_Blue(Rock_pos_blue, 4, 10, Rock_red);
	ABasicParent* Laser_blue = CreateMany_Blue(Laser_pos_blue, 6, 12, Laser_red);
	ABasicParent* Blue = CreateMany_Blue(Blue_pos, 7, 13, nullptr);
	ABasicParent* TNT_blue = CreateMany_Blue(TNT_pos_blue, 9, 9, TNT_red);

	// Movement textboxes
	ABasicParent* W_blue = Create_Dynamic_blue(2, Movement_pos_blue[0][0], Movement_pos_blue[0][1], nullptr, 0, 0);
	ABasicParent* S_blue = Create_Dynamic_blue(2, Movement_pos_blue[1][0], Movement_pos_blue[1][1], nullptr, 1, 0);
	ABasicParent* A_blue = Create_Dynamic_blue(2, Movement_pos_blue[2][0], Movement_pos_blue[2][1], nullptr, 2, 0);
	ABasicParent* D_blue = Create_Dynamic_blue(2, Movement_pos_blue[3][0], Movement_pos_blue[3][1], nullptr, 3, 0);
	ABasicParent* W_red = Create_Dynamic_red(2, Movement_pos_red[0][0], Movement_pos_red[0][1], nullptr, 0, 0);
	ABasicParent* S_red = Create_Dynamic_red(2, Movement_pos_red[1][0], Movement_pos_red[1][1], nullptr, 1, 0);
	ABasicParent* A_red = Create_Dynamic_red(2, Movement_pos_red[2][0], Movement_pos_red[2][1], nullptr, 2, 0);
	ABasicParent* D_red = Create_Dynamic_red(2, Movement_pos_red[3][0], Movement_pos_red[3][1], nullptr, 3, 0);

	// Statics for blue
	Create_Static_blue(3, 3, 6);
	Create_Static_blue(9, 3, 2);
	Create_Static_blue(6, 6, 1);
	Create_Static_blue(5, 6, 12);
	Create_Static_blue(1, 5, 13);
	Create_Static_blue(4, 5, 14);
	Create_Static_blue(8, 7, 15);

	// Statics for red
	Create_Static_red(1, 5, 7);
	Create_Static_red(11, 11, 3);
	Create_Static_red(2, 7, 4);
	Create_Static_red(5, 0, 8);
	Create_Static_red(6, 0, 9);
	Create_Static_red(7, 0, 10);
	Create_Static_red(8, 0, 11);


	// Spawning player1 with level
	currPlayer1 = Blue->Next;
	currPlayer1->change_level(2);

	// Spawning player2 with level
	currPlayer2 = Red->Next;
	currPlayer2->change_level(2);

	// Initialising the controls
	Food_red->Next->change_win(true);
	Rock_red->Next->change_level(3);
	TNT_red->change_level(2);
	TNT_red->change_CanPass(true);
	Laser_red->Next->change_CanPass(false);

	

	Func_Mapping[0] = { 1, 2, 3, 4 };
	Func_Mapping[1] = { 1, 2, 3, 4 };
}

void AA_ArenaGameMode::Level4()
{
	// Blue Positions
	TArray<TArray<int>> Rock_pos_blue = { {5,10,0}, {10,0,0}, {1,1,0}, {3,1,0}, {10,1,0}, {11,1,0}, {1,2,0}, {2,2,0}, {3,2,0}, {1,3,0}, {3,3,0}, {2,4,0},{2,5,0}, {1,6,0}, {3,6,0}, {1,7,0}, {2,7,0}, {3,7,0}, {1,8,0}, {3,8,0}, {1,9,0}, {3,9,0} };
	TArray<TArray<int>> Laser_pos_blue = { {0,11,0}, {7,11,2} };
	TArray<TArray<int>> Blue_pos = { {11,0,0}, {2,8,0} };
	TArray<TArray<int>> Movement_pos_blue = { {6,0,0}, {6,3,0}, {6,2,0}, {6,1,0} };
	TArray<TArray<int>> TNT_pos_blue = { {5,0,0}, {5,1,0}, {5,2,0}, {5,3,0}, {10,6,0}, {11,6,0}, {11,8,0}, {11,9,0}, {6,5,0} };

	// Red Positions
	TArray<TArray<int>> Rock_pos_red = { {3,1,0}, {7,3,0}, {8,3,0}, {5,5,0}, {5,8,0}, {7,11,0}, {6,11,0} };
	TArray<TArray<int>> Laser_pos_red = { {10,11,0}, {8,9,4}, {9,8,1} };
	TArray<TArray<int>> Red_pos = { {10,6,0}, {8,2,0} };
	TArray<TArray<int>> Movement_pos_red = { {4,5,0}, {6,6,0}, {3,6,0}, {5,7,0} };
	TArray<TArray<int>> TNT_pos_red = { {0,9,0} };
	TArray<TArray<int>> Food_pos_red = { {10,10,0}, {11,11,0} };

	// Dynamic for red
	ABasicParent* Rock_red = CreateMany_Red(Rock_pos_red, 4, 10, nullptr);
	ABasicParent* Laser_red = CreateMany_Red(Laser_pos_red, 6, 12, nullptr);
	ABasicParent* Red = CreateMany_Red(Red_pos, 8, 14, nullptr);
	ABasicParent* TNT_red = CreateMany_Red(TNT_pos_red, 9, 9, nullptr);
	ABasicParent* Food_red = CreateMany_Red(Food_pos_red, 5, 11, nullptr);

	// Dynamic for blue
	ABasicParent* Rock_blue = CreateMany_Blue(Rock_pos_blue, 4, 10, Rock_red);
	ABasicParent* Laser_blue = CreateMany_Blue(Laser_pos_blue, 6, 12, Laser_red);
	ABasicParent* Blue = CreateMany_Blue(Blue_pos, 7, 13, nullptr);
	ABasicParent* TNT_blue = CreateMany_Blue(TNT_pos_blue, 9, 9, TNT_red);

	if (Rock_blue) {
		UE_LOG(LogTemp, Warning, TEXT("da"));
		if (Rock_blue->Next) {
			UE_LOG(LogTemp, Warning, TEXT("sss"));
		}
	}
	// Movement textboxes
	ABasicParent* W_blue = Create_Dynamic_blue(2, Movement_pos_blue[0][0], Movement_pos_blue[0][1], nullptr, 0, 0);
	ABasicParent* S_blue = Create_Dynamic_blue(2, Movement_pos_blue[1][0], Movement_pos_blue[1][1], nullptr, 1, 0);
	ABasicParent* A_blue = Create_Dynamic_blue(2, Movement_pos_blue[2][0], Movement_pos_blue[2][1], nullptr, 2, 0);
	ABasicParent* D_blue = Create_Dynamic_blue(2, Movement_pos_blue[3][0], Movement_pos_blue[3][1], nullptr, 3, 0);
	ABasicParent* W_red = Create_Dynamic_red(2, Movement_pos_red[0][0], Movement_pos_red[0][1], nullptr, 0, 0);
	ABasicParent* S_red = Create_Dynamic_red(2, Movement_pos_red[1][0], Movement_pos_red[1][1], nullptr, 1, 0);
	ABasicParent* A_red = Create_Dynamic_red(2, Movement_pos_red[2][0], Movement_pos_red[2][1], nullptr, 2, 0);
	ABasicParent* D_red = Create_Dynamic_red(2, Movement_pos_red[3][0], Movement_pos_red[3][1], nullptr, 3, 0);

	// Statics for blue
	Create_Static_blue(11, 0, 6);
	Create_Static_blue(5, 10, 2);
	Create_Static_blue(9, 11, 1);
	Create_Static_blue(6, 3, 12);
	Create_Static_blue(6, 2, 14);
	Create_Static_blue(6, 1, 13);
	Create_Static_blue(6, 0, 15);

	// Statics for red
	Create_Static_red(10, 6, 7);
	Create_Static_red(2, 1, 3);
	Create_Static_red(10, 10, 4);
	Create_Static_red(4, 5, 8);
	Create_Static_red(6, 6, 9);
	Create_Static_red(3, 6, 10);
	Create_Static_red(5, 7, 11);

	// Spawning player1 with level
	currPlayer1 = Blue->Next;
	currPlayer1->change_level(2);

	// Spawning player2 with level
	currPlayer2 = Red->Next;
	currPlayer2->change_level(2);


	// Initialising the controls
	Food_red->Next->change_win(true);
	Rock_red->Next->change_level(3);
	TNT_blue->change_level(2);
	TNT_blue->change_CanPass(true);
	Laser_blue->Next->change_CanPass(false);	

	// Initialising Func_Mapping
	Func_Mapping[0] = { 1,2,3,4 };
	Func_Mapping[1] = { 4,1,3,2 };
}

void AA_ArenaGameMode::Level5()
{
	// Blue Positions
	TArray<TArray<int>> Rock_pos_blue = { {0, 0, 0}, {0, 2, 0}, {1, 2, 0}, {2, 2, 0}, {2, 1, 0}, {2, 0, 0}, {9, 11, 0} };
	TArray<TArray<int>> Laser_pos_blue = { {10,4,0}, {8,11,2} };
	TArray<TArray<int>> Blue_pos = { {6,7,0}, {9,0,0} };
	TArray<TArray<int>> Food_pos_blue = { {9,8,0} };
	TArray<TArray<int>> Movement_pos_blue = { {1,8,0},{9,1,0},{4,6,0},{7,10,0} };

	// Red Positions
	TArray<TArray<int>> Rock_pos_red = { {0,7,0}, {0,8,0}, {0,9,0},{1,7,0}, {1,9,0}, {2,7,0}, {2,9,0}, {3,7,0}, {3,9,0}, {4,6,0}, {4,10,0}, {6,7,0}, {6,8,0}, {6,9,0} };
	TArray<TArray<int>> Laser_pos_red = { {9,3,0}, {9,7,1}, {10,7,1} };
	TArray<TArray<int>> Red_pos = { {3,3,0}, {3,8,0} };
	TArray<TArray<int>> Food_pos_red = { {1,8,0} };
	TArray<TArray<int>> Movement_pos_red = { {8,5,0},{10,0,0},{10,5,0},{11,0,0} };
	TArray<TArray<int>> TNT_pos_red = { {9,5,0} };

	// Dynamic for blue
	ABasicParent* Rock_blue = CreateMany_Blue(Rock_pos_blue, 4, 10, nullptr);
	ABasicParent* Laser_blue = CreateMany_Blue(Laser_pos_blue, 6, 12, nullptr);
	ABasicParent* Blue = CreateMany_Blue(Blue_pos, 7, 13, nullptr);
	ABasicParent* Food_blue = CreateMany_Blue(Food_pos_blue, 5, 11, nullptr);

	// Dynamic for red
	ABasicParent* Rock_red = CreateMany_Red(Rock_pos_red, 10, 10, Rock_blue);
	ABasicParent* Laser_red = CreateMany_Red(Laser_pos_red, 6, 12, Laser_blue);
	ABasicParent* Red = CreateMany_Red(Red_pos, 8, 14, nullptr);
	ABasicParent* TNT_red = CreateMany_Red(TNT_pos_red, 9, 9, nullptr);
	ABasicParent* Food_red = CreateMany_Red(Food_pos_red, 11, 11, Food_blue);

	// Movement textboxes
	ABasicParent* W_blue = Create_Dynamic_blue(2, Movement_pos_blue[0][0], Movement_pos_blue[0][1], nullptr, 0, 0);
	ABasicParent* S_blue = Create_Dynamic_blue(2, Movement_pos_blue[1][0], Movement_pos_blue[1][1], nullptr, 1, 0);
	ABasicParent* A_blue = Create_Dynamic_blue(2, Movement_pos_blue[2][0], Movement_pos_blue[2][1], nullptr, 2, 0);
	ABasicParent* D_blue = Create_Dynamic_blue(2, Movement_pos_blue[3][0], Movement_pos_blue[3][1], nullptr, 3, 0);
	ABasicParent* W_red = Create_Dynamic_red(2, Movement_pos_red[0][0], Movement_pos_red[0][1], nullptr, 0, 0);
	ABasicParent* S_red = Create_Dynamic_red(2, Movement_pos_red[1][0], Movement_pos_red[1][1], nullptr, 1, 0);
	ABasicParent* A_red = Create_Dynamic_red(2, Movement_pos_red[2][0], Movement_pos_red[2][1], nullptr, 2, 0);
	ABasicParent* D_red = Create_Dynamic_red(2, Movement_pos_red[3][0], Movement_pos_red[3][1], nullptr, 3, 0);

	// Statics for blue
	Create_Static_blue(6, 7, 6);
	Create_Static_blue(0, 0, 2);
	Create_Static_blue(9, 8, 4);
	Create_Static_blue(11, 4, 3);
	Create_Static_blue(1, 8, 12);
	Create_Static_blue(9, 1, 13);
	Create_Static_blue(4, 6, 14);
	Create_Static_blue(7, 10, 15);

	// Statics for red
	Create_Static_red(3, 3, 7);
	Create_Static_red(0, 0, 1);
	Create_Static_red(9, 3, 5);
	Create_Static_red(8, 5, 8);
	Create_Static_red(10, 0, 9);
	Create_Static_red(10, 5, 10);
	Create_Static_red(11, 0, 11);

	// Initialising the controls
	currPlayer1 = Blue->Next;
	currPlayer1->change_level(2);

	// Spawning player2 with level
	currPlayer2 = Red->Next;
	currPlayer2->change_level(2);

	Food_red->Next->change_win(true);
	Rock_red->Next->change_level(3);
	TNT_red->change_level(2);
	TNT_red->change_CanPass(true);
	Laser_red->Next->change_CanPass(false);
	Laser_red->Next->change_defeat(true);

	// Spawning player1 with level



	Func_Mapping[0] = { 1, 2, 3, 4 };
	Func_Mapping[1] = { 1, 2, 3, 4 };
}

void AA_ArenaGameMode::Level6()
{
	TArray<TArray<int>> Rock_pos_blue = { {7,8,0}, {0,9,0}, {2,9,0}, {2,10,0}, {2,11,0}, {3,9,0}, {3,10,0}, {3,11,0}, {4,5,0} };
	TArray<TArray<int>> Laser_pos_blue = { {3,7,3}, {3,8,3}, {6,0,4}, {6,1,4}, {6,3,4}, {7,5,2}, {8,5,2}, {10,5,2} };
	TArray<TArray<int>> Blue_pos = { {7,7,0}, {0,11,0} };
	TArray<TArray<int>> Food_pos_blue = { {2,3,0}, {9,2,0} };
	TArray<TArray<int>> Movement_pos_blue = { {3,3,0}, {2,2,0}, {1,3,0}, {2,4,0} };

	// Red Positions
	TArray<TArray<int>> Rock_pos_red = { {2,2,0}, {4,5,0}, {4,6,0}, {4,7,0}, {5,6,0}, {6,4,0}, {6,5,0}, {6,6,0} };
	TArray<TArray<int>> Laser_pos_red = { {11,10,0}, {8,8,4}, {8,9,4}, {9,7,1}, {10,7,1} };
	TArray<TArray<int>> Red_pos = { {1,10,0}, {8,5,0} };
	TArray<TArray<int>> Food_pos_red = { {11,11,11} };
	TArray<TArray<int>> Movement_pos_red = { {3,2,0}, {2,3,0}, {2,1,0}, {1,2,0} };
	TArray<TArray<int>> TNT_pos_red = { {10,0,0}, {11,0,0}, {10,1,0}, {11,1,0} };

	// Dynamic for blue
	ABasicParent* Rock_red = CreateMany_Red(Rock_pos_red, 4, 10, nullptr);
	ABasicParent* Rock_blue = CreateMany_Blue(Rock_pos_blue, 10, 10, Rock_red);
	ABasicParent* Laser_red = CreateMany_Red(Laser_pos_red, 6, 12, nullptr);
	ABasicParent* Laser_blue = CreateMany_Blue(Laser_pos_blue, 12, 12, Laser_red);
	ABasicParent* Blue = CreateMany_Blue(Blue_pos, 7, 13, nullptr);
	ABasicParent* Food_blue = CreateMany_Blue(Food_pos_blue, 5, 11, nullptr);

	// Dynamic for red
	ABasicParent* Red = CreateMany_Red(Red_pos, 8, 14, nullptr);
	ABasicParent* TNT_red = CreateMany_Red(TNT_pos_red, 9, 9, nullptr);
	ABasicParent* Food_red = CreateMany_Red(Food_pos_red, 11, 11, Food_blue);

	// Movement textboxes
	ABasicParent* W_blue = Create_Dynamic_blue(2, Movement_pos_blue[0][0], Movement_pos_blue[0][1], nullptr, 0, 0);
	ABasicParent* S_blue = Create_Dynamic_blue(2, Movement_pos_blue[1][0], Movement_pos_blue[1][1], nullptr, 1, 0);
	ABasicParent* A_blue = Create_Dynamic_blue(2, Movement_pos_blue[2][0], Movement_pos_blue[2][1], nullptr, 2, 0);
	ABasicParent* D_blue = Create_Dynamic_blue(2, Movement_pos_blue[3][0], Movement_pos_blue[3][1], nullptr, 3, 0);
	ABasicParent* W_red = Create_Dynamic_red(2, Movement_pos_red[0][0], Movement_pos_red[0][1], nullptr, 0, 0);
	ABasicParent* S_red = Create_Dynamic_red(2, Movement_pos_red[1][0], Movement_pos_red[1][1], nullptr, 1, 0);
	ABasicParent* A_red = Create_Dynamic_red(2, Movement_pos_red[2][0], Movement_pos_red[2][1], nullptr, 2, 0);
	ABasicParent* D_red = Create_Dynamic_red(2, Movement_pos_red[3][0], Movement_pos_red[3][1], nullptr, 3, 0);

	// Statics for blue
	Create_Static_blue(7, 7, 6);
	Create_Static_blue(2, 3, 4);
	Create_Static_blue(3, 3, 12);
	Create_Static_blue(2, 2, 13);
	Create_Static_blue(1, 3, 14);
	Create_Static_blue(2, 4, 15);

	// Statics for red
	Create_Static_red(1, 10, 7);
	Create_Static_red(2, 2, 2);
	Create_Static_red(5, 2, 1);
	Create_Static_red(11, 2, 3);
	Create_Static_red(1, 2, 8);
	Create_Static_red(3, 2, 9);
	Create_Static_red(2, 3, 10);
	Create_Static_red(2, 1, 11);

	currPlayer1 = Blue->Next;
	currPlayer1->change_level(2);

	// Spawning player2 with level
	currPlayer2 = Red->Next;
	currPlayer2->change_level(2);
	// Initialising the controls
	Food_blue->Next->change_win(true);
	Rock_blue->Next->change_level(3);
	TNT_red->change_level(2);
	TNT_red->change_CanPass(true);
	Laser_blue->Next->change_CanPass(false);

	// Spawning player1 with level

	Func_Mapping[0] = { 1, 2, 3, 4 };
	Func_Mapping[1] = { 1, 2, 3, 4 };
	// Blue Positions
	
}

void AA_ArenaGameMode::Level7()
{
	TArray<TArray<int>> Rock_pos_blue = { {2,2, 0}, {2 ,3, 0}, {2, 6, 0}, {2, 7, 0}, {3, 2, 0}, {3, 3, 0}, {3, 6, 0}
								,{3,7, 0}, {4 ,2, 0}, {4, 3, 0}, {4, 6, 0}, {4, 7, 0}, {5, 2, 0}, {5, 3, 0},
		{5,6, 0}, {5 ,7, 0}, {6, 2, 0}, {6, 3, 0}, {6, 6, 0}, {6, 7, 0}
	};
	TArray<TArray<int>> Blue_pos = { {11,0,0}, {4,4,0} };
	TArray<TArray<int>> Food_pos_blue = { {6,6,0} };
	TArray<TArray<int>> Movement_pos_blue = { {0,0,0},{0,1,0},{0,2,0},{0,3,0} };

	// Red Positions
	TArray<TArray<int>> Laser_pos_red = { {11,11,0}, {3,11,2}, {7,11,2},{9,7,3}, {9,3,3} };
	TArray<TArray<int>> Red_pos = { {11,11,0}, {5,4,0} };
	TArray<TArray<int>> Food_pos_red = { {5,5,0} };
	TArray<TArray<int>> Movement_pos_red = { {0,0,0},{0,1,0},{0,2,0},{0,3,0} };

	// Dynamic for blue
	ABasicParent* Rock_blue = CreateMany_Blue(Rock_pos_blue, 4, 10, nullptr);
	ABasicParent* Blue = CreateMany_Blue(Blue_pos, 7, 13, nullptr);


	// Dynamic for red
	ABasicParent* Red = CreateMany_Red(Red_pos, 8, 14, nullptr);
	ABasicParent* Food_red = CreateMany_Red(Food_pos_red, 5, 11, nullptr);
	ABasicParent* Food_blue = CreateMany_Blue(Food_pos_blue, 11, 11, Food_red);
	ABasicParent* Laser_red = CreateMany_Blue(Laser_pos_red, 6, 12, nullptr);
	// Movement textboxes
	ABasicParent* W_blue = Create_Dynamic_blue(2, Movement_pos_blue[0][0], Movement_pos_blue[0][1], nullptr, 0, 0);
	ABasicParent* S_blue = Create_Dynamic_blue(2, Movement_pos_blue[1][0], Movement_pos_blue[1][1], nullptr, 1, 0);
	ABasicParent* A_blue = Create_Dynamic_blue(2, Movement_pos_blue[2][0], Movement_pos_blue[2][1], nullptr, 2, 0);
	ABasicParent* D_blue = Create_Dynamic_blue(2, Movement_pos_blue[3][0], Movement_pos_blue[3][1], nullptr, 3, 0);
	ABasicParent* W_red = Create_Dynamic_red(2, Movement_pos_red[0][0], Movement_pos_red[0][1], nullptr, 0, 0);
	ABasicParent* S_red = Create_Dynamic_red(2, Movement_pos_red[1][0], Movement_pos_red[1][1], nullptr, 1, 0);
	ABasicParent* A_red = Create_Dynamic_red(2, Movement_pos_red[2][0], Movement_pos_red[2][1], nullptr, 2, 0);
	ABasicParent* D_red = Create_Dynamic_red(2, Movement_pos_red[3][0], Movement_pos_red[3][1], nullptr, 3, 0);

	// Statics for blue
	Create_Static_blue(2, 2, 4);
	Create_Static_red(10, 11, 2);
	Create_Static_blue(0, 0, 12);
	Create_Static_blue(0, 1, 13);
	Create_Static_blue(0, 2, 14);
	Create_Static_blue(0, 3, 15);

	// Statics for red
	Create_Static_red(0, 0, 8);
	Create_Static_red(0, 1, 9);
	Create_Static_red(0, 2, 10);
	Create_Static_red(0, 3, 11);

	// Initialising the controls

	// Spawning player1 with level
	currPlayer1 = Blue->Next;
	currPlayer1->change_level(2);

	// Spawning player2 with level
	currPlayer2 = Red->Next;
	currPlayer2->change_level(2);
	Food_red->Next->change_win(true);

	Func_Mapping[0] = { 1, 2, 3, 4 };
	Func_Mapping[1] = { 1, 2, 3, 4 };
	// Blue Positions
	/*
	TArray<TArray<int>> Rock_pos_blue = { {7,8,0}, {0,9,0}, {2,9,0}, {2,10,0}, {2,11,0}, {3,9,0}, {3,10,0}, {3,11,0}, {4,5,0} };
	TArray<TArray<int>> Laser_pos_blue = { {3,7,3}, {3,8,3}, {6,0,4}, {6,1,4}, {6,2,4}, {6,3,4}, {7,5,2}, {8,5,2}, {9,5,2}, {10,5,2} };
	TArray<TArray<int>> Blue_pos = { {7,7}, {0,11} };
	TArray<TArray<int>> Food_pos_blue = { {2,3,0}, {9,2,0} };
	TArray<TArray<int>> Movement_pos_blue = { {3,3,0}, {2,2,0}, {1,3,0}, {2,4,0} };

	// Red Positions
	TArray<TArray<int>> Rock_pos_red = { {2,2}, {4,5}, {4,6}, {4,7}, {5,6}, {6,4}, {6,5}, {6,6} };
	TArray<TArray<int>> Laser_pos_red = { {11,10,0}, {8,8,4}, {8,9,4}, {9,7,1}, {10,7,1} };
	TArray<TArray<int>> Red_pos = { {1,10,0}, {8,5,0} };
	TArray<TArray<int>> Food_pos_red = { {11,11,11} };
	TArray<TArray<int>> Movement_pos_red = { {3,2,0}, {2,3,0}, {2,1,0}, {1,2,0} };
	TArray<TArray<int>> TNT_pos_red = { {10,0,0}, {11,0,0}, {10,1,0}, {11,1,0} };

	// Dynamic for blue
	ABasicParent* Rock_blue = CreateMany_Blue(Rock_pos_blue, 4, 10, nullptr);
	ABasicParent* Laser_blue = CreateMany_Blue(Laser_pos_blue, 6, 12, nullptr);
	ABasicParent* Blue = CreateMany_Blue(Blue_pos, 7, 13, nullptr);
	ABasicParent* Food_blue = CreateMany_Blue(Food_pos_blue, 5, 11, nullptr);

	// Dynamic for red
	ABasicParent* Rock_red = CreateMany_Red(Rock_pos_red, 10, 10, Rock_blue);
	ABasicParent* Laser_red = CreateMany_Red(Laser_pos_red, 12, 12, Laser_blue);
	ABasicParent* Red = CreateMany_Red(Red_pos, 8, 14, nullptr);
	ABasicParent* TNT_red = CreateMany_Red(TNT_pos_red, 9, 9, nullptr);
	ABasicParent* Food_red = CreateMany_Red(Food_pos_red, 11, 11, Food_blue);

	// Movement textboxes
	ABasicParent* W_blue = Create_Dynamic_blue(2, Movement_pos_blue[0][0], Movement_pos_blue[0][1], nullptr, 0, 0);
	ABasicParent* S_blue = Create_Dynamic_blue(2, Movement_pos_blue[1][0], Movement_pos_blue[1][1], nullptr, 1, 0);
	ABasicParent* A_blue = Create_Dynamic_blue(2, Movement_pos_blue[2][0], Movement_pos_blue[2][1], nullptr, 2, 0);
	ABasicParent* D_blue = Create_Dynamic_blue(2, Movement_pos_blue[3][0], Movement_pos_blue[3][1], nullptr, 3, 0);
	ABasicParent* W_red = Create_Dynamic_red(2, Movement_pos_red[0][0], Movement_pos_red[0][1], nullptr, 0, 0);
	ABasicParent* S_red = Create_Dynamic_red(2, Movement_pos_red[1][0], Movement_pos_red[1][1], nullptr, 1, 0);
	ABasicParent* A_red = Create_Dynamic_red(2, Movement_pos_red[2][0], Movement_pos_red[2][1], nullptr, 2, 0);
	ABasicParent* D_red = Create_Dynamic_red(2, Movement_pos_red[3][0], Movement_pos_red[3][1], nullptr, 3, 0);

	// Statics for blue
	Create_Static_blue(7, 7, 6);
	Create_Static_blue(2, 3, 4);
	Create_Static_blue(3, 3, 12);
	Create_Static_blue(2, 2, 13);
	Create_Static_blue(1, 3, 14);
	Create_Static_blue(2, 4, 15);

	// Statics for red
	Create_Static_red(1, 10, 7);
	Create_Static_red(2, 2, 2);
	Create_Static_red(5, 2, 1);
	Create_Static_red(11, 2, 3);
	Create_Static_red(1, 2, 8);
	Create_Static_red(3, 2, 9);
	Create_Static_red(2, 3, 10);
	Create_Static_red(2, 1, 11);

	currPlayer1 = Blue->Next;
	currPlayer1->change_level(2);

	// Spawning player2 with level
	currPlayer2 = Red->Next;
	currPlayer2->change_level(2);
	// Initialising the controls
	Food_blue->Next->change_win(true);
	Rock_blue->Next->change_level(3); 
	TNT_red->change_level(2);
	TNT_red->change_CanPass(true);
	Laser_blue->Next->change_CanPass(false);

	// Spawning player1 with level

	Func_Mapping[0] = { 1, 2, 3, 4 };
	Func_Mapping[1] = { 1, 2, 3, 4 };
	*/
}

void AA_ArenaGameMode::Level8()
{
}

void AA_ArenaGameMode::Level9()
{
	// Blue Positions
	/* TArray<TArray<int>> Rock_pos_blue = {{0,0},{0,2},{1,2},{2,2},{2,1},{2,0}};
	TArray<TArray<int>> Laser_pos_blue = { {11,4,0}, {8,11,2} };
	TArray<TArray<int>> Blue_pos = { {6,7,0}, {9,0,0} };
	TArray<TArray<int>> Food_pos_blue = { {9,8,0} };
	TArray<TArray<int>> Movement_pos_blue = { {1,8},{9,1},{4,6},{7,10} };

	// Red Positions
	TArary<TArray<int>> Rock_pos_red = {};*/
}

void AA_ArenaGameMode::Level10()
{
	std::vector<std::vector<int>> Rock_pos_blue = {
		{1, 0, 0},{0 ,1 ,0},
		{1, 1, 0}, {1, 5, 0}, {1, 10, 0},
		{2, 0, 0}, {2, 1, 0}, {2, 2, 0}, {2, 5, 0}, {2, 10, 0},
		{3, 3, 0}, {3, 6, 0}, {3, 10, 0},
		{4, 4, 0}, {4, 5, 0}, {4, 7, 0}, {4, 10, 0}, {4, 11, 0}
	};
	std::vector<std::vector<int>> Rock_pos_red = {
		{1, 7, 0},
		{2, 2, 0}, {2, 3, 0}, {2, 4, 0}, {2, 7, 0},
		{3, 1, 0}, {3, 5, 0}, {3, 7, 0},
		{4, 1, 0}, {4, 3, 0}, {4, 5, 0}, {4, 7, 0},
		{5, 4, 0}, {5, 5, 0}, {5, 6, 0}, {5, 7, 0},
		{6, 2, 0}, {6, 3, 0}, {6, 10, 0}, {6, 11, 0},
		{7, 10, 0},
		{8, 10, 0},
		{9, 10, 0},
		{10, 10, 0}
	};
	TArray<TArray<int>> Food_pos_blue = { {0,0,0}, {11,4,0} };
	TArray<TArray<int>> Blue_pos = { {4,11,0}, {1,6,0} };
	TArray<TArray<int>> Red_pos = { {8,10,0}, {5,4,10} };

	// Movement TextBoxes
	ABasicParent* W_blue = Create_Dynamic_blue(2, 0, 11, nullptr, 0, 0);
	ABasicParent* S_blue = Create_Dynamic_blue(2, 1, 11, nullptr, 1, 0);
	ABasicParent* A_blue = Create_Dynamic_blue(2, 2, 11, nullptr, 2, 0);
	ABasicParent* D_blue = Create_Dynamic_blue(2, 3, 11, nullptr, 3, 0);

	// Dynamic for Red
	ABasicParent* W_red = Create_Dynamic_red(2, 1, 3, nullptr, 0, 0);
	ABasicParent* S_red = Create_Dynamic_red(2, 1, 5, nullptr, 1, 0);
	ABasicParent* A_red = Create_Dynamic_red(2, 1, 7, nullptr, 2, 0);
	ABasicParent* D_red = Create_Dynamic_red(2, 1, 9, nullptr, 3, 0);
}

void AA_ArenaGameMode::newLevel1()
{
	TArray<TArray<int>> Rock_pos_blue = {
		{11,11,0}, {4,6,0}, {6,4,0}, {6,8,0}, {8,6,0}
	};
	TArray<TArray<int>> Rock_pos_red = {
		{1,1,0}, {1,2,0}, {1,3,0}, {1,4,0}, {1,5,0}, {1,6,0}, {1,7,0}, {1,8,0}, {1,9,0}, {1,10,0} ,{1,11,0},
		{3,1,0}, {3,2,0}, {3,3,0}, {3,4,0}, {3,5,0}, {3,6,0}, {3,7,0}, {3,8,0}, {3,9,0}, {3,10,0} ,{3,0,0},
		{5,1,0}, {5,2,0}, {5,3,0}, {5,4,0}, {5,5,0}, {5,6,0}, {5,7,0}, {5,8,0}, {5,9,0}, {5,10,0} ,{5,11,0},
		{6,5,0}, {7,0,0}, {7,1,0}, {7,2,0}, {7,3,0}, {7,4,0}, {7,5,0}
	};
	
	ABasicParent* Rock_blue = CreateMany_Blue(Rock_pos_blue, 4, 10, nullptr);
	ABasicParent* Rock_red = CreateMany_Red(Rock_pos_red, 10, 10, Rock_blue);
	TArray<TArray<int>> Food_pos_red = { {11,0,0}, {6,4,0} };
	TArray<TArray<int>> Blue_pos = { {0,0,0}, {10,0,0} };
	TArray<TArray<int>> Red_pos = { {11,5,0}, {0,11,0} };

	//win  STOP
	Create_Static_red(11, 0, 4);
	Create_Static_blue(11, 11, 2); //correct

	//player 1 player 2 
	Create_Static_blue(0, 0, 6);
	Create_Static_red(11, 5, 7);

	Create_Static_blue(0, 11, 12);
	Create_Static_blue(1, 11, 13);
	Create_Static_blue(2, 11, 14);
	Create_Static_blue(3, 11, 15);

	Create_Static_red(8, 11, 8);
	Create_Static_red(9, 11, 9);
	Create_Static_red(10, 11, 10);
	Create_Static_red(11, 11, 11);

	// Movement TextBoxes
	ABasicParent* W_blue = Create_Dynamic_blue(2, 0, 11, nullptr, 0, 0);
	ABasicParent* S_blue = Create_Dynamic_blue(2, 1, 11, nullptr, 1, 0);
	ABasicParent* A_blue = Create_Dynamic_blue(2, 2, 11, nullptr, 2, 0);
	ABasicParent* D_blue = Create_Dynamic_blue(2, 3, 11, nullptr, 3, 0);

	// Dynamic for Red
	ABasicParent* W_red = Create_Dynamic_red(2, 8, 11, nullptr, 0, 0);
	ABasicParent* S_red = Create_Dynamic_red(2, 9, 11, nullptr, 1, 0);
	ABasicParent* A_red = Create_Dynamic_red(2, 10, 11, nullptr, 2, 0);
	ABasicParent* D_red = Create_Dynamic_red(2, 11, 11, nullptr, 3, 0);

	ABasicParent* Blue = CreateMany_Blue(Blue_pos, 7, 13, nullptr);
	ABasicParent* Red = CreateMany_Red(Red_pos, 8, 14, nullptr);
	ABasicParent* Food_red = CreateMany_Red(Food_pos_red, 5, 11, nullptr);

	currPlayer1 = Blue->Next; 
	currPlayer1->change_level(2); 
	currPlayer2 = Red->Next; 
	currPlayer2->change_level(2);
	Rock_blue->Next->change_level(3);
	Food_red->change_win(true);
	Func_Mapping[0] = { 1,2,3,4 };
	Func_Mapping[1] = { 1,2,3,4 };
}

void AA_ArenaGameMode::newLevel2()
{
	TArray<TArray<int>> Rock_pos_blue = {
		{11,11,0}, {11,0,0}, {7,7,0}, {6,8,0}
	};
	TArray<TArray<int>> Rock_pos_red = {
		{0,8,0}, {1,8,0}, {2,8,0}, {1,7,0}, {8,5,0}
	};

	ABasicParent* Rock_blue = CreateMany_Blue(Rock_pos_blue, 4, 10, nullptr);
	ABasicParent* Rock_red = CreateMany_Red(Rock_pos_red, 10, 10, Rock_blue);
	TArray<TArray<int>> Laser_pos_blue = { {0,4,0}, {2,11,2},{9,0,1} };
	TArray<TArray<int>> Laser_pos_red = { {2,10,4} };
	TArray<TArray<int>> Food_pos_red = { {0,1,0}, {11,11,0} };
	TArray<TArray<int>> Blue_pos = { {0,11,0}, {6,7,0} };
	TArray<TArray<int>> Red_pos = { {0,0,0}, {0,7,0} };

	//win  STOP
	Create_Static_red(0, 1, 4);
	Create_Static_blue(11, 11, 2); //correct

	//player 1 player 2 
	Create_Static_blue(0, 11, 6);
	Create_Static_red(0, 0, 7);

	Create_Static_blue(0, 3, 12);
	Create_Static_blue(0, 1, 13);
	Create_Static_blue(0, 2, 14);
	Create_Static_blue(0, 0, 15);

	Create_Static_red(8, 0, 8);
	Create_Static_red(9, 0, 9);
	Create_Static_red(10, 0, 10);
	Create_Static_red(11, 0, 11);

	// Movement TextBoxes
	ABasicParent* W_blue = Create_Dynamic_blue(2, 0, 3, nullptr, 0, 0);
	ABasicParent* S_blue = Create_Dynamic_blue(2, 0, 2, nullptr, 1, 0);
	ABasicParent* A_blue = Create_Dynamic_blue(2, 0, 1, nullptr, 2, 0);
	ABasicParent* D_blue = Create_Dynamic_blue(2, 0, 0, nullptr, 3, 0);

	// Dynamic for Red
	ABasicParent* W_red = Create_Dynamic_red(2, 8, 0, nullptr, 0, 0);
	ABasicParent* S_red = Create_Dynamic_red(2, 9, 0, nullptr, 1, 0);
	ABasicParent* A_red = Create_Dynamic_red(2, 10, 0, nullptr, 2, 0);
	ABasicParent* D_red = Create_Dynamic_red(2, 11, 0, nullptr, 3, 0);

	ABasicParent* Blue = CreateMany_Blue(Blue_pos, 7, 13, nullptr);
	ABasicParent* Red = CreateMany_Red(Red_pos, 8, 14, nullptr);
	ABasicParent* Food_red = CreateMany_Red(Food_pos_red, 5, 11, nullptr);
	ABasicParent* Laser_blue = CreateMany_Blue(Laser_pos_blue, 6, 12, nullptr);
	ABasicParent* Laser_red = CreateMany_Red(Laser_pos_red, 12, 12, Laser_blue);

	currPlayer1 = Blue->Next;
	currPlayer1->change_level(2);
	currPlayer2 = Red->Next;
	currPlayer2->change_level(2);
	Rock_blue->Next->change_level(3);
	Food_red->change_win(true);
	Func_Mapping[0] = { 1,2,3,4 };
	Func_Mapping[1] = { 1,2,3,4 };
}

void AA_ArenaGameMode::newLevel3()
{
	TArray<TArray<int>> Rock_pos_blue = {
		{2,8,0}, {2,9,0}, {2,10,0}, {3,7,0}, {4,7,0}, {5,8,0}, {5,9,0}, {5,10,0}, {7,6,0}, {7,5,0}, {8,6,0}
	};
	TArray<TArray<int>> Rock_pos_red = {
		{11,0,0}, {0,0,0}, {6,0,0}, {6,4,0}, {6,6,0}, {6,5,0}, {5,7,0}, {4,8,0}, {3,9,0}
	};

	ABasicParent* Rock_red = CreateMany_Red(Rock_pos_red, 4, 10, nullptr);
	ABasicParent* Rock_blue = CreateMany_Blue(Rock_pos_blue, 10, 10, Rock_red);
	TArray<TArray<int>> Laser_pos_blue = { {4,4,2} };
	TArray<TArray<int>> Laser_pos_red = { {11,1,0}, {7,4,2} };
	TArray<TArray<int>> Food_pos_blue = { {0,0,0}, {3,8,0} };
	TArray<TArray<int>> Blue_pos = { {0,1,0}, {6,3,0} };
	TArray<TArray<int>> Red_pos = { {0,1,0}, {7,7,0} };
	TArray<TArray<int>> TNT_pos_blue = { {1,1,0}, {5,5,0}, {3,10,0}, {4,10,0}, {11,0,0} };

	//win  STOP
	Create_Static_blue(0, 0, 4);
	Create_Static_red(11, 0, 2); //correct

	//player 1 player 2 
	Create_Static_blue(0, 1, 6);
	Create_Static_red(0, 1, 7);

	Create_Static_blue(8, 11, 12);
	Create_Static_blue(9, 11, 13);
	Create_Static_blue(10, 11, 14);
	Create_Static_blue(11, 11, 15);

	Create_Static_red(8, 11, 8);
	Create_Static_red(9, 11, 9);
	Create_Static_red(10, 11, 10);
	Create_Static_red(11, 11, 11);

	// Movement TextBoxes
	ABasicParent* W_blue = Create_Dynamic_blue(2, 8, 11, nullptr, 0, 0);
	ABasicParent* S_blue = Create_Dynamic_blue(2, 9, 11, nullptr, 1, 0);
	ABasicParent* A_blue = Create_Dynamic_blue(2, 10, 11, nullptr, 2, 0);
	ABasicParent* D_blue = Create_Dynamic_blue(2, 11, 11, nullptr, 3, 0);

	// Dynamic for Red
	ABasicParent* W_red = Create_Dynamic_red(2, 8, 11, nullptr, 0, 0);
	ABasicParent* S_red = Create_Dynamic_red(2, 9, 11, nullptr, 1, 0);
	ABasicParent* A_red = Create_Dynamic_red(2, 10, 11, nullptr, 2, 0);
	ABasicParent* D_red = Create_Dynamic_red(2, 11, 11, nullptr, 3, 0);

	ABasicParent* Blue = CreateMany_Blue(Blue_pos, 7, 13, nullptr);
	ABasicParent* Red = CreateMany_Red(Red_pos, 8, 14, nullptr);
	ABasicParent* Food_blue = CreateMany_Blue(Food_pos_blue, 5, 11, nullptr);
	ABasicParent* Laser_red = CreateMany_Red(Laser_pos_red, 6, 12, nullptr);
	ABasicParent* Laser_blue = CreateMany_Blue(Laser_pos_blue, 12, 12, Laser_red);
	ABasicParent* TNT_blue = CreateMany_Blue(TNT_pos_blue, 9, 9, nullptr);

	currPlayer1 = Blue->Next;
	currPlayer1->change_level(2);
	currPlayer2 = Red->Next;
	currPlayer2->change_level(2);
	Rock_blue->Next->change_level(3);
	Food_blue->change_win(true);
	TNT_blue->change_level(2);
	Func_Mapping[0] = { 1,2,3,4 };
	Func_Mapping[1] = { 1,2,3,4 };
}

void AA_ArenaGameMode::newLevel4()
{
	// Blue Positions
	TArray<TArray<int>> Rock_pos_blue = {
		{3,3,0}, {4,3,0}, {5,3,0}, {6,3,0}, {7,3,0}, {8,3,0}, {3,4,0}, {3,5,0}, {3,6,0}, {3,7,0},
		{3,8,0}, {4,8,0}, {5,8,0}, {6,8,0}, {7,8,0}, {8,8,0}, {8,4,0}, {8,5,0}, {8,6,0}, {8,7,0}
	};
	// TArray<TArray<int>> Laser_pos_blue = {	};
	TArray<TArray<int>> Blue_pos = { {0,11,0}, {4,4,0} };
	TArray<TArray<int>> Food_pos_blue = { {0,0,0}, {11,0,0} };
	TArray<TArray<int>> Movement_pos_blue = { {8,11,0}, {9,11,0}, {10,11,0}, {11,11,0} };

	// Red Positions
	TArray<TArray<int>> Rock_pos_red = {
		{7,2,0}, {1,2,0}, {2,2,0}, {3,2,0}, {1,3,0}, {3,3,0}, {2,4,0}, {2,5,0}, {7,3,0}, {7,4,0}, {7,5,0}, {7,6,0}
	};
	// TArray<TArray<int>> Laser_pos_red = { {11,10,0}, {8,8,4}, {8,9,4}, {9,7,1}, {10,7,1} };
	TArray<TArray<int>> Red_pos = { {0,11,0}, {9,9,0} };
	// TArray<TArray<int>> Food_pos_red = { {11,11,11} };
	TArray<TArray<int>> Movement_pos_red = { {8,11,0}, {9,11,0}, {10,11,0}, {11,11,0} };
	// TArray<TArray<int>> TNT_pos_red = { {10,0,0}, {11,0,0}, {10,1,0}, {11,1,0} };

	// Dynamic for red
	ABasicParent* Rock_red = CreateMany_Red(Rock_pos_red, 4, 10, nullptr);
	// ABasicParent* Laser_red = CreateMany_Red(Laser_pos_red, 6, 12, Laser_blue);
	ABasicParent* Red = CreateMany_Red(Red_pos, 8, 14, nullptr);
	// ABasicParent* TNT_red = CreateMany_Red(TNT_pos_red, 9, 9, nullptr);
	// ABasicParent* Food_red = CreateMany_Red(Food_pos_red, 11, 11, Food_blue);

	// Dynamic for blue
	ABasicParent* Rock_blue = CreateMany_Blue(Rock_pos_blue, 10, 10, Rock_red);
	// ABasicParent* Laser_blue = CreateMany_Blue(Laser_pos_blue, 12, 12, nullptr);
	ABasicParent* Blue = CreateMany_Blue(Blue_pos, 7, 13, nullptr);
	ABasicParent* Food_blue = CreateMany_Blue(Food_pos_blue, 5, 11, nullptr);

	// Movement textboxes
	ABasicParent* W_blue = Create_Dynamic_blue(2, Movement_pos_blue[0][0], Movement_pos_blue[0][1], nullptr, 0, 0);
	ABasicParent* S_blue = Create_Dynamic_blue(2, Movement_pos_blue[1][0], Movement_pos_blue[1][1], nullptr, 1, 0);
	ABasicParent* A_blue = Create_Dynamic_blue(2, Movement_pos_blue[2][0], Movement_pos_blue[2][1], nullptr, 2, 0);
	ABasicParent* D_blue = Create_Dynamic_blue(2, Movement_pos_blue[3][0], Movement_pos_blue[3][1], nullptr, 3, 0);
	ABasicParent* W_red = Create_Dynamic_red(2, Movement_pos_red[0][0], Movement_pos_red[0][1], nullptr, 0, 0);
	ABasicParent* S_red = Create_Dynamic_red(2, Movement_pos_red[1][0], Movement_pos_red[1][1], nullptr, 1, 0);
	ABasicParent* A_red = Create_Dynamic_red(2, Movement_pos_red[2][0], Movement_pos_red[2][1], nullptr, 2, 0);
	ABasicParent* D_red = Create_Dynamic_red(2, Movement_pos_red[3][0], Movement_pos_red[3][1], nullptr, 3, 0);

	// Statics for blue
	Create_Static_blue(0, 11, 6);
	Create_Static_blue(0, 0, 4);
	Create_Static_blue(8, 11, 12);
	Create_Static_blue(9, 11, 13);
	Create_Static_blue(10, 11, 14);
	Create_Static_blue(11, 11, 15);

	// Statics for red
	Create_Static_red(0, 11, 7);
	Create_Static_red(7, 2, 2);
	Create_Static_red(2, 3, 1);
	// Create_Static_red(1, 2, 3);
	Create_Static_red(8, 11, 8);
	Create_Static_red(9, 11, 9);
	Create_Static_red(10, 11, 10);
	Create_Static_red(11, 11, 11);

	// Initialising the controls
	currPlayer1 = Blue->Next;
	currPlayer1->change_level(2);

	currPlayer2 = Red->Next;
	currPlayer2->change_level(2);
	Food_blue->Next->change_win(true);
	Rock_red->Next->change_level(3);
	//Rock_blue->Next->change_level(3);



	Func_Mapping[0] = { 1, 2, 3, 4 };
	Func_Mapping[1] = { 1, 2, 3, 4 };
}

void AA_ArenaGameMode::newLevel5()
{
	// Blue Positions
	TArray<TArray<int>> Rock_pos_blue = {
		{10,0,0},
		{2,9,0}, {3,9,0}, {4,9,0}, {5,9,0}, {3,8,0}, {4,7,0}, {5,6,0}, {6,6,0}, {7,6,0}, {8,6,0}, {7,7,0}, {6,8,0},
		{10,1,0}, {11,1,0}, {9,1,0}
	};
	// TArray<TArray<int>> Laser_pos_blue = { {3,7,3}, {3,8,3}, {6,0,4}, {6,1,4}, {6,2,4}, {6,3,4}, {7,5,2}, {8,5,2}, {9,5,2}, {10,5,2} };
	TArray<TArray<int>> Blue_pos = { {11,11,0}, {4,8,0} };
	// TArray<TArray<int>> Food_pos_blue = { {2,3,0}, {9,2,0} };
	TArray<TArray<int>> TNT_pos_blue = { {5,7,0} };
	TArray<TArray<int>> Movement_pos_blue = { {0,0,0}, {1,0,0}, {2,0,0}, {3,0,0} };

	// Red Positions
	TArray<TArray<int>> Rock_pos_red = { {11,0,0}, {9,0,0}, {9,1,0}, {9,2,0}, {9,3,0}, {10,3,0}, {11,3,0} };
	TArray<TArray<int>> Laser_pos_red = { {11,10,0}, {5,7,1} };
	TArray<TArray<int>> Red_pos = { {0,11,0}, {10,2,0} };
	TArray<TArray<int>> Food_pos_red = { {11,11,0}, {4,10,0} };
	TArray<TArray<int>> Movement_pos_red = { {0,1,0}, {0,0,0}, {1,0,0}, {2,0,0} };
	// TArray<TArray<int>> TNT_pos_red = { {10,0,0}, {11,0,0}, {10,1,0}, {11,1,0} };

	// Dynamic for blue
	ABasicParent* Rock_blue = CreateMany_Blue(Rock_pos_blue, 4, 10, nullptr);
	// ABasicParent* Laser_blue = CreateMany_Blue(Laser_pos_blue, 12, 12, nullptr);
	ABasicParent* Blue = CreateMany_Blue(Blue_pos, 7, 13, nullptr);
	ABasicParent* TNT_blue = CreateMany_Blue(TNT_pos_blue, 9, 9, nullptr);
	// ABasicParent* Food_blue = CreateMany_Blue(Food_pos_blue, 5, 11, nullptr);

	// Dynamic for red
	ABasicParent* Rock_red = CreateMany_Red(Rock_pos_red, 10, 10, Rock_blue);
	ABasicParent* Laser_red = CreateMany_Red(Laser_pos_red, 6, 12, nullptr);
	ABasicParent* Red = CreateMany_Red(Red_pos, 8, 14, nullptr);
	// ABasicParent* TNT_red = CreateMany_Red(TNT_pos_red, 9, 9, nullptr);
	ABasicParent* Food_red = CreateMany_Red(Food_pos_red, 5, 11, nullptr);

	// Movement textboxes
	ABasicParent* W_blue = Create_Dynamic_blue(2, Movement_pos_blue[0][0], Movement_pos_blue[0][1], nullptr, 0, 0);
	ABasicParent* S_blue = Create_Dynamic_blue(2, Movement_pos_blue[1][0], Movement_pos_blue[1][1], nullptr, 1, 0);
	ABasicParent* A_blue = Create_Dynamic_blue(2, Movement_pos_blue[2][0], Movement_pos_blue[2][1], nullptr, 2, 0);
	ABasicParent* D_blue = Create_Dynamic_blue(2, Movement_pos_blue[3][0], Movement_pos_blue[3][1], nullptr, 3, 0);
	ABasicParent* W_red = Create_Dynamic_red(2, Movement_pos_red[0][0], Movement_pos_red[0][1], nullptr, 0, 0);
	ABasicParent* S_red = Create_Dynamic_red(2, Movement_pos_red[1][0], Movement_pos_red[1][1], nullptr, 1, 0);
	ABasicParent* A_red = Create_Dynamic_red(2, Movement_pos_red[2][0], Movement_pos_red[2][1], nullptr, 2, 0);
	ABasicParent* D_red = Create_Dynamic_red(2, Movement_pos_red[3][0], Movement_pos_red[3][1], nullptr, 3, 0);

	// Statics for blue
	Create_Static_blue(11, 11, 6);
	// Create_Static_blue(2, 3, 4);
	Create_Static_blue(10, 0, 2);
	Create_Static_blue(11, 0, 1);
	Create_Static_blue(0, 0, 12);
	Create_Static_blue(1, 0, 13);
	Create_Static_blue(2, 0, 14);
	Create_Static_blue(3, 0, 15);

	// Statics for red
	Create_Static_red(0, 11, 7);
	Create_Static_red(11, 11, 4);
	// Create_Static_red(2, 2, 2);
	// Create_Static_red(5, 2, 1);
	// Create_Static_red(1, 2, 3);
	Create_Static_red(0, 1, 8);
	Create_Static_red(0, 0, 9);
	Create_Static_red(1, 0, 10);
	Create_Static_red(2, 0, 11);

	currPlayer1 = Blue->Next;
	currPlayer1->change_level(2);

	// Spawning player2 with level
	currPlayer2 = Red->Next;
	currPlayer2->change_level(2);

	Food_red->Next->change_win(true);
	Rock_blue->Next->change_level(3);
	// TNT_red->change_level(2);
	TNT_blue->change_level(2);
	TNT_blue->change_CanPass(true);

	Func_Mapping[0] = { 1, 2, 3, 4 };
	Func_Mapping[1] = { 1, 2, 3, 4 };
}

void AA_ArenaGameMode::newLevel6()
{
	// Blue Positions
	TArray<TArray<int>> Rock_pos_blue = {
		{0,0,0},
		{0,4,0}, {0,5,0}, {0,6,0}, {1,6,0}, {1,7,0}, {1,9,0}, {1,10,0}, {1,11,0}, {3,6,0}, {3,8,0}, {3,9,0}, {3,10,0},{3,11,0} , {4,6,0} ,{5,6,0} ,{6,6,0} ,{7,5,0},
		{6,4,0}, {5,4,0}, {5,3,0} , {4,3,0} ,{3,3,0} ,{2,3,0}
	};
	// TArray<TArray<int>> Laser_pos_blue = { {3,7,3}, {3,8,3}, {6,0,4}, {6,1,4}, {6,2,4}, {6,3,4}, {7,5,2}, {8,5,2}, {9,5,2}, {10,5,2} };
	TArray<TArray<int>> Blue_pos = { {0,11,0}, {11,0,0} };
	// TArray<TArray<int>> Food_pos_blue = { {2,3,0}, {9,2,0} };
	// TArray<TArray<int>> TNT_pos_blue = { {5,7,0} };
	TArray<TArray<int>> Movement_pos_blue = { {5,5,0}, {1,8,0}, {2,10,0}, {3,7,0} };
	TArray<TArray<int>> TNT_pos_blue = { {11,11,0} };

	// Red Positions
	TArray<TArray<int>> Rock_pos_red = { {7,2,0}, {7,3,0}, {7,4,0}, {7,1,0} };
	TArray<TArray<int>> Laser_pos_red = { {11,1,0}, {7,5,3} };
	TArray<TArray<int>> Red_pos = { {0,10,0}, {10,2,0} };
	TArray<TArray<int>> Food_pos_red = { {11,10,0}, {6,4,0} };
	TArray<TArray<int>> Movement_pos_red = { {0,0,0}, {11,11,0}, {0,11,0}, {11,0,0} };
	TArray<TArray<int>> TNT_pos_red = { {0,8,0}, {0,7,0}, {0,6,0} };

	// Dynamic for blue
	ABasicParent* Rock_blue = CreateMany_Blue(Rock_pos_blue, 4, 10, nullptr);
	// ABasicParent* Laser_blue = CreateMany_Blue(Laser_pos_blue, 12, 12, nullptr);
	ABasicParent* Blue = CreateMany_Blue(Blue_pos, 7, 13, nullptr);
	ABasicParent* TNT_blue = CreateMany_Blue(TNT_pos_blue, 9, 9, nullptr);
	// ABasicParent* Food_blue = CreateMany_Blue(Food_pos_blue, 5, 11, nullptr);

	// Dynamic for red
	ABasicParent* Rock_red = CreateMany_Red(Rock_pos_red, 10, 10, Rock_blue);
	ABasicParent* Laser_red = CreateMany_Red(Laser_pos_red, 6, 12, nullptr);
	ABasicParent* Red = CreateMany_Red(Red_pos, 8, 14, nullptr);
	ABasicParent* TNT_red = CreateMany_Red(TNT_pos_red, 9, 9, nullptr);
	ABasicParent* Food_red = CreateMany_Red(Food_pos_red, 5, 11, nullptr);

	// Movement textboxes
	ABasicParent* W_blue = Create_Dynamic_blue(2, Movement_pos_blue[0][0], Movement_pos_blue[0][1], nullptr, 0, 0);
	ABasicParent* S_blue = Create_Dynamic_blue(2, Movement_pos_blue[1][0], Movement_pos_blue[1][1], nullptr, 1, 0);
	ABasicParent* A_blue = Create_Dynamic_blue(2, Movement_pos_blue[2][0], Movement_pos_blue[2][1], nullptr, 2, 0);
	ABasicParent* D_blue = Create_Dynamic_blue(2, Movement_pos_blue[3][0], Movement_pos_blue[3][1], nullptr, 3, 0);
	ABasicParent* W_red = Create_Dynamic_red(2, Movement_pos_red[0][0], Movement_pos_red[0][1], nullptr, 0, 0);
	ABasicParent* S_red = Create_Dynamic_red(2, Movement_pos_red[1][0], Movement_pos_red[1][1], nullptr, 1, 0);
	ABasicParent* A_red = Create_Dynamic_red(2, Movement_pos_red[2][0], Movement_pos_red[2][1], nullptr, 2, 0);
	ABasicParent* D_red = Create_Dynamic_red(2, Movement_pos_red[3][0], Movement_pos_red[3][1], nullptr, 3, 0);

	// Statics for blue
	Create_Static_blue(0, 11, 6);
	// Create_Static_blue(2, 3, 4);
	Create_Static_blue(0, 0, 2);
	Create_Static_blue(1, 0, 1);
	Create_Static_blue(6, 5, 12);
	Create_Static_blue(0, 8, 13);
	Create_Static_blue(2, 11, 14);
	Create_Static_blue(4, 7, 15);

	// Statics for red
	Create_Static_red(0, 10, 7);
	Create_Static_red(11, 10, 4);
	// Create_Static_red(2, 2, 2);
	// Create_Static_red(5, 2, 1);
	// Create_Static_red(1, 2, 3);
	Create_Static_red(0, 0, 8);
	Create_Static_red(11, 11, 9);
	Create_Static_red(0, 11, 10);
	Create_Static_red(11, 0, 11);

	currPlayer1 = Blue->Next;
	currPlayer1->change_level(2);

	// Spawning player2 with level
	currPlayer2 = Red->Next;
	currPlayer2->change_level(2);

	Food_red->Next->change_win(true);
	Rock_blue->Next->change_level(3);
	TNT_red->change_level(2);
	TNT_red->change_CanPass(true);
	TNT_blue->change_level(2);
	TNT_blue->change_CanPass(true);

	Func_Mapping[0] = { 1, 2, 3, 4 };
	Func_Mapping[1] = { 0, 0, 0, 0 };
}

void AA_ArenaGameMode::newLevel7()
{
	// Blue Positions
	TArray<TArray<int>> Rock_pos_blue = { {0,2,0}, {1,0,0}, {1,1,0}, {1,2,0}, {6,5,0}, {7,4,0}, {7,7,0}, {8,8,0} };
	TArray<TArray<int>> Laser_pos_blue = { {10,11,0}, {4,8,1} };
	TArray<TArray<int>> Blue_pos = { {11,11,0}, {6,6,0} };
	// TArray<TArray<int>> Food_pos_blue = { {2,3,0}, {9,2,0} };
	// TArray<TArray<int>> TNT_pos_blue = { {5,7,0} };
	TArray<TArray<int>> Movement_pos_blue = { {11,0,0} , {10,1,0}, {0,11,0}, {1,10,0} };

	// Red Positions
	TArray<TArray<int>> Rock_pos_red = { {7,9,0}, {7,10,0}, {7,8,0}, {8,8,0}, {8,10,0}, {9,8,0}, {9,9,0}, {9,10,0} };
	TArray<TArray<int>> Laser_pos_red = { {3,6,2}, {5,2,4} };
	TArray<TArray<int>> Red_pos = { {4,0,0}, {2,0,0} };
	TArray<TArray<int>> Food_pos_red = { {11,10,0}, {8,9,0} };
	TArray<TArray<int>> Movement_pos_red = { {0,0,0}, {11,11,0}, {0,11,0}, {11,0,0} };
	// TArray<TArray<int>> TNT_pos_red = { {0,8,0}, {0,7,0}, {0,6,0}, {0,5,0} };

	// Dynamic for blue
	ABasicParent* Rock_blue = CreateMany_Blue(Rock_pos_blue, 4, 10, nullptr);
	ABasicParent* Laser_blue = CreateMany_Blue(Laser_pos_blue, 6, 12, nullptr);
	ABasicParent* Blue = CreateMany_Blue(Blue_pos, 7, 13, nullptr);
	// ABasicParent* TNT_blue = CreateMany_Blue(TNT_pos_blue, 9, 9, nullptr);
	// ABasicParent* Food_blue = CreateMany_Blue(Food_pos_blue, 5, 11, nullptr);

	// Dynamic for red
	ABasicParent* Rock_red = CreateMany_Red(Rock_pos_red, 10, 10, Rock_blue);
	ABasicParent* Laser_red = CreateMany_Red(Laser_pos_red, 12, 12, Laser_blue);
	ABasicParent* Red = CreateMany_Red(Red_pos, 8, 14, nullptr);
	// ABasicParent* TNT_red = CreateMany_Red(TNT_pos_red, 9, 9, nullptr);
	ABasicParent* Food_red = CreateMany_Red(Food_pos_red, 5, 11, nullptr);

	// Movement textboxes
	ABasicParent* W_blue = Create_Dynamic_blue(2, Movement_pos_blue[0][0], Movement_pos_blue[0][1], nullptr, 0, 0);
	ABasicParent* S_blue = Create_Dynamic_blue(2, Movement_pos_blue[1][0], Movement_pos_blue[1][1], nullptr, 1, 0);
	ABasicParent* A_blue = Create_Dynamic_blue(2, Movement_pos_blue[2][0], Movement_pos_blue[2][1], nullptr, 2, 0);
	ABasicParent* D_blue = Create_Dynamic_blue(2, Movement_pos_blue[3][0], Movement_pos_blue[3][1], nullptr, 3, 0);
	ABasicParent* W_red = Create_Dynamic_red(2, Movement_pos_red[0][0], Movement_pos_red[0][1], nullptr, 0, 0);
	ABasicParent* S_red = Create_Dynamic_red(2, Movement_pos_red[1][0], Movement_pos_red[1][1], nullptr, 1, 0);
	ABasicParent* A_red = Create_Dynamic_red(2, Movement_pos_red[2][0], Movement_pos_red[2][1], nullptr, 2, 0);
	ABasicParent* D_red = Create_Dynamic_red(2, Movement_pos_red[3][0], Movement_pos_red[3][1], nullptr, 3, 0);

	// Statics for blue
	Create_Static_blue(11, 11, 6);
	// Create_Static_blue(2, 3, 4);
	Create_Static_blue(0, 1, 2);
	Create_Static_blue(0, 2, 2);
	Create_Static_blue(0, 0, 1);
	Create_Static_blue(10, 11, 1);
	Create_Static_blue(11, 0, 12);
	Create_Static_blue(10, 1, 13);
	Create_Static_blue(1, 10, 14);
	Create_Static_blue(0, 11, 15);

	// Statics for red
	Create_Static_red(4, 0, 7);
	Create_Static_red(11, 10, 4);
	// Create_Static_red(2, 2, 2);
	// Create_Static_red(5, 2, 1);
	// Create_Static_red(1, 2, 3);
	Create_Static_red(0, 0, 8);
	Create_Static_red(11, 11, 9);
	Create_Static_red(0, 11, 10);
	Create_Static_red(11, 0, 11);

	currPlayer1 = Blue->Next;
	currPlayer1->change_level(2);

	// Spawning player2 with level
	currPlayer2 = Red->Next;
	currPlayer2->change_level(2);

	Food_red->Next->change_win(true);
	Rock_blue->Next->change_level(3);
	Laser_blue->Next->change_level(2);

	Func_Mapping[0] = { 1, 2, 3, 4 };
	Func_Mapping[1] = { 1, 2, 4, 3 };
}

void AA_ArenaGameMode::newLevel8()
{
	// Blue Positions
	TArray<TArray<int>> Rock_pos_blue = { {5,4,0}, {6,4,0}, {7,4,0}, {5,5,0}, {5,6,0}, {5,7,0}, {5,8,0}, {5,9,0}, {6,9,0}, {7,9,0} };
	TArray<TArray<int>> Laser_pos_blue = { {8,11,2}, {4,4,1}, {3,10,4}, {9,3,3} };
	TArray<TArray<int>> Blue_pos = { {0,1,0}, {7,7,0} };
	TArray<TArray<int>> Food_pos_blue = { {11,0,0}, {1,1,0} };
	// TArray<TArray<int>> TNT_pos_blue = { {5,7,0} };
	TArray<TArray<int>> Movement_pos_blue = { {0,11,0} , {1,0,0}, {0,0,0}, {0,10,0} };

	// Red Positions
	TArray<TArray<int>> Rock_pos_red = { {4,9,10},
		{3,0,0}, {3,1,0}, {3,2,0}, {2,7,0}, {3,7,0}, {3,9,0},
		{4,0,0}, {4,6,0},
		{5,0,0}, {5,1,0}, {5,2,0}, {5,3,0}, {5,4,0}, {5,5,0}, {5,10,0},
		{8,9,0}
	};
	TArray<TArray<int>> Laser_pos_red = { {0,10,0}, {6,5,4}, {8,11,3} };
	TArray<TArray<int>> Red_pos = { {0,0,0}, {4,10,0} };
	// TArray<TArray<int>> Food_pos_red = { {11,10,0}, {8,9,0} };
	TArray<TArray<int>> Movement_pos_red = { {8,0,0}, {9,0,0}, {10,0,0}, {11,0,0} };
	TArray<TArray<int>> TNT_pos_red = { {0,3,0}, {0,4,0}, {1,3,0}, {1,4,0} };

	// Dynamic for blue
	ABasicParent* Blue = CreateMany_Blue(Blue_pos, 7, 13, nullptr);
	// ABasicParent* TNT_blue = CreateMany_Blue(TNT_pos_blue, 9, 9, nullptr);
	ABasicParent* Food_blue = CreateMany_Blue(Food_pos_blue, 5, 11, nullptr);

	ABasicParent* Laser_red = CreateMany_Red(Laser_pos_red, 6, 12, nullptr);
	ABasicParent* Laser_blue = CreateMany_Blue(Laser_pos_blue, 12, 12, Laser_red);

	ABasicParent* Rock_red = CreateMany_Red(Rock_pos_red, 4, 10, nullptr);
	ABasicParent* Rock_blue = CreateMany_Blue(Rock_pos_blue, 10, 10, Rock_red);

	// Dynamic for red
	ABasicParent* Red = CreateMany_Red(Red_pos, 8, 14, nullptr);
	ABasicParent* TNT_red = CreateMany_Red(TNT_pos_red, 9, 9, nullptr);
	// ABasicParent* Food_red = CreateMany_Red(Food_pos_red, 5, 11, nullptr);

	// Movement textboxes
	ABasicParent* W_blue = Create_Dynamic_blue(2, Movement_pos_blue[0][0], Movement_pos_blue[0][1], nullptr, 0, 0);
	ABasicParent* S_blue = Create_Dynamic_blue(2, Movement_pos_blue[1][0], Movement_pos_blue[1][1], nullptr, 1, 0);
	ABasicParent* A_blue = Create_Dynamic_blue(2, Movement_pos_blue[2][0], Movement_pos_blue[2][1], nullptr, 2, 0);
	ABasicParent* D_blue = Create_Dynamic_blue(2, Movement_pos_blue[3][0], Movement_pos_blue[3][1], nullptr, 3, 0);
	ABasicParent* W_red = Create_Dynamic_red(2, Movement_pos_red[0][0], Movement_pos_red[0][1], nullptr, 0, 0);
	ABasicParent* S_red = Create_Dynamic_red(2, Movement_pos_red[1][0], Movement_pos_red[1][1], nullptr, 1, 0);
	ABasicParent* A_red = Create_Dynamic_red(2, Movement_pos_red[2][0], Movement_pos_red[2][1], nullptr, 2, 0);
	ABasicParent* D_red = Create_Dynamic_red(2, Movement_pos_red[3][0], Movement_pos_red[3][1], nullptr, 3, 0);

	// Statics for blue
	Create_Static_blue(0, 1, 6);
	Create_Static_blue(11, 0, 4);
	// Create_Static_blue(0, 1, 2);
	// Create_Static_blue(0, 2, 2);
	// Create_Static_blue(0, 0, 1);
	// Create_Static_blue(10, 11, 1);
	Create_Static_blue(0, 11, 12);
	Create_Static_blue(1, 0, 13);
	Create_Static_blue(0, 0, 14);
	Create_Static_blue(0, 10, 15);

	// Statics for red
	Create_Static_red(0, 0, 7);
	// Create_Static_red(11, 10, 4);
	Create_Static_red(4, 9, 2);
	Create_Static_red(0, 10, 1);
	Create_Static_red(4, 7, 1);
	// Create_Static_red(1, 2, 3);
	Create_Static_red(8, 0, 8);
	Create_Static_red(9, 0, 9);
	Create_Static_red(10, 0, 10);
	Create_Static_red(11, 0, 11);

	currPlayer1 = Blue->Next;
	currPlayer1->change_level(2);

	// Spawning player2 with level
	currPlayer2 = Red->Next;
	currPlayer2->change_level(2);

	Food_blue->Next->change_win(true);
	Rock_red->Next->change_level(3);
	TNT_red->change_level(2);
	TNT_red->change_CanPass(true);
	Laser_red->Next->change_level(2);

	Func_Mapping[0] = { 1, 2, 3, 4 };
	Func_Mapping[1] = { 1, 2, 3, 4 };
}

void AA_ArenaGameMode::newLevel9()
{
	// Blue Positions
	TArray<TArray<int>> Rock_pos_blue = { {11,11,0}, {6,0,0}, {6,1,0}, {6,2,0}, {7,1,0}, {7,2,0}, {8,1,0}, {8,2,0}, {9,1,0}, {9,2,0}, {10,1,0}, {10,2,0}, {11,1,0}, {11,2,0}, {9,7,0},{10,7,0}, {11,7,0} };
	TArray<TArray<int>> Laser_pos_blue = { {6,7,0}, {3,5,2} };
	TArray<TArray<int>> Blue_pos = { {7,7,0}, {5,7,0} };
	TArray<TArray<int>> Food_pos_blue = { {8,0,0} };
	// TArray<TArray<int>> TNT_pos_blue = { {5,7,0} };
	TArray<TArray<int>> Movement_pos_blue = { {0,11,0}, {1,11,0}, {2,11,0}, {3,11,0} };

	// Red Positions
	// TArray<TArray<int>> Rock_pos_red = { {0,5,0}, {1,5,0}, {2,5,0}, {3,5,0}, {4,5,0}, {5,5,0}, {6,5,0}, {7,5,0}, {8,5,0}, {9,5,0}, {10,5,0}, {11,5,0} };
	// TArray<TArray<int>> Laser_pos_red = { {3,6,2}, {5,2,4} };
	TArray<TArray<int>> Red_pos = { {11,0,0}, {5,7,0} };
	TArray<TArray<int>> Food_pos_red = { {6,7,0} };
	TArray<TArray<int>> Movement_pos_red = { {0,11,0}, {1,11,0}, {2,11,0}, {3,11,0} };
	// TArray<TArray<int>> TNT_pos_red = { {0,8,0}, {0,7,0}, {0,6,0}, {0,5,0} };

	// Dynamic for blue
	ABasicParent* Rock_blue = CreateMany_Blue(Rock_pos_blue, 4, 10, nullptr);
	ABasicParent* Laser_blue = CreateMany_Blue(Laser_pos_blue, 6, 12, nullptr);
	ABasicParent* Blue = CreateMany_Blue(Blue_pos, 7, 13, nullptr);
	// ABasicParent* TNT_blue = CreateMany_Blue(TNT_pos_blue, 9, 9, nullptr);
	ABasicParent* Food_blue = CreateMany_Blue(Food_pos_blue, 5, 11, nullptr);

	// Dynamic for red
	// ABasicParent* Rock_red = CreateMany_Red(Rock_pos_red, 10, 10, Rock_blue);
	// ABasicParent* Laser_red = CreateMany_Red(Laser_pos_red, 12, 12, Laser_blue);
	ABasicParent* Red = CreateMany_Red(Red_pos, 8, 14, nullptr);
	// ABasicParent* TNT_red = CreateMany_Red(TNT_pos_red, 9, 9, nullptr);
	ABasicParent* Food_red = CreateMany_Red(Food_pos_red, 11, 11, Food_blue);

	// Movement textboxes
	ABasicParent* W_blue = Create_Dynamic_blue(2, Movement_pos_blue[0][0], Movement_pos_blue[0][1], nullptr, 0, 0);
	ABasicParent* S_blue = Create_Dynamic_blue(2, Movement_pos_blue[1][0], Movement_pos_blue[1][1], nullptr, 1, 0);
	ABasicParent* A_blue = Create_Dynamic_blue(2, Movement_pos_blue[2][0], Movement_pos_blue[2][1], nullptr, 2, 0);
	ABasicParent* D_blue = Create_Dynamic_blue(2, Movement_pos_blue[3][0], Movement_pos_blue[3][1], nullptr, 3, 0);
	ABasicParent* W_red = Create_Dynamic_red(2, Movement_pos_red[0][0], Movement_pos_red[0][1], nullptr, 0, 0);
	ABasicParent* S_red = Create_Dynamic_red(2, Movement_pos_red[1][0], Movement_pos_red[1][1], nullptr, 1, 0);
	ABasicParent* A_red = Create_Dynamic_red(2, Movement_pos_red[2][0], Movement_pos_red[2][1], nullptr, 2, 0);
	ABasicParent* D_red = Create_Dynamic_red(2, Movement_pos_red[3][0], Movement_pos_red[3][1], nullptr, 3, 0);

	// Statics for blue
	Create_Static_blue(7, 7, 6);
	Create_Static_blue(8, 7, 1);
	Create_Static_blue(9, 0, 4);
	Create_Static_blue(11, 11, 2);
	Create_Static_blue(8, 0, 5);
	// Create_Static_blue(0, 2, 2);
	// Create_Static_blue(0, 0, 1);
	// Create_Static_blue(10, 11, 1);
	Create_Static_blue(0, 11, 12);
	Create_Static_blue(1, 11, 13);
	Create_Static_blue(2, 11, 14);
	Create_Static_blue(3, 11, 15);

	// Statics for red
	Create_Static_red(11, 0, 7);
	// Create_Static_red(11, 10, 4);
	// Create_Static_red(2, 2, 2);
	// Create_Static_red(5, 2, 1);
	// Create_Static_red(1, 2, 3);
	Create_Static_red(0, 11, 8);
	Create_Static_red(1, 11, 9);
	Create_Static_red(2, 11, 10);
	Create_Static_red(3, 11, 11);

	currPlayer1 = Blue->Next;
	currPlayer1->change_level(2);

	// Spawning player2 with level
	currPlayer2 = Red->Next;
	currPlayer2->change_level(2);

	//Food_blue->Next->change_win(true);
	Rock_blue->Next->change_level(3);
	Food_blue->Next->change_defeat(true);
	//Laser_blue->Next->change_level(2);

	Func_Mapping[0] = { 1, 2, 3, 4 };
	Func_Mapping[1] = { 1, 2, 3, 4 };
}

void AA_ArenaGameMode::newLevel10()
{
	// Blue Positions
	TArray<TArray<int>> Rock_pos_blue = { {3,7,0}, {5,8,0}, {0,11,0}, {1,11,0}, {2,11,0}, {7,0,0}, {7,1,0}, {7,2,0}, {7,3,0} };
	TArray<TArray<int>> Laser_pos_blue = { {7,8,0}, {6,8,2} };
	TArray<TArray<int>> Blue_pos = { {4,8,0}, {9,2,0} };
	TArray<TArray<int>> Food_pos_blue = { {8,8,0} };
	TArray<TArray<int>> TNT_pos_blue = { {0,10,0}, {9,8,0} };
	TArray<TArray<int>> Movement_pos_blue = { {0,0,0} , {1,0,0}, {2,0,0}, {3,0,0} };

	// Red Positions
	TArray<TArray<int>> Rock_pos_red = { {10,2,0}, {0,10,0}, {0,11,0}, {10,11,0}, {11,10,0}, {11,11,0} };
	TArray<TArray<int>> Laser_pos_red = { {4,5,2}, {5,4,3} };
	TArray<TArray<int>> Red_pos = { {11,0,0}, {7,10,0} };
	TArray<TArray<int>> Food_pos_red = { {0,0,0} };
	TArray<TArray<int>> Movement_pos_red = { {0,6,0}, {0,7,0}, {0,8,0}, {0,9,0} };
	TArray<TArray<int>> TNT_pos_red = { {11,9,0}, {11,8,0} };

	// Dynamic for blue
	ABasicParent* Rock_blue = CreateMany_Blue(Rock_pos_blue, 4, 10, nullptr);
	ABasicParent* Laser_blue = CreateMany_Blue(Laser_pos_blue, 6, 12, nullptr);
	ABasicParent* Blue = CreateMany_Blue(Blue_pos, 7, 13, nullptr);
	ABasicParent* TNT_blue = CreateMany_Blue(TNT_pos_blue, 9, 9, nullptr);
	ABasicParent* Food_blue = CreateMany_Blue(Food_pos_blue, 5, 11, nullptr);

	// Dynamic for red
	ABasicParent* Rock_red = CreateMany_Red(Rock_pos_red, 4, 10, Rock_blue);
	ABasicParent* Laser_red = CreateMany_Red(Laser_pos_red, 12, 12, Laser_blue);
	ABasicParent* Red = CreateMany_Red(Red_pos, 8, 14, nullptr);
	ABasicParent* TNT_red = CreateMany_Red(TNT_pos_red, 9, 9, nullptr);
	ABasicParent* Food_red = CreateMany_Red(Food_pos_red, 11, 11, Food_blue);

	// Movement textboxes
	ABasicParent* W_blue = Create_Dynamic_blue(2, Movement_pos_blue[0][0], Movement_pos_blue[0][1], nullptr, 0, 0);
	ABasicParent* S_blue = Create_Dynamic_blue(2, Movement_pos_blue[1][0], Movement_pos_blue[1][1], nullptr, 1, 0);
	ABasicParent* A_blue = Create_Dynamic_blue(2, Movement_pos_blue[2][0], Movement_pos_blue[2][1], nullptr, 2, 0);
	ABasicParent* D_blue = Create_Dynamic_blue(2, Movement_pos_blue[3][0], Movement_pos_blue[3][1], nullptr, 3, 0);
	ABasicParent* W_red = Create_Dynamic_red(2, Movement_pos_red[0][0], Movement_pos_red[0][1], nullptr, 0, 0);
	ABasicParent* S_red = Create_Dynamic_red(2, Movement_pos_red[1][0], Movement_pos_red[1][1], nullptr, 1, 0);
	ABasicParent* A_red = Create_Dynamic_red(2, Movement_pos_red[2][0], Movement_pos_red[2][1], nullptr, 2, 0);
	ABasicParent* D_red = Create_Dynamic_red(2, Movement_pos_red[3][0], Movement_pos_red[3][1], nullptr, 3, 0);

	// Statics for blue
	Create_Static_blue(4, 8, 6);
	Create_Static_blue(8, 8, 4);
	// Create_Static_blue(0, 1, 2);
	// Create_Static_blue(0, 2, 2);
	// Create_Static_blue(0, 0, 1);
	// Create_Static_blue(10, 11, 1);
	Create_Static_blue(0, 0, 12);
	Create_Static_blue(1, 0, 13);
	Create_Static_blue(2, 0, 14);
	Create_Static_blue(3, 0, 15);

	// Statics for red
	Create_Static_red(11, 0, 7);
	// Create_Static_red(11, 10, 4);
	// Create_Static_red(2, 2, 2);
	Create_Static_red(9, 2, 1);
	// Create_Static_red(1, 2, 3);
	Create_Static_red(0, 6, 8);
	Create_Static_red(0, 7, 9);
	Create_Static_red(0, 8, 10);
	Create_Static_red(0, 9, 11);

	currPlayer1 = Blue->Next;
	currPlayer1->change_level(2);

	// Spawning player2 with level
	currPlayer2 = Red->Next;
	currPlayer2->change_level(2);

	Food_blue->Next->change_win(true);
	Rock_blue->Next->change_level(3);
	//Laser_blue->Next->change_level(2);
	TNT_blue->change_level(2);
	TNT_red->change_level(2);
	TNT_blue->change_CanPass(true);
	TNT_red->change_CanPass(true);

	Func_Mapping[0] = { 1, 2, 3, 4 };
	Func_Mapping[1] = { 1, 2, 3, 4 };
}

void AA_ArenaGameMode::Initialise_laser_light_blue(int x, int y)
{
	ABasicParent* Laser = BlueTile[x][y]->ObjectVector[1];
	if (Laser->LaserDirection == 1) {
		for (int i = y + 1; i < 12; i++) {
			// Updating whether there is laser or not
			BlueTile[x][i]->bIsInLaser = true;
			laser_material_blue(x, i, 16);
		}
	}
	else if (Laser->LaserDirection == 2) {
		for (int i = y - 1; i >= 0; i--) {
			// Updating whether there is laser or not
			BlueTile[x][i]->bIsInLaser = true;
			laser_material_blue(x, i, 16);
		}
	}
	else if (Laser->LaserDirection == 3) {
		for (int i = x - 1; i >= 0; i--) {
			// Updating whether there is laser or not
			BlueTile[i][y]->bIsInLaser = true;
			laser_material_blue(i, y, 16);
		}
	}
	else if (Laser->LaserDirection == 4) {
		for (int i = x + 1; i < 12; i++) {
			// Updating whether there is laser or not
			BlueTile[i][y]->bIsInLaser = true;
			laser_material_blue(i, y, 16);
		}
	}
}

void AA_ArenaGameMode::Initialise_laser_light_red(int x, int y)
{
	ABasicParent* Laser = RedTile[x][y]->ObjectVector[1];
	if (Laser->LaserDirection == 1) {
		for (int i = y + 1; i < 12; i++) {
			// Updating whether there is laser or not
			RedTile[x][i]->bIsInLaser = true;
			laser_material_red(x, i, 16);
		}
	}
	else if (Laser->LaserDirection == 2) {
		for (int i = y - 1; i >= 0; i--) {
			// Updating whether there is laser or not
			RedTile[x][i]->bIsInLaser = true;
			laser_material_red(x, i, 16);
		}
	}
	else if (Laser->LaserDirection == 3) {
		for (int i = x - 1; i >= 0; i--) {
			// Updating whether there is laser or not
			RedTile[i][y]->bIsInLaser = true;
			laser_material_red(i, y, 16);
		}
	}
	else if (Laser->LaserDirection == 4) {
		for (int i = x + 1; i < 12; i++) {
			// Updating whether there is laser or not
			RedTile[i][y]->bIsInLaser = true;
			laser_material_red(i, y, 16);
		}
	}
}

void AA_ArenaGameMode::remove_laser_light_blue(ABasicParent* laser)
{
	int x = laser->x, y = laser->y;
	if (laser->LaserDirection == 1) {
		for (int i = y + 1; i < 12; i++) {
			if ((x <= 0 || BlueTile[x-1][i]->bIsInLaser == true) || (x >= 11 || BlueTile[x+1][i]->bIsInLaser == true))
				continue;
			BlueTile[x][i]->bIsInLaser = false;
			laser_material_blue(x, i, 0);
		}
	}
	else if (laser->LaserDirection == 2) {
		for (int i = y - 1; i >= 0; i--) {
			if ((x <= 0 || BlueTile[x - 1][i]->bIsInLaser == true) || (x >= 11 || BlueTile[x + 1][i]->bIsInLaser == true))
				continue;
			BlueTile[x][i]->bIsInLaser = false;
			laser_material_blue(x, i, 0);
		}
	}
	else if (laser->LaserDirection == 3) {
		for (int i = x - 1; i >= 0; i--) {
			if ((y <= 0 || BlueTile[i][y - 1]->bIsInLaser == true) || (y >= 11 || BlueTile[i][y + 1]->bIsInLaser == true))
				continue;
			BlueTile[i][y]->bIsInLaser = false;
			laser_material_blue(i, y, 0);
		}
	}
	else if (laser->LaserDirection == 4) {
		for (int i = x + 1; i < 12; i++) {
			if ((y <= 0 || BlueTile[i][y - 1]->bIsInLaser == true) || (y >= 11 || BlueTile[i][y + 1]->bIsInLaser == true))
				continue;
			BlueTile[i][y]->bIsInLaser = false;
			laser_material_blue(i, y, 0);
		}
	}
}

void AA_ArenaGameMode::remove_laser_light_red(ABasicParent* laser)
{
	int x = laser->x, y = laser->y;
	if (laser->LaserDirection == 1) {
		for (int i = y + 1; i < 12; i++) {
			if ((x <= 0 || RedTile[x - 1][i]->bIsInLaser == true) || (x >= 11 || RedTile[x + 1][i]->bIsInLaser == true))
				continue;
			RedTile[x][i]->bIsInLaser = false;
			laser_material_red(x, i, 0);
		}
	}
	else if (laser->LaserDirection == 2) {
		for (int i = y - 1; i >= 0; i--) {
			if ((x <= 0 || RedTile[x - 1][i]->bIsInLaser == true) || (x >= 11 || RedTile[x + 1][i]->bIsInLaser == true))
				continue;
			RedTile[x][i]->bIsInLaser = false;
			laser_material_red(x, i, 0);
		}
	}
	else if (laser->LaserDirection == 3) {
		for (int i = x - 1; i >= 0; i--) {
			if ((y <= 0 || RedTile[i][y - 1]->bIsInLaser == true) || (y >= 11 || RedTile[i][y + 1]->bIsInLaser == true))
				continue;
			RedTile[i][y]->bIsInLaser = false;
			laser_material_red(i, y, 0);
		}
	}
	else if (laser->LaserDirection == 4) {
		for (int i = x + 1; i < 12; i++) {
			if ((y <= 0 || RedTile[i][y - 1]->bIsInLaser == true) || (y >= 11 || RedTile[i][y + 1]->bIsInLaser == true))
				continue;
			RedTile[i][y]->bIsInLaser = false;
			laser_material_red(i, y, 0);
		}
	}
}

void AA_ArenaGameMode::MovementSuccessfull()
{
	UGameplayStatics::PlaySound2D(GetWorld(), MovementSound );
	NoOfMoves++;
}
