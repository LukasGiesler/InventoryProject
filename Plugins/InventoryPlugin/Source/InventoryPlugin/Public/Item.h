// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	DEFAULT,
	BACKPACK,
	WEAPON,
	COSMETIC
};

UCLASS()
class INVENTORYPLUGIN_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Constructor
	AItem(const FObjectInitializer& ObjectInitializer);

	// Use Event
	UFUNCTION(BlueprintNativeEvent, Category = "Inventory|Item")
		void OnUse();

	// Thumbnail used in UI for this item
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inventory|Item")
		UTexture2D* ItemThumbnail;

	// Item Mesh Component
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inventory|Item")
	UStaticMeshComponent* ItemMesh;

	// Translateable Display name of this item
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inventory|Item")
		FText ItemName;

	// Translateable description displayed in UI
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inventory|Item")
		FText ItemDescription;

	// Sets how many of this item will be added to our inventory on pickup
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inventory|Item")
		int32 PickupAmount = 0;

	// Maximum size of an item stack
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inventory|Item")
		int32 ItemMaxAmount = 0;

	// Set weight of this item per item unit
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inventory|Item")
		int32 ItemWeight = 0;

	// Weight bonus applied when carrying this item
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inventory|Item")
		int32 WeightBonus = 0;

	// Sort priority (highest numerical priority = first item in list)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inventory|Item")
		int32 SortPriority = 0;

	// Set type of this item
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inventory|Item")
		EItemType Type = EItemType::DEFAULT;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	

};
