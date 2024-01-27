#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CRifle.generated.h"

UCLASS()
class U04_BASICCPP_API ACRifle : public AActor
{
	GENERATED_BODY()
	
public:	
	ACRifle();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	static ACRifle* Spawn(class UWorld* InWorld, class ACharacter* InOwner);

	FORCEINLINE bool IsEquipping() { return bEquipping; }
	FORCEINLINE bool IsEquipped() { return bEquipped; }
	FORCEINLINE bool IsAiming() { return bAiming; }


	FORCEINLINE bool IsFiring() { return bFiring; }
	FORCEINLINE bool IsAutoFire() { return bAutoFire; }
	FORCEINLINE void ToggleAutoFire() { bAutoFire = !bAutoFire; }

public:
	void Equip();
	void Begin_Equip();
	void End_Equip();

	void Unequip();
	void Begin_Unequip();
	void End_Unequip();

	void Begin_Aim();
	void End_Aim();

	void Begin_Fire();	//Ç¥ÀûÆÇÀ» ³Ñ¾î¶ß¸² + ÃÑ±â ÀÓÆåÆ® Àç»ý
	void End_Fire();

	void Firing();

	void Bring_Widget(class UCAutoFireWidget* A);

	void Reloading();
	void Reloading2();



private:
	UPROPERTY(EditDefaultsOnly, Category = "Fire")
		float PitchSpeed = 3.f;

	UPROPERTY(EditDefaultsOnly, Category = "Fire")
		float LimitPitch = 0.4f;

	UPROPERTY(EditDefaultsOnly, Category = "Socket")
		FName HolsterSocket = "Holster_Rifle";

	UPROPERTY(EditDefaultsOnly, Category = "Socket")
		FName HandrSocket = "Hand_Rifle";

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
		class UAnimMontage* GrabMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
		class UAnimMontage* UngrabMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
		class UAnimMontage* Reload_Motion;

	UPROPERTY(EditDefaultsOnly, Category = "Effect")
		TSubclassOf<UCameraShake> ShakeClass;

	UPROPERTY(EditDefaultsOnly, Category = "Effect")
		TSubclassOf<class ACBullet> BulletClass;

	UPROPERTY(EditDefaultsOnly, Category = "Effect")
		class UParticleSystem* MuzzleParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Effect")
		class UParticleSystem* EjectParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Effect")
		class UParticleSystem* ImpactParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Effect")
		class USoundCue* FireSoundCue;

	UPROPERTY(EditDefaultsOnly, Category = "Effect")
		class UMaterialInstanceConstant* DecalMaterial;

private:
	UPROPERTY(VisibleDefaultsOnly)
		class USkeletalMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TSubclassOf<class UCAutoFireWidget> AutoFireWidgetClass;

private:
	class ACharacter* OwnerCharacter;
	class UCAutoFireWidget* AutoFireWidget;

	bool bEquipping;//Is Playing Montages
	bool bEquipped;	//Used In AnimInstance
	bool bAiming;	//Is R-Button Pressed
	bool bFiring;	//Is L_Button Pressed
	bool bAutoFire;	 //Is 'B' Button Toggle

	int32 bullet;


	FTimerHandle AutoFireTimer;

	float CurrentPitch;
};
