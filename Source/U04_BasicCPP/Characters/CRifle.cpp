#include "CRifle.h"
#include "Global.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/DecalComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Characters/IRifle.h"
#include "CBullet.h"



ACRifle::ACRifle()
{
	PrimaryActorTick.bCanEverTick = true;

	CHelpers::CreateSceneComponent(this, &Mesh, "Mesh");

	USkeletalMesh* meshAsset;
	CHelpers::GetAsset(&meshAsset, "SkeletalMesh'/Game/Weapons/Meshes/AR4/SK_AR4.SK_AR4'");
	Mesh->SetSkeletalMesh(meshAsset);

	CHelpers::GetAsset(&GrabMontage, "/Game/Character/Animations/Rifle/Rifle_Grab_Montage.Rifle_Grab_Montage");
	CHelpers::GetAsset(&UngrabMontage, "/Game/Character/Animations/Rifle/Rifle_UnGrab_Montage.Rifle_Ungrab_Montage");

	//Get CameraShake ClasRef
	CHelpers::GetClass<UCameraShake>(&ShakeClass, "Blueprint'/Game/Player/BP_Shake.BP_Shake_C'");
	
	//Get Bullet ClasRef
	CHelpers::GetClass<ACBullet>(&BulletClass, "Blueprint'/Game/Player/BP_CBullet.BP_CBullet_C'");

	//Get Effects
	CHelpers::GetAsset(&MuzzleParticle, "ParticleSystem'/Game/Particles_Rifle/Particles/VFX_Muzzleflash.VFX_Muzzleflash'");
	CHelpers::GetAsset(&EjectParticle, "ParticleSystem'/Game/Particles_Rifle/Particles/VFX_Eject_bullet.VFX_Eject_bullet'");
	CHelpers::GetAsset(&ImpactParticle, "ParticleSystem'/Game/Particles_Rifle/Particles/VFX_Impact_Default.VFX_Impact_Default'");
	CHelpers::GetAsset(&FireSoundCue, "SoundCue'/Game/Sounds/S_RifleShoot_Cue.S_RifleShoot_Cue'");
	CHelpers::GetAsset(&DecalMaterial, "MaterialInstanceConstant'/Game/Materials/M_Decal_Inst.M_Decal_Inst'");

}

ACRifle* ACRifle::Spawn(UWorld* InWorld, ACharacter* InOwner)
{
	FActorSpawnParameters spawnParam;
	spawnParam.Owner = InOwner;

	return InWorld->SpawnActor<ACRifle>(spawnParam);
}



void ACRifle::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACharacter>(GetOwner());

	if (!!OwnerCharacter)
		AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true), HolsterSocket);
}

void ACRifle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckFalse(bAiming);

	//LineTrace for Aim(HitScan)
	IIRifle* rifleCharacter = Cast< IIRifle>(OwnerCharacter);
	CheckNull(rifleCharacter);

	FVector start, end, direction;
	rifleCharacter->GetAimInfo(start, end, direction);

	//DrawDebugLine(GetWorld(), start, end, FColor::Red);

	FCollisionQueryParams queryParam;
	queryParam.AddIgnoredActor(this);
	queryParam.AddIgnoredActor(OwnerCharacter);

	FHitResult hitResult;
	if (GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECollisionChannel::ECC_PhysicsBody, queryParam))
	{
		//�浹�� ���Ͱ� StaticMeshActor Ÿ������ �˻�
		AStaticMeshActor* otherActor = Cast<AStaticMeshActor>(hitResult.GetActor());
		if (!!otherActor)
		{
			//��Ʈ ������Ʈ�� StaticMeshComp Ÿ������ �˻�
			UStaticMeshComponent* otherComp = Cast <UStaticMeshComponent>(otherActor->GetRootComponent());
			if (!!otherComp)
			{
				//�� ��Ʈ ������Ʈ�� �ùķ���Ʈ �������� ���� �ִ���
				if (otherComp->BodyInstance.bSimulatePhysics == true)
				{
					rifleCharacter->OnTarget();
					return;
				}
			}
		}
	}
	rifleCharacter->OffTarget();



}

void ACRifle::Equip()
{
	CheckTrue(bEquipping);
	CheckTrue(bEquipped);

	bEquipping = true;
	OwnerCharacter->PlayAnimMontage(GrabMontage, 1.1f);
}

void ACRifle::Begin_Equip()
{
	bEquipped = true;

	AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true), HandrSocket);
	
	OwnerCharacter->bUseControllerRotationYaw = true;
	OwnerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
}

void ACRifle::End_Equip()
{
	bEquipping = false;
}

void ACRifle::Unequip()
{
	CheckFalse(bEquipped);
	CheckTrue(bEquipping);

	bEquipped = true;
	OwnerCharacter->PlayAnimMontage(UngrabMontage, 1.25f);
}

void ACRifle::Begin_Unequip()
{
	bEquipped = false;
	AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true), HolsterSocket);

	OwnerCharacter->bUseControllerRotationYaw = false;
	OwnerCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
}

void ACRifle::End_Unequip()
{
	bEquipping = false;
}

void ACRifle::Begin_Aim()
{
	bAiming = true;
}

void ACRifle::End_Aim()
{
	bAiming = false;
}

void ACRifle::Begin_Fire()
{
	CheckFalse(bEquipped);
	CheckTrue(bEquipping);
	//CheckFalse(bAiming);
	CheckTrue(bFiring);

	bFiring = true;

	CurrentPitch = 0.0f;

	if (bAutoFire == true)
	{
		GetWorld()->GetTimerManager().SetTimer(AutoFireTimer, this, &ACRifle::Firing, 0.1f, true);

		return;
	}

	Firing();
}

void ACRifle::End_Fire()
{
	bFiring = false;

	if (bAutoFire == true)
		GetWorld()->GetTimerManager().ClearTimer(AutoFireTimer);
}

void ACRifle::Firing()
{
	//Get Aim Info
	IIRifle* rifleCharacter = Cast<IIRifle>(OwnerCharacter);
	CheckNull(rifleCharacter);

	FVector start, end, direction;
	rifleCharacter->GetAimInfo(start, end, direction);

	//Play Camera Shake
	APlayerController* controller = OwnerCharacter->GetController<APlayerController>();
	if (!!controller)
		controller->PlayerCameraManager->PlayCameraShake(ShakeClass);

	//Spawn Bullet
	FVector muzzleLocation = Mesh->GetSocketLocation("MuzzleFlash");
	if (!!BulletClass)
		GetWorld()->SpawnActor<ACBullet>(BulletClass, muzzleLocation, direction.Rotation());

	//Play Particle & Sound
	UGameplayStatics::SpawnEmitterAttached(MuzzleParticle, Mesh, "MuzzleFlash");
	UGameplayStatics::SpawnEmitterAttached(EjectParticle, Mesh, "EjectBullet");
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSoundCue, muzzleLocation);

	//Additional Pitch
	CurrentPitch -= PitchSpeed * GetWorld()->GetDeltaSeconds();
	if (CurrentPitch > -LimitPitch)
		OwnerCharacter->AddControllerPitchInput(CurrentPitch);

	//LineTrace for AddImpulse
	FCollisionQueryParams queryParam;
	queryParam.AddIgnoredActor(this);
	queryParam.AddIgnoredActor(OwnerCharacter);

	FHitResult hitResult;
	if (GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECollisionChannel::ECC_Visibility, queryParam))
	{
		//�浹�� ���Ͱ� StaticMeshActor Ÿ������ �˻�
		AStaticMeshActor* otherActor = Cast<AStaticMeshActor>(hitResult.GetActor());
		if (!!otherActor)
		{
			//Spawn Decal
			FRotator decalRotation = hitResult.ImpactNormal.Rotation();
			UDecalComponent* decalComp = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), DecalMaterial, FVector(5), hitResult.Location, decalRotation, 10.f);
			decalComp->SetFadeScreenSize(0);

			//Spawn Impact Particle
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, hitResult.Location, decalRotation, true);


			//Add Impulse
			//��Ʈ ������Ʈ�� StaticMeshComp Ÿ������ �˻�
			UStaticMeshComponent* otherComp = Cast <UStaticMeshComponent>(otherActor->GetRootComponent());
			if (!!otherComp)
			{
				//�� ��Ʈ ������Ʈ�� �ùķ���Ʈ �������� ���� �ִ���
				if (otherComp->BodyInstance.bSimulatePhysics == true)
				{
					//�÷��̾ ǥ������ �ٶ󺸴� ����
					direction = otherActor->GetActorLocation() - OwnerCharacter->GetActorLocation();
					direction.Normalize();

					otherComp->AddImpulseAtLocation(direction * 3000.0f, OwnerCharacter->GetActorLocation());

					return;
				}
			}
		}
	}

}
