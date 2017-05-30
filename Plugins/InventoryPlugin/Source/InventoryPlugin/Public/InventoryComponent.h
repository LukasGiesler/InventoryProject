// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Components/ActorComponent.h"
#include "Item.h"
#include "InventoryComponent.generated.h"

//
UENUM(BlueprintType)
enum class ESortMethod : uint8
{
	NAME,
	WEIGHT,
	AMOUNT,
	PRIORITY
};

// Represents one slot in the inventory
USTRUCT(BlueprintType)
struct FInventoryStruct
{
	GENERATED_BODY()

	// Thumbnail used in UI for this item
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Structure")
		UTexture2D* ItemThumbnail;

	// Item Actor Class which represents this item in the scene
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Structure")
		TSubclassOf<class AItem> ItemClass;

	// Translateable Display name of this item
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Structure")
		FText ItemName;

	// Translateable description displayed in UI
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inventory Structure")
		FText ItemDescription;

	// Amount of items on this slot
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Structure")
		int32 ItemAmount;

	// Maximum amount of items that can be on this slot
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Structure")
		int32 ItemMaxAmount;

	// Weight of this slot in "weight units"
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Structure")
		int32 ItemWeight;

	// Weight bonus slots gained from carrying this item
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Structure")
		int32 WeightBonus;

	// Unique identifier for this slot
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Structure")
		int32 UniqueID;

	// Sort priority used for default sorting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Structure")
		int32 SortPriority;

	// Set type of this item
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inventory|Item")
		EItemType ItemType = EItemType::DEFAULT;

	// Default Constructor
	FInventoryStruct()
	{
		ItemClass = NULL;
		ItemAmount = -1;
		ItemMaxAmount = -1;
		ItemWeight = -1;
		WeightBonus = -1;
		UniqueID = -1;
		SortPriority = 0;
		ItemType = EItemType::DEFAULT;
	}

	// Constructor
	FInventoryStruct(TSubclassOf<class AItem> InItemClass, FText InItemName, FText InItemDescription, int32 InItemAmount, int32 InItemMaxAmount, 
		int32 InItemWeight, UTexture2D* InItemThumbnail, int32 InItemWeightBonus, int32 InUniqueID, int32 InSortPriority, EItemType InItemType)
	{
		ItemClass = InItemClass;
		ItemName = InItemName;
		ItemDescription = InItemDescription;
		ItemAmount = InItemAmount;
		ItemMaxAmount = InItemMaxAmount;
		ItemWeight = InItemWeight;
		ItemThumbnail = InItemThumbnail;
		WeightBonus = InItemWeightBonus;
		UniqueID = InUniqueID;
		SortPriority = InSortPriority;
		ItemType = InItemType;
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInventoryOutOfSpaceDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INVENTORYPLUGIN_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Constructor
	UInventoryComponent();

	// Add an item from the scene to the inventory
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		bool AddItem(AItem* InItem);

	// Remove an item from the inventory
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		bool DropItem(FInventoryStruct InInventoryStruct);

	// Remove an item from the inventory
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		bool UseItem(TSubclassOf<class AItem> ItemClass);

	// Sort all items in the inventory
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		bool SortInventory(ESortMethod SortMethod);

	// Split selected item stack into two seperate stacks
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		bool SplitStack(int32 InIndex, int32 SplitAmount);

	// Combine two item stacks
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		bool CombineStack(int32 FirstIndex, int32 SecondIndex);

	// Remove Items from Stack
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		bool RemoveFromStack(int32 StackIndex, int32 Amount, bool RemoveWholeStack);

	// Find Item Stack by Unique ID
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		bool FindItemStackByUniqueID(int32 InStackID, FInventoryStruct& OutStack, int32& OutIndex);
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		bool FindStackByClass(TSubclassOf<class AItem> StackClass, bool bReturnFullStacks, FInventoryStruct& outStructure, int32& OutIndex);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		bool FindStackByIndex(int32 Index, FInventoryStruct& outStructure);

	// Find Item Stack by Unique ID
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		int32 CalculateUniqueID();

	// Calculate total weight of one slot
	UFUNCTION(BlueprintPure, Category = "Inventory")
		int32 CalculateStackWeight(FInventoryStruct& outStructure);

	// Calculate total weight of all item slots
	UFUNCTION(BlueprintPure, Category = "Inventory")
		int32 CalculateInventoryWeight();

	// Array that holds most items
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		TArray<FInventoryStruct> ItemArray;

	// Backpack Slot that increases inventory capacity
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		FInventoryStruct EquippedBackpack;

	// Weapon Slot
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		FInventoryStruct EquippedWeapon;

	// Cosmetic Slot
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		FInventoryStruct EquippedCosmetic;

	// Backpack Mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		UStaticMeshComponent* BackpackMesh;

	// Weapon Mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		UStaticMeshComponent* WeaponMesh;

	// Cosmetic Mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		UStaticMeshComponent* CosmeticMesh;

	// All items in proximity
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		TArray<AItem*> ProximityItems;

	// Inventory Size determines the default size of DefaultItems Array, can be increased by Backpack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		int32 MaxIntentoryWeight = 50;

	UPROPERTY(BlueprintAssignable, Category = "Test")
		FInventoryOutOfSpaceDelegate OnOutOfSpace;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	//
	UFUNCTION()
		void AddDefaultItem(AItem* InItem);

	//
	UFUNCTION()
		void AddBackpackItem(AItem* InItem);

	//
	UFUNCTION()
		void AddWeaponItem(AItem* InItem);

	//
	UFUNCTION()
		void AddCosmeticItem(AItem* InItem);

	// Attach the item mesh to socket
	UFUNCTION()
		void AttachItemMeshToCharacter(UStaticMesh* ItemMesh, FName AttachSocket, UStaticMeshComponent* MeshSlot);

	// Sort based on the name from A to Z
	UFUNCTION()
		void SortInventoryByName();

	// Sort based on the Weight from highest to lowest
	UFUNCTION()
		void SortInventoryByWeight();

	// Sort based on the amount from highest to lowest
	UFUNCTION()
		void SortInventoryByAmount();

	// Sort based on the priority from highest to lowest
	UFUNCTION()
		void SortInventoryByPriority();

	// Counter used to generate unique stack IDs
	UPROPERTY()
		int32 UniqueIDCounter = 0;
};
