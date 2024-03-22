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


TArray<FVector2D> UMyBlueprintFunctionLibrary::VoronoiSeeds;
TArray<FVector2D> UMyBlueprintFunctionLibrary::Vertices;
TArray<FVector2D> UMyBlueprintFunctionLibrary::MergedVertices;
TArray<TArray<float>> UMyBlueprintFunctionLibrary::DistField;
TArray<TArray<float>> UMyBlueprintFunctionLibrary::GradientField;
TArray<TArray<FVector2D>> UMyBlueprintFunctionLibrary::ClosestCellVoronoiSeedXY;

void UMyBlueprintFunctionLibrary::InitializeClosestCellVoronoiSeedXY(UTexture2D* Texture2D)
{
	const int32 Width = Texture2D -> GetSizeX();
	const int32 Height = Texture2D -> GetSizeY();
	ClosestCellVoronoiSeedXY.SetNum(Width);
	
	for(int X = 0; X < Width; ++X)
	{
		ClosestCellVoronoiSeedXY[X].SetNum(Height);
		
		for(int Y = 0; Y < Height; ++Y)
		{
			ClosestCellVoronoiSeedXY[X][Y] = FVector2D(0,0); 
		}
	}
}

void UMyBlueprintFunctionLibrary::InitializeDistField(UTexture2D* Texture2D)
{
	//__int128 test;1<<ghfh|
	const int32 Width = Texture2D -> GetSizeX();
	const int32 Height = Texture2D -> GetSizeY();
	DistField.SetNum(Width);
	
	for(int X = 0; X < Width; ++X)
	{
		DistField[X].SetNum(Height);
		
		for(int Y = 0; Y < Height; ++Y)
		{
			DistField[X][Y] = 1000.0f; 
		}
	}
}


void UMyBlueprintFunctionLibrary::InitializeGradientField(UTexture2D* Texture2D)
{
	const int32 Width = Texture2D -> GetSizeX();
	const int32 Height = Texture2D -> GetSizeY();
	GradientField.SetNum(Width);
	
	for(int X = 0; X < Width; ++X)
	{
		GradientField[X].SetNum(Height);
		
		for(int Y = 0; Y < Height; ++Y)
		{
			GradientField[X][Y] = 1000.0f; 
		}
	}
}


void UMyBlueprintFunctionLibrary::DrawVoronoiSeedsOnTexture2D(UTexture2D* Texture2D, FColor color)
{

	const int32 Width = Texture2D->GetSizeX();
	const int32 Height = Texture2D->GetSizeY();
	FByteBulkData* RawImageDataOut = &Texture2D->GetPlatformData()->Mips[0].BulkData;
	FColor* FormatedImageDataOut = static_cast<FColor*>(RawImageDataOut->Lock(LOCK_READ_WRITE));
	for(int i = 0; i<VoronoiSeeds.Num(); i++)
	{
		for (int X=0; X<Width; X++)
		{
			for(int Y =0; Y<Height; Y++)
			{
				if(X == VoronoiSeeds[i].X && Y == VoronoiSeeds[i].Y)
				{
					FormatedImageDataOut[(Y * Width) + X] = color;
				}
			}
			
		}
	}
	RawImageDataOut->Unlock();
	Texture2D->UpdateResource();
	VoronoiSeeds.Empty();
}

/*FColor UMyBlueprintFunctionLibrary::VoronoiCalculation(FVector2D PixelLocation, float CellScale)
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
			//cellPos is seed
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
}*/

FColor UMyBlueprintFunctionLibrary::VoronoiCalculation(FVector2D PixelLocation, float CellCount)
{
	return FColor::Black;


}




void UMyBlueprintFunctionLibrary::DrawVoronoiOnTexture2D(UTexture2D* Texture2D, float CellCount)
{

	const int32 Width = Texture2D->GetSizeX();
	const int32 Height = Texture2D->GetSizeY();
	FByteBulkData* RawImageDataOut = &Texture2D->GetPlatformData()->Mips[0].BulkData;
	FColor* FormatedImageDataOut = static_cast<FColor*>(RawImageDataOut->Lock(LOCK_READ_WRITE));

	const int32 PixelsInEachCellX = Width/CellCount;
	const int32 PixelsInEachCellY = Height/CellCount;

	
	
	for(int X=1; X<Width-1; X++)
	{
		for (int Y =1; Y<Height-1; Y++)
		{
			const FVector2D CellXY = FVector2D(FMath::Floor(X/PixelsInEachCellX), FMath::Floor(Y/PixelsInEachCellY));
			float MinDist = FLT_MAX;
			FVector2D ClosestCell = FVector2D(0,0);
			FVector2D PixelXYToClosestCellSeed = FVector2D(0,0);

			for(int i = -1; i<=1; i++)
			{
				for (int j = -1; j <=1; j++)
				{
					const FVector2D CurrentCellXY = FVector2D(CellXY.X + i, CellXY.Y +j);
					if(CurrentCellXY.X < 0 || CurrentCellXY.Y < 0 || CurrentCellXY.X >=CellCount || CurrentCellXY.Y >=CellCount) continue;
					FVector2D CurrentCellPixelXY = FVector2D(PixelsInEachCellX* CurrentCellXY.X,PixelsInEachCellY* CurrentCellXY.Y );
					FVector2D VoronoiSeedPixelXY = CurrentCellPixelXY + FMath::Floor(FMath::Lerp(0, PixelsInEachCellX, RandomVector2DtoVector2D(CurrentCellPixelXY).X));
					VoronoiSeeds.Add(VoronoiSeedPixelXY);
					FVector2D CurrentPixelToVoronoiSeed = (FVector2D(X,Y) - VoronoiSeedPixelXY);
					const float Dist = CurrentPixelToVoronoiSeed.Length();
					if(Dist < MinDist)
					{
						PixelXYToClosestCellSeed = CurrentPixelToVoronoiSeed;
						MinDist = Dist;
						ClosestCell = CurrentCellXY;
						ClosestCellVoronoiSeedXY[X][Y] = VoronoiSeedPixelXY;
					}
					
				}
			}
			

			
			
			const float NoiseColor = RandomVector2DtoVector1D(ClosestCell, FVector2D(289.89, 38.02), 4.98, 60.90);
			const FVector Color = RandomVector1DtoVector3D(NoiseColor);
			const FColor PixelDrawCol = FColor(Color.X * 255, Color.Y * 255,Color.Z * 255, 255);


			const FColor PixelColor = PixelDrawCol;
			FormatedImageDataOut[(Y * Width) + X] = PixelColor;
			
		}
	}
	RawImageDataOut->Unlock();
	Texture2D->UpdateResource();
}

void UMyBlueprintFunctionLibrary::CalculateVertices(UTexture2D* Texture2D)
{
	const int32 Width = Texture2D->GetSizeX();
	const int32 Height = Texture2D->GetSizeY();

	for(int X=1; X<Width-1; X++)
	{
		for (int Y =1; Y<Height-1; Y++)
		{
			TSet<FVector2D> UniqueSeeds;
			for(int i = -1; i <=1; i++)
			{
				for(int j = -1; j <= 1; j++)
				{
					if(i == 0 && j == 0) continue;
					FVector2D CurrentClosestSeed = ClosestCellVoronoiSeedXY[X+i][Y+j];
					UniqueSeeds.Add(CurrentClosestSeed);
					
	
				}
			}
			
			if(UniqueSeeds.Num()>=3)
			{
				Vertices.Add(FVector2D(X, Y));
			}
			
		}
	}


}

void UMyBlueprintFunctionLibrary::DrawVerticesOnTexture2D(UTexture2D* Texture2D, FColor color)
{

	const int32 Width = Texture2D->GetSizeX();
	const int32 Height = Texture2D->GetSizeY();
	FByteBulkData* RawImageDataOut = &Texture2D->GetPlatformData()->Mips[0].BulkData;
	FColor* FormatedImageDataOut = static_cast<FColor*>(RawImageDataOut->Lock(LOCK_READ_WRITE));
	for(int i = 0; i<Vertices.Num(); i++)
	{
		for (int X=0; X<Width; X++)
		{
			for(int Y =0; Y<Height; Y++)
			{
				if(X == Vertices[i].X && Y == Vertices[i].Y)
				{
					FormatedImageDataOut[(Y * Width) + X] = color;
				}
			}
			
		}
	}
	RawImageDataOut->Unlock();
	Texture2D->UpdateResource();
	//UE_LOG(LogTemp, Warning, TEXT("Number of vertices: %d"), Vertices.Num());
	
}

void UMyBlueprintFunctionLibrary::MergeCloseVertices(float MergeDistance)
{
	TArray<bool> isVertexMerged;
	isVertexMerged.Init(false, Vertices.Num());
	for (int i = 0; i < Vertices.Num(); i++)
	{
		if(isVertexMerged[i]) continue;
		FVector2D CurrentVertex = Vertices[i];
		TArray<FVector2D> ClusterVertices;
		ClusterVertices.Add(CurrentVertex);
		for (int j = i + 1; j < Vertices.Num(); j++)
		{
			if(isVertexMerged[j]) continue;
			FVector2D DistanceVector = CurrentVertex-Vertices[j];
			const float Dist = DistanceVector.Length();
			if(Dist<MergeDistance)
			{
				ClusterVertices.Add(Vertices[j]);
				isVertexMerged[j] = true;
			}
				
		}

		FVector2D MergedPosition = FVector2D(0,0);

		for(int X = 0; X<ClusterVertices.Num(); X++)
		{
			MergedPosition +=  ClusterVertices[X];
		}

		MergedPosition /= ClusterVertices.Num();

		MergedVertices.Add(MergedPosition);
		
		
		





		
	}
	UE_LOG(LogTemp, Warning, TEXT("Number of vertices: %i"), MergedVertices.Num());
	
}

void UMyBlueprintFunctionLibrary::DrawMergedVerticesOnTexture2D(UTexture2D* Texture2D, FColor color)
{
	const int32 Width = Texture2D->GetSizeX();
	const int32 Height = Texture2D->GetSizeY();
	FByteBulkData* RawImageDataOut = &Texture2D->GetPlatformData()->Mips[0].BulkData;
	FColor* FormatedImageDataOut = static_cast<FColor*>(RawImageDataOut->Lock(LOCK_READ_WRITE));
	for(int i = 0; i<MergedVertices.Num(); i++)
	{
		int32 MergedVertexX = FMath::RoundToInt(MergedVertices[i].X);
		int32 MergedVertexY = FMath::RoundToInt(MergedVertices[i].Y);
		for (int X=0; X<Width; X++)
		{
			for(int Y =0; Y<Height; Y++)
			{
				if(FMath::Abs(X - MergedVertexX) <= 0.5f && FMath::Abs(Y - MergedVertexY) <= 0.5f)
				{
					FormatedImageDataOut[(Y * Width) + X] = color;
				}
			}
			
		}
	}
	RawImageDataOut->Unlock();
	Texture2D->UpdateResource();
}
