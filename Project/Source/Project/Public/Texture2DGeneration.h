// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Texture2DGeneration.generated.h"

UCLASS()
class PROJECT_API ATexture2DGeneration : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATexture2DGeneration();
	virtual void Tick(float DeltaTime) override;


	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Texture Creation")
	UTexture2D* pTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object to render")
	UStaticMeshComponent* Plane;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Material")
	UMaterialInstanceDynamic* MaterialInstance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	UMaterialInterface* MaterialInterface;

	

};
