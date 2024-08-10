// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "../KeyEnum.h"
#include "Functional"
#include "A_ArenaGameMode.generated.h"

class ABasicParent;
class AA_Tile;

DECLARE_DELEGATE_OneParam(MovementDelegate, ABasicParent*);

UCLASS()
class BREAKINGSYMMETRY_API AA_ArenaGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	class AA_PCArena* ArenPlayerController;

	void SpawnActor();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MyGameMode")
	TSubclassOf<AA_Tile> BPActorClass;

	std::vector<std::vector<AA_Tile*>> BlueTile;
	std::vector<std::vector<AA_Tile*>> RedTile;

	ABasicParent* currPlayer1;

	ABasicParent* currPlayer2;

	void Up(ABasicParent* currPlayer);
	void Down(ABasicParent* currPlayer);
	void left(ABasicParent* currPlayer);
	void right(ABasicParent* currPlayer);
	void do_nothing(ABasicParent* currPlayer);

	void update(int x, int y, bool blue, int Din_x, int Din_y);

	TArray<MovementDelegate> MovementDelegates;

	std::vector<std::vector<int>> Func_Mapping;

	// For action calling
	void check_static_red(int x, int y);
	void check_static_blue(int x, int y);

	// For reaction calling
	void check_reaction_red(ABasicParent* DynamicObject);
	void check_reaction_blue(ABasicParent* DynamicObject);

	void check_IsInLaser_red(int x, int y, int ObjectLevel);
	void check_IsInLaser_blue(int x, int y, int ObjectLevel);

	void laser_moved_red(int prev_x, int prev_y, int x, int y);
	void laser_moved_blue(int prev_x, int prev_y, int x, int y);

	bool TNT_blast_red(ABasicParent* TNT_blaster, int x, int y);
	bool TNT_blast_blue(ABasicParent* TNT_blaster, int x, int y);

	void win_blue(int prev_x, int prev_y, int x, int y);
	void win_red(int prev_x, int prev_y, int x, int y);

	void defeat_blue(int prev_x, int prev_y, int x, int y);
	void defeat_red(int prev_x, int prev_y, int x, int y);

	void laser_blue_to_red(int y);
	void laser_red_to_blue(int y);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class UMaterial*> StaticMaterialArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class UMaterial*> DynamicMaterialArray;

	void Update_Location_blue(ABasicParent* object, int x, int y);
	void Update_Location_red(ABasicParent* object, int x, int y);

	void laser_material_blue(int x, int y, int material_idx);
	void laser_material_red(int x, int y, int material_idx);

	//Creating subClass array for slecting dynamic actor 
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class ABasicParent>> ActorToSpawn;

	//Creating subClass array for selecting Static actors
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class AStaticTile>> StaticToSpawn;

	//creating function for creating dynamic objects;
	ABasicParent* Create_Dynamic_blue(int level, int x, int y, ABasicParent* prev_dynamic, int DynamicHeaderInt, int LaserDirection);
	ABasicParent* Create_Dynamic_red(int level, int x, int y, ABasicParent* prev_dynamic, int DynamicHeaderInt, int LaserDirection);


	ABasicParent* CreateMany_Blue(TArray<TArray<int>> positions, int DynHeaderText, int DynHeaderObject, ABasicParent* others);
	ABasicParent* CreateMany_Red(TArray<TArray<int>> positions, int DynHeaderText, int DynHeaderObject , ABasicParent* others);

	void Create_Static_blue(int x, int y, int StaticHeaderInt);
	void Create_Static_red(int x, int y, int StaticHeaderInt);

	// creating Levels Function
	void CreateDummyLevel();

	UPROPERTY(BlueprintReadWrite)
	int NoOfMoves=0;

	UFUNCTION(BlueprintCallable)
	void Level1();
	UFUNCTION(BlueprintCallable)
	void Level2();
	UFUNCTION(BlueprintCallable)
	void Level3();
	UFUNCTION(BlueprintCallable)
	void Level4();
	UFUNCTION(BlueprintCallable)
	void Level5();
	UFUNCTION(BlueprintCallable)
	void Level6();
	UFUNCTION(BlueprintCallable)
	void Level7();
	UFUNCTION(BlueprintCallable)
	void Level8();
	UFUNCTION(BlueprintCallable)
	void Level9();
	UFUNCTION(BlueprintCallable)
	void Level10();

	UFUNCTION(BlueprintCallable)
	void newLevel1();
	UFUNCTION(BlueprintCallable)
	void newLevel2();
	UFUNCTION(BlueprintCallable)
	void newLevel3();
	UFUNCTION(BlueprintCallable)
	void newLevel4();
	UFUNCTION(BlueprintCallable)
	void newLevel5();
	UFUNCTION(BlueprintCallable)
	void newLevel6();
	UFUNCTION(BlueprintCallable)
	void newLevel7();
	UFUNCTION(BlueprintCallable)
	void newLevel8();
	UFUNCTION(BlueprintCallable)
	void newLevel9();
	UFUNCTION(BlueprintCallable)
	void newLevel10();
	/*UFUNCTION(BlueprintCallable)
	void newLevel11();
	UFUNCTION(BlueprintCallable)
	void newLevel12();
	*/



	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void Win();

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void Loose();

	// Initialising laser
	void Initialise_laser_light_blue(int x, int y);
	void Initialise_laser_light_red(int x, int y);

	// Removing laser light
	void remove_laser_light_blue(ABasicParent* laser);
	void remove_laser_light_red(ABasicParent* laser);

	UPROPERTY(EditAnywhere, Category = "Audio")
	class USoundBase* TNTBlastSound;

	UPROPERTY(EditAnywhere, Category = "Audio")
	class USoundBase* LaserBlastSound;

	UPROPERTY(EditAnywhere, Category = "Audio")
	class USoundBase* MovementSound;

	void MovementSuccessfull();
};
