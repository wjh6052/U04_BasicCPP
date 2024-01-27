#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "IRifle.h"
#include "CPlayer.generated.h"

UCLASS()
class U04_BASICCPP_API ACPlayer : public ACharacter, public IIRifle
{
	GENERATED_BODY()

public:
	ACPlayer();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public: //인터페이스에서 가져온 함수를 재정의
	FORCEINLINE ACRifle* GetRifle() override { return Rifle; };
	void GetAimInfo(FVector& OutAimStart, FVector& OutAimEnd, FVector& OutAimDirection);

	virtual void OnTarget() override;
	virtual void OffTarget() override;

	//Axis Event
private:
	void OnMoveForward(float InAxis);
	void OnMoveRight(float InAxis);

	void OnHorizontalLook(float InAxis);
	void OnVerticalLook(float InAxis);

	//Action Event
private:
	void OnRun();
	void OffRun();

	void OnRifle();

	void OnAim();
	void OffAim();

	void OnFire();
	void OffFire();

	void OnAutoFire();

	void OnReloading();

protected:
	UFUNCTION(BlueprintImplementableEvent)
		void ZoomIn();
	UFUNCTION(BlueprintImplementableEvent)
		void ZoomOut();

public:
	UFUNCTION(BlueprintCallable)
		void SetBodyColor(FLinearColor InColor);

private:
	UPROPERTY(VisibleDefaultsOnly)
		class USpringArmComponent* SpringArm;

protected:
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly)
		class UCameraComponent* Camera;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TSubclassOf<class UCAimWidget> AimWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TSubclassOf<class UCAutoFireWidget> AutoFireWidgetClass;

	

private:
	class UMaterialInstanceDynamic* BodyMaterial;
	class UMaterialInstanceDynamic* LogoMaterial;
	

	class ACRifle* Rifle;
	class UCAimWidget* AimWidget;
	class UCAutoFireWidget* AutoFireWidget;
	
};
