// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../StaticTile.h"
#include "WinTile.generated.h"

/**
 * 
 */
UCLASS()
class BREAKINGSYMMETRY_API AWinTile : public AStaticTile
{
	GENERATED_BODY()
public:
	virtual void action(ABasicParent* DynamicClass) override;
	virtual void reaction(ABasicParent* DynamicClass) override;
};
