// Fill out your copyright notice in the Description page of Project Settings.


#include "Texture2DGeneration.h"

#include "Kismet/KismetRenderingLibrary.h"


// Sets default values
ATexture2DGeneration::ATexture2DGeneration()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Plane = CreateDefaultSubobject<UStaticMeshComponent>("Plane");
	MaterialInterface = CreateDefaultSubobject<UMaterialInterface>("MaterialInterface");
	
	
	
	
}

// Called when the game starts or when spawned
void ATexture2DGeneration::BeginPlay()
{
	Super::BeginPlay();
	//HeightfieldTextureRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(this, 1024, 1024);
	CanvasRenderTarget = UCanvasRenderTarget2D::CreateCanvasRenderTarget2D(this, UCanvasRenderTarget2D::StaticClass(),1024, 1024);
	CanvasRenderTarget->ClearColor = FLinearColor::Black;
	MaterialInstance = UMaterialInstanceDynamic::Create(MaterialInterface, this);
	MaterialInstance->SetTextureParameterValue(FName("BaseTexture"), CanvasRenderTarget);
	Plane->SetMaterial(0, MaterialInstance);
	
	CanvasRenderTarget->OnCanvasRenderTargetUpdate.AddDynamic(this, &ATexture2DGeneration::OnUpdateCanvasRenderTarget);
	
	
}

// Called every frame
void ATexture2DGeneration::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CanvasRenderTarget->UpdateResource();

}



void ATexture2DGeneration::OnUpdateCanvasRenderTarget(UCanvas* Canvas, int32 Width, int32 Height)
{
	
	constexpr int32 X = 20;
	constexpr int32 Y = 20;
	const FLinearColor DrawColor = FLinearColor::White;
	const FVector2D RectSize = FVector2D(200, 200); 
	const FVector2D RectPosition = FVector2D(100, 100); 
	Canvas->K2_DrawBox(RectPosition, RectSize, 1.0f, DrawColor);
	//Canvas->K2_DrawTexture(CanvasRenderTarget, FVector2D(X,Y),FVector2d(1.0,1.0), FVector2D(0.0, 0.0), FVector2D::UnitVector, DrawColor);
	GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, TEXT("update"));
	
}
