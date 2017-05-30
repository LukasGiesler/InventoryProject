// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryComponent.h"



// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Add an item from the scene to the inventory
bool UInventoryComponent::AddItem(AItem* InItem)
{
	if(!InItem->IsValidLowLevel())
		return false;

	ACharacter* Character = Cast<ACharacter>(GetOwner());
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Check type of item and call respective function
	switch (InItem->Type)
	{
		case EItemType::DEFAULT : AddDefaultItem(InItem);
			break;
		case EItemType::BACKPACK : AddBackpackItem(InItem);
			break;
		case EItemType::WEAPON : AddWeaponItem(InItem);
			break;
		case EItemType::COSMETIC : AddCosmeticItem(InItem);
			break;
	}

	return true;
}

void UInventoryComponent::AddDefaultItem(AItem* InItem)
{
	int32 PickupAmount = 1;
	int32 StackWeight = 1;
	bool bPickWholeStack = false;

	// Check if inventory has enough space for this item
	if ((CalculateInventoryWeight() + (InItem->ItemWeight * InItem->PickupAmount)) > MaxIntentoryWeight)
	{
		// Not enough space for whole stack
		if (CalculateInventoryWeight() >= MaxIntentoryWeight)
		{
			// Inventory is completely full, broadcast Out of Space Delegate
			OnOutOfSpace.Broadcast();
			return;
		}
		else
		{
			// Pickup as much as we can of the item stack
			PickupAmount = FMath::DivideAndRoundDown((MaxIntentoryWeight - CalculateInventoryWeight()), InItem->ItemWeight);
			if (PickupAmount <= 0)
			{
				OnOutOfSpace.Broadcast();
				return;
			}

			InItem->PickupAmount -= PickupAmount;
		}
	}
	else
	{
		// Pickup whole stack
		PickupAmount = InItem->PickupAmount;
		bPickWholeStack = true;
	}

	// Create inventory struct
	FInventoryStruct NewItem(InItem->GetClass(), InItem->ItemName, InItem->ItemDescription, PickupAmount, InItem->ItemMaxAmount, 
		InItem->ItemWeight, InItem->ItemThumbnail, InItem->WeightBonus, CalculateUniqueID(), InItem->SortPriority, InItem->Type);

	// Try to combine with existing stacks
	FInventoryStruct CombineStruct;
	int32 CombineIndex;
	if (FindStackByClass(InItem->GetClass(), false, CombineStruct, CombineIndex))
	{
		int32 newIndex = ItemArray.Add(NewItem);
		CombineStack(CombineIndex, newIndex);
	}
	else
	{
		// Add to inventory array
		int32 newIndex = ItemArray.Add(NewItem);
	}

	// Destroy Item in scene
	if (bPickWholeStack)
	{
		InItem->Destroy();
	}
		
}


void UInventoryComponent::AddBackpackItem(AItem* InItem)
{
	// Create inventory struct
	FInventoryStruct NewItem(InItem->GetClass(), InItem->ItemName, InItem->ItemDescription, InItem->PickupAmount, 
		InItem->ItemMaxAmount, InItem->ItemWeight, InItem->ItemThumbnail, InItem->WeightBonus, CalculateUniqueID(), InItem->SortPriority, InItem->Type);

	// Drop current backpack (if existent)
	if (BackpackMesh)
	{
		// Destroy current backpack
		BackpackMesh->DestroyComponent();
		BackpackMesh->SetActive(false);
		MaxIntentoryWeight -= EquippedBackpack.WeightBonus;

		// Spawn backpack on ground
		DropItem(EquippedBackpack);
	}

	// Set backpack slot
	EquippedBackpack = NewItem;
	MaxIntentoryWeight += EquippedBackpack.WeightBonus;

	// Spawn and attach backpack mesh
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	BackpackMesh = NewObject<UStaticMeshComponent>(Character->GetMesh(), NAME_None);
	AttachItemMeshToCharacter(InItem->ItemMesh->GetStaticMesh(), TEXT("BackpackSocket"), BackpackMesh);

	// Destroy Item in scene
	InItem->Destroy();
}

void UInventoryComponent::AddWeaponItem(AItem* InItem)
{
	// Create inventory struct
	FInventoryStruct NewItem(InItem->GetClass(), InItem->ItemName, InItem->ItemDescription, InItem->PickupAmount, 
		InItem->ItemMaxAmount, InItem->ItemWeight, InItem->ItemThumbnail, InItem->WeightBonus, CalculateUniqueID(), InItem->SortPriority, InItem->Type);

	// Drop current weapon (if existent)
	if (WeaponMesh)
	{
		// Destroy current weapon
		WeaponMesh->DestroyComponent();
		WeaponMesh->SetActive(false);

		// Spawn weapon on ground
		DropItem(EquippedWeapon);
	}

	// Set weapon slot
	EquippedWeapon = NewItem;

	// Spawn and attach weapon mesh
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	WeaponMesh = NewObject<UStaticMeshComponent>(Character->GetMesh(), NAME_None);
	AttachItemMeshToCharacter(InItem->ItemMesh->GetStaticMesh(), TEXT("WeaponSocket"), WeaponMesh);

	// Destroy Item in scene
	InItem->Destroy();
}

void UInventoryComponent::AddCosmeticItem(AItem* InItem)
{
	// Create inventory struct
	FInventoryStruct NewItem(InItem->GetClass(), InItem->ItemName, InItem->ItemDescription, InItem->PickupAmount, 
		InItem->ItemMaxAmount, InItem->ItemWeight, InItem->ItemThumbnail, InItem->WeightBonus, CalculateUniqueID(), InItem->SortPriority, InItem->Type);

	// Drop current cosmetic (if existent)
	if (CosmeticMesh)
	{
		// Destroy current cosmetic
		CosmeticMesh->DestroyComponent();
		CosmeticMesh->SetActive(false);

		// Spawn cosmetic on ground
		DropItem(EquippedCosmetic);
	}

	// Set cosmetic slot
	EquippedCosmetic = NewItem;

	// Spawn and attach cosmetic mesh
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	CosmeticMesh = NewObject<UStaticMeshComponent>(Character->GetMesh(), NAME_None);
	AttachItemMeshToCharacter(InItem->ItemMesh->GetStaticMesh(), TEXT("CosmeticSocket"), CosmeticMesh);

	// Destroy Item in scene
	InItem->Destroy();
}

// Attach the item mesh to socket
void UInventoryComponent::AttachItemMeshToCharacter(UStaticMesh* ItemMesh, FName AttachSocket, UStaticMeshComponent* MeshSlot)
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Spawn a static mesh and attach it to the socket
	MeshSlot->SetStaticMesh(ItemMesh);
	MeshSlot->SetRelativeLocation(FVector(0, 0, 0));
	MeshSlot->RegisterComponentWithWorld(GetWorld());
	MeshSlot->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, AttachSocket);
	MeshSlot->SetActive(true);
}

// Remove an item from the inventory
bool UInventoryComponent::DropItem(FInventoryStruct InInventoryStruct)
{
	// Check if this Item is in our inventory
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	FVector DropLocation = Character->GetMesh()->GetSocketLocation(TEXT("ItemSpawnSocket"));

	// Spawn Item in scene
	AItem* DroppedItem = GetWorld()->SpawnActor<AItem>(InInventoryStruct.ItemClass, DropLocation, FRotator::ZeroRotator, SpawnInfo);

	// Adjust variables
	DroppedItem->PickupAmount = InInventoryStruct.ItemAmount;

	// Remove from Inventory array
	if (InInventoryStruct.ItemType == EItemType::DEFAULT)
	{
		FInventoryStruct StackToRemove;
		int32 IndexToRemove = 0;
		FindItemStackByUniqueID(InInventoryStruct.UniqueID, StackToRemove, IndexToRemove);
		ItemArray.RemoveAt(IndexToRemove);
	}

	return true;
}

// Use item an item of this class
bool UInventoryComponent::UseItem(TSubclassOf<class AItem> ItemClass)
{
	FInventoryStruct InInventoryStruct;
	int32 InIndex;

	// Check if the inventory contains this item
	if (!(FindStackByClass(ItemClass, true, InInventoryStruct, InIndex)))
		return false;

	// Remove 1 from stack
	RemoveFromStack(InIndex, 1, false);

	// Spawn temp item to access use function
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AItem* TempItem = GetWorld()->SpawnActor<AItem>(InInventoryStruct.ItemClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnInfo);

	TempItem->OnUse();

	// Destroy after use
	TempItem->Destroy();

	return true;
}

// Split selected item stack into two seperate stacks
bool UInventoryComponent::SplitStack(int32 InIndex, int32 SplitAmount)
{
	FInventoryStruct InventoryStruct;
	int32 Remainder;

	FindStackByIndex(InIndex, InventoryStruct);

	// Amount to split is bigger than amount on slot or too small
	if ((SplitAmount >= InventoryStruct.ItemAmount) || (SplitAmount <= 0))
	{
		return false;
	}

	// Calculate new amounts
	Remainder = InventoryStruct.ItemAmount - SplitAmount;
	ItemArray[InIndex].ItemAmount = InventoryStruct.ItemAmount - SplitAmount;

	// Add the split stack to inventory
	FInventoryStruct NewStack(InventoryStruct.ItemClass, InventoryStruct.ItemName, InventoryStruct.ItemDescription, SplitAmount, InventoryStruct.ItemMaxAmount, InventoryStruct.ItemWeight, 
		InventoryStruct.ItemThumbnail, InventoryStruct.WeightBonus, CalculateUniqueID(), InventoryStruct.SortPriority, InventoryStruct.ItemType);

	ItemArray.Add(NewStack);

	return true;
}

// Combine two item stacks
bool UInventoryComponent::CombineStack(int32 FirstIndex, int32 SecondIndex)
{
	FInventoryStruct FirstInventoryStruct;
	FInventoryStruct SecondInventoryStruct;

	// Find the stacks to combine
	FindStackByIndex(FirstIndex, FirstInventoryStruct);
	FindStackByIndex(SecondIndex, SecondInventoryStruct);

	// Check if they have the same item class
	if (ItemArray[FirstIndex].ItemClass != ItemArray[SecondIndex].ItemClass)
	{
		return false;
	}

	// Check if we will go over max capacity of this stack
	if (ItemArray[FirstIndex].ItemMaxAmount >= FirstInventoryStruct.ItemAmount + SecondInventoryStruct.ItemAmount)
	{
		// Both stacks can be combined to one stack
		ItemArray[FirstIndex].ItemAmount += ItemArray[SecondIndex].ItemAmount;
		ItemArray.RemoveAt(SecondIndex);

		return true;
	}
	else {
		// A second stack is required to contain the remainder
		int32 Remainder = ItemArray[FirstIndex].ItemMaxAmount - ItemArray[FirstIndex].ItemAmount;
		ItemArray[FirstIndex].ItemAmount = ItemArray[FirstIndex].ItemMaxAmount;
		ItemArray[SecondIndex].ItemAmount -= Remainder;

		return true;
	}
}

// Remove specified amount from item stack
bool UInventoryComponent::RemoveFromStack(int32 StackIndex, int32 Amount, bool RemoveWholeStack)
{
	// Check if we can remove that much from this stack
	if (Amount > ItemArray[StackIndex].ItemAmount)
		return false;

	if (RemoveWholeStack) {
		ItemArray.RemoveAt(StackIndex);

		return true;
	}
	else 
	{
		// Remove items from stack
		ItemArray[StackIndex].ItemAmount -= Amount;

		// Remove stack if completely empty
		if (ItemArray[StackIndex].ItemAmount <= 0) {
			ItemArray.RemoveAt(StackIndex);
		}

		return true;
	}
}

// Find Item Stack by ID
bool UInventoryComponent::FindItemStackByUniqueID(int32 InStackID, FInventoryStruct& OutStack, int32& OutIndex)
{
	// Iterate through Item Array
	for (auto Iterator(this->ItemArray.CreateIterator()); Iterator; Iterator++)
	{
		if (!Iterator) continue;

		// Find the stack we are looking for
		if (Iterator->UniqueID == InStackID)
		{
			OutStack = *Iterator;
			OutIndex = Iterator.GetIndex();

			return true;
		}

	}

	return false;
}

// Find item stack of a certain class
bool UInventoryComponent::FindStackByClass(TSubclassOf<class AItem> StackClass, bool bReturnFullStacks, FInventoryStruct& outStruct, int32& FoundIndex)
{
	// Iterate through Item Array
	for (auto Iterator(this->ItemArray.CreateIterator()); Iterator; Iterator++)
	{
		if (!Iterator) continue;

		// Check if current iterator is the class we are looking for
		if (Iterator->ItemClass == StackClass)
		{
			if (Iterator->ItemAmount < Iterator->ItemMaxAmount)
			{
				// Return any stack
				outStruct = *Iterator;
				FoundIndex = Iterator.GetIndex();

				return true;
			}
			else
			{
				// Allow returning full stacks
				if (bReturnFullStacks)
				{
					outStruct = *Iterator;
					FoundIndex = Iterator.GetIndex();

					return true;
				}
			}
		}
	}

	return false;
}

// Search Item Stack by Index
bool UInventoryComponent::FindStackByIndex(int32 Index, FInventoryStruct& outStructure) {
	if ((Index < 0) || (Index > ItemArray.Num()))
		return false;

	outStructure = this->ItemArray[Index];

	return true;
}

// Accumulate weight of all items and return total weight
int32 UInventoryComponent::CalculateInventoryWeight()
{
	int32 OutWeight = 0;

	// Iterate over Item Array
	for (auto Iterator(this->ItemArray.CreateIterator()); Iterator; Iterator++)
	{
		if (!Iterator) continue;

		OutWeight += CalculateStackWeight(*Iterator);
	}

	return OutWeight;
}

// Calculate total weight of one slot
int32 UInventoryComponent::CalculateStackWeight(FInventoryStruct& InStack)
{
	int32 OutWeight = 0;

	OutWeight = InStack.ItemAmount * InStack.ItemWeight;

	return OutWeight;
}

// Calculate a unique stack ID
int32 UInventoryComponent::CalculateUniqueID()
{
	int32 OutUniqueID = -1;

	OutUniqueID = ++UniqueIDCounter;
	
	return OutUniqueID;
}

// Sort all items in the inventory based on given sort method
bool UInventoryComponent::SortInventory(ESortMethod SortMethod)
{
	switch (SortMethod)
	{
	case ESortMethod::NAME :
	{
		SortInventoryByName();
	}
	break;

	case ESortMethod::WEIGHT :
	{
		SortInventoryByWeight();
	}
	break;

	case ESortMethod::AMOUNT :
	{
		SortInventoryByAmount();
	}
	break;

	default:
	{
		SortInventoryByPriority();
	}
	break;
	}
	return true;
}

void UInventoryComponent::SortInventoryByName()
{
	ItemArray.Sort([](const FInventoryStruct& One, const FInventoryStruct& Two) {
		return One.ItemName.ToString() < Two.ItemName.ToString();
	});
}

void UInventoryComponent::SortInventoryByWeight()
{
	ItemArray.Sort([](const FInventoryStruct& One, const FInventoryStruct& Two) {
		return One.ItemWeight < Two.ItemWeight;
	});
}

void UInventoryComponent::SortInventoryByAmount()
{
	ItemArray.Sort([](const FInventoryStruct& One, const FInventoryStruct& Two) {
		return One.ItemAmount < Two.ItemAmount;
	});
}

void UInventoryComponent::SortInventoryByPriority()
{
	ItemArray.Sort([](const FInventoryStruct& One, const FInventoryStruct& Two) {
		return One.SortPriority < Two.SortPriority;
	});
}