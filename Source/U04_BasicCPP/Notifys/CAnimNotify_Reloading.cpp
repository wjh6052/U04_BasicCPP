#include "CAnimNotify_Reloading.h"
#include "Global.h"
#include "Characters/IRifle.h"
#include "Characters/CRifle.h"

FString UCAnimNotify_Reloading::GetNotifyName_Implementation() const
{
	return "Reloading";
}

void UCAnimNotify_Reloading::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	CheckNull(MeshComp);

	//Get Player
	IIRifle* rifleCharacter = Cast<IIRifle>(MeshComp->GetOwner());
	CheckNull(rifleCharacter);

	rifleCharacter->GetRifle()->Reloading2();
}
