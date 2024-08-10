// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../StaticTile.h"
#include "PassTile.generated.h"

/**
 * 
 */
UCLASS()
class BREAKINGSYMMETRY_API APassTile : public AStaticTile
{
	GENERATED_BODY()
public:
	virtual void action(ABasicParent* DynamicClass) override;
	virtual void reaction(ABasicParent* DynamicClass) override;
};
