// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBlueprintFunctionLibrary.h"


UTexture2D* UMyBlueprintFunctionLibrary::CreateTexture2D(int32 Width, int32 Height)
{
	UTexture2D* Texture2D = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8);
	Texture2D->NeverStream = true;
	return Texture2D;
	
}

void UMyBlueprintFunctionLibrary::ClearTexture2D(UTexture2D* Texture2D, FColor Colour)
{
	const int32 Width = Texture2D->GetSizeX();
	const int32 Height = Texture2D->GetSizeY();
	FByteBulkData* RawImageDataOut = &Texture2D->GetPlatformData()->Mips[0].BulkData;
	FColor* FormatedImageDataOut = static_cast<FColor*>(RawImageDataOut->Lock(LOCK_READ_WRITE));
	for(int X=0; X<Width; X++)
	{
		for (int Y =0; Y<Height; Y++)
		{
			FormatedImageDataOut[(Y * Width) + X] = Colour;
		}
	}
	RawImageDataOut->Unlock();
	Texture2D->UpdateResource();
	
}

void UMyBlueprintFunctionLibrary::SetTexture2DPixels(UTexture2D* Texture2D, int32 X, int32 Y, FColor Colour)
{
	const int32 Width = Texture2D->GetSizeX();
	const int32 Height = Texture2D->GetSizeY();
	FByteBulkData* RawImageDataOut = &Texture2D->GetPlatformData()->Mips[0].BulkData;
	FColor* FormatedImageDataOut = static_cast<FColor*>(RawImageDataOut->Lock(LOCK_READ_WRITE));
	FormatedImageDataOut[(Y * Width) + X] = Colour;
	RawImageDataOut->Unlock();
	Texture2D->UpdateResource();
}


float UMyBlueprintFunctionLibrary::RandomVector2DtoVector1D (FVector2D Vector2D, FVector2D a, float b, float c)
{
	//avoid artifacts
	FVector2D SmallerValue;
	SmallerValue.X = FMath::Sin(Vector2D.X);
	SmallerValue.Y = FMath::Sin(Vector2D.Y);
	//get scalar from vector2D
	float Random = FVector2D::DotProduct(SmallerValue, a);
	Random = FMath::Frac(FMath::Sin(Random + b)* c);
	return Random;
	
}

FVector2D UMyBlueprintFunctionLibrary::RandomVector2DtoVector2D(FVector2D Vector2D)
{
	Vector2D.X = RandomVector2DtoVector1D(Vector2D, FVector2D(8.453, 7.983), 2.2, 50.12);
	Vector2D.Y = RandomVector2DtoVector1D(Vector2D, FVector2D(4.912, 10.902), -9.8, 102.54);
	return Vector2D;
	
}

float UMyBlueprintFunctionLibrary::RandomVector1DtoVector1D(float RandomFloatNumber, float a,float b)
{
	const float RandomNumber =  FMath::Frac(FMath::Sin(RandomFloatNumber + a)*b);
	return RandomNumber;
	
}

FVector UMyBlueprintFunctionLibrary::RandomVector1DtoVector3D(float RandomFloatNumber)
{
	FVector Vector3D;
	Vector3D.X = RandomVector1DtoVector1D(RandomFloatNumber, 21.289,90.92);
	Vector3D.Y = RandomVector1DtoVector1D(RandomFloatNumber, 61.281,40.02);
	Vector3D.Z = RandomVector1DtoVector1D(RandomFloatNumber, 23.09,80.65);
	return Vector3D;
}

FColor UMyBlueprintFunctionLibrary::VoronoiCalculation(FVector2D PixelLocation, float CellScale)
{
	const FVector2D Value = FVector2D(PixelLocation.X/CellScale, PixelLocation.Y/CellScale);
	const FVector2D BaseCell = FVector2D(FMath::Floor(Value.X), FMath::Floor(Value.Y));
	float MinDist = FLT_MAX;
	FVector2D ClosestCell = FVector2D();
	for(int X = -1; X <=1; X++)
	{
		for(int Y = -1; Y <=1; Y++)
		{
			FVector2D CellXYIndex = BaseCell + FVector2D(X,Y);
			FVector2D CellPos = CellXYIndex + RandomVector2DtoVector2D(CellXYIndex);
			const float CellDistWorldPosition = (CellPos - Value).Length();
			if(CellDistWorldPosition < MinDist)
			{
				MinDist = CellDistWorldPosition;
				ClosestCell = CellXYIndex;
				
			}
			
			
		}
	}

	const float RandomColor = RandomVector2DtoVector1D(ClosestCell, FVector2D(289.89, 38.02), 4.98, 60.90);
	const FVector vOffset = RandomVector1DtoVector3D(RandomColor);
	const FColor PixelDrawCol = FColor(vOffset.X * 255, vOffset.Y * 255,vOffset.Z * 255, 255);
	return PixelDrawCol;

}

void UMyBlueprintFunctionLibrary::DrawVoronoiOnTexture2D(UTexture2D* Texture2D, float CellSize)
{

	const int32 Width = Texture2D->GetSizeX();
	const int32 Height = Texture2D->GetSizeY();
	FByteBulkData* RawImageDataOut = &Texture2D->GetPlatformData()->Mips[0].BulkData;
	FColor* FormatedImageDataOut = static_cast<FColor*>(RawImageDataOut->Lock(LOCK_READ_WRITE));
	for(int X=0; X<Width; X++)
	{
		for (int Y =0; Y<Height; Y++)
		{
			const FColor PixelColor = VoronoiCalculation(FVector2D(X, Y), CellSize);
			FormatedImageDataOut[(Y * Width) + X] = PixelColor;
		}
	}
	RawImageDataOut->Unlock();
	Texture2D->UpdateResource();
}
