// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "../DynamicEnumHeader.h"
#include "I_ParentStatic.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UI_ParentStatic : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BREAKINGSYMMETRY_API II_ParentStatic
{
	GENERATED_BODY()
public:
	virtual void action(class ABasicParent* DynamicClass) = 0;
	virtual void reaction(ABasicParent* DynamicClass) = 0;
};
