// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBlueprintFunctionLibrary.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "GameFramework/HUD.h"


void UMyBlueprintFunctionLibrary::SetTextureResolution(int32 width, int32 height)
{
	TextureResolutionInX = width;
	TextureResolutionInY = height;
}

UTexture2D* UMyBlueprintFunctionLibrary::CreateTexture2D()
{
	UTexture2D* Texture2D = UTexture2D::CreateTransient(TextureResolutionInX, TextureResolutionInY, PF_B8G8R8A8);
	Texture2D->Filter=TF_Nearest;
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

void UMyBlueprintFunctionLibrary::SetVoronoiAndPerlinSeed(float aSeed,float bSeed, float perlinSeed)
{
	VectorASeed = FVector2D(3.453, 2.983) + aSeed;
	AOffset = aSeed;
	AAmplitude = 10000000.0f +aSeed;
	VectorBSeed = FVector2D(5.932, 7.652) + bSeed;
	BOffset = bSeed;
	BAmplitude = 10000000.0f + bSeed;
	PerlinNoiseSeed = perlinSeed;
}

void UMyBlueprintFunctionLibrary::SetCellCount(int32 cellCount)
{
	CellCount = cellCount;
}


FVector2D UMyBlueprintFunctionLibrary::PelinNoiseGeneratePseudoRandomVector2D(FVector2D Vector2D, float perlinNoiseSeed)
{
	Vector2D.X = Vector2DtoGeneratePseudoRandomVector1D(Vector2D, FVector2D(12.32+perlinNoiseSeed, 1.235+perlinNoiseSeed), 12.3+perlinNoiseSeed, 1000000.0f+perlinNoiseSeed);
	Vector2D.Y = Vector2DtoGeneratePseudoRandomVector1D(Vector2D, FVector2D(9.322+perlinNoiseSeed, 4.031+perlinNoiseSeed),55.9+perlinNoiseSeed, 1000000.0f+perlinNoiseSeed);
	Vector2D = Vector2D*2.0f -1.0f;
	return Vector2D;
}

float UMyBlueprintFunctionLibrary::Vector2DtoGeneratePseudoRandomVector1D (FVector2D Vector2D, FVector2D a, float b, float c)
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



FVector2D UMyBlueprintFunctionLibrary::Vector2DtoGeneratePseudoRandomVector2D(FVector2D Vector2D)
{
	Vector2D.X = Vector2DtoGeneratePseudoRandomVector1D(Vector2D, VectorASeed, AOffset, AAmplitude);
	Vector2D.Y = Vector2DtoGeneratePseudoRandomVector1D(Vector2D, VectorBSeed, BOffset, BAmplitude);
	return Vector2D;
	
}



float UMyBlueprintFunctionLibrary::Vector1DtoGeneratePseudoRandomVector1D(float RandomFloatNumber, float a,float b)
{
	const float RandomNumber =  FMath::Frac(FMath::Sin(RandomFloatNumber + a)*b);
	return RandomNumber;
	
}

FVector UMyBlueprintFunctionLibrary::Vector1DtoGeneratePseudoRandomVector3D(float RandomFloatNumber)
{
	FVector Vector3D;
	Vector3D.X = Vector1DtoGeneratePseudoRandomVector1D(RandomFloatNumber, 21.289,90.92);
	Vector3D.Y = Vector1DtoGeneratePseudoRandomVector1D(RandomFloatNumber, 61.281,40.02);
	Vector3D.Z = Vector1DtoGeneratePseudoRandomVector1D(RandomFloatNumber, 23.09,80.65);
	return Vector3D;
}


TArray<FVector2D> UMyBlueprintFunctionLibrary::VoronoiSeeds;
TArray<FVector2D> UMyBlueprintFunctionLibrary::Vertices;
TArray<FVector2D> UMyBlueprintFunctionLibrary::MergedVertices;
TArray<TArray<FVector2D>> UMyBlueprintFunctionLibrary::ClosestCellVoronoiSeedXY;
TArray<FVerticesEdgesStruct> UMyBlueprintFunctionLibrary::VerticesWithUniqueCellNumber;
TArray<FVerticesEdgesStruct> UMyBlueprintFunctionLibrary::MergedCornerVerticesEdges;
TArray<FVerticesEdgesStruct> UMyBlueprintFunctionLibrary::Merged4CellCountVerticesEdges;
TMap<FVector2D, int32> UMyBlueprintFunctionLibrary::CellUniqueNumbers;
TArray<FPairedVertices> UMyBlueprintFunctionLibrary::PairedVertices;
//below for poly calculation
TArray<FVerticesEdgesStruct> UMyBlueprintFunctionLibrary::ProcessedVertices;
TMap<FVector, int32> UMyBlueprintFunctionLibrary::VertexIndexMap;
TArray<FVector> UMyBlueprintFunctionLibrary::SameCellVertices;
TMap<int32, TArray<FVector>> UMyBlueprintFunctionLibrary::CellWithVerticesArrays;
TMap<int32, TArray<FPairedVerticesWith3DInfo>> UMyBlueprintFunctionLibrary::CellWithEdgesArrays;
TArray<FCellStruct> UMyBlueprintFunctionLibrary::Cells;
TArray<FVector> UMyBlueprintFunctionLibrary::WholeVertices;
TArray<int32> UMyBlueprintFunctionLibrary::Triangles;
TArray<int32> UMyBlueprintFunctionLibrary::WholeVerticesIndex;
//TMap<FVertexData, int32> UMyBlueprintFunctionLibrary::GlobalVertexIndexMap;
TArray<FVector2D> UMyBlueprintFunctionLibrary::UV0;
TArray<FVector2D> UMyBlueprintFunctionLibrary::UV1;
TArray<FVector2D> UMyBlueprintFunctionLibrary::UV2;
TArray<FVector> UMyBlueprintFunctionLibrary::Normal;
float UMyBlueprintFunctionLibrary::UVScale = 1.0f;
//normal
float UMyBlueprintFunctionLibrary::MaxHeight;
FVector2D UMyBlueprintFunctionLibrary::CityCenterPos;
float UMyBlueprintFunctionLibrary::SigmaX;
float UMyBlueprintFunctionLibrary::SigmaY;
//UI
FVector2D UMyBlueprintFunctionLibrary::VectorASeed;
float UMyBlueprintFunctionLibrary::AOffset;
float UMyBlueprintFunctionLibrary::AAmplitude;
FVector2D UMyBlueprintFunctionLibrary::VectorBSeed;
float UMyBlueprintFunctionLibrary::BOffset;
float UMyBlueprintFunctionLibrary::BAmplitude;
int32 UMyBlueprintFunctionLibrary::CellCount;
float UMyBlueprintFunctionLibrary::RoadWidth;
float UMyBlueprintFunctionLibrary::PavementWidth;
int32 UMyBlueprintFunctionLibrary::TextureResolutionInX;
int32 UMyBlueprintFunctionLibrary::TextureResolutionInY;
int32 UMyBlueprintFunctionLibrary::PerlinNoiseSeed;
UInstancedStaticMeshComponent* UMyBlueprintFunctionLibrary::Tree;
UInstancedStaticMeshComponent* UMyBlueprintFunctionLibrary::StreetLight;

float UMyBlueprintFunctionLibrary::PerlinNoiseLerp(float l, float r, float t)
{
	t = ((6 * t - 15) * t + 10) * t * t * t;
	return FMath::Lerp(l, r, t);
}

float UMyBlueprintFunctionLibrary::PerlinNoiseCalculation(UTexture2D* Texture2D, int32 GridCount, float perlinNoiseSeed, FVector2D PixelPos)
{
	const int32 Width = Texture2D->GetSizeX();
	const int32 Height = Texture2D->GetSizeY();

	const int32 PixelInEachGridX = Width/GridCount; 
	const int32 PixelInEachGridY = Height/GridCount;
	int32 i = PixelPos.X;
	int32 j = PixelPos.Y;

	//left bottom ---A
	FVector2D CurrentGridLeftBottomIndex = FVector2D(FMath::Floor(i/(PixelInEachGridX)),FMath::Floor(j/(PixelInEachGridX)));
	FVector2D CurrentGridLeftBottomPseudoVector = PelinNoiseGeneratePseudoRandomVector2D(CurrentGridLeftBottomIndex, perlinNoiseSeed);
	FGridPointWithPseudoRandomVector CurrentLeftBottom (CurrentGridLeftBottomPseudoVector,CurrentGridLeftBottomIndex);

	//right bottom ----D
	FVector2D CurrentGridRightBottomIndex = FVector2D(CurrentLeftBottom.GridIndex.X+1, CurrentLeftBottom.GridIndex.Y);
	FVector2D CurrentGridRightBottomPseudoVector = PelinNoiseGeneratePseudoRandomVector2D(CurrentGridRightBottomIndex, perlinNoiseSeed);
	FGridPointWithPseudoRandomVector CurrentRightBottom (CurrentGridRightBottomPseudoVector,CurrentGridRightBottomIndex);

	//left top ---B
	
	FVector2D CurrentGridLeftTopIndex = FVector2D(CurrentLeftBottom.GridIndex.X, CurrentLeftBottom.GridIndex.Y+1);
	FVector2D CurrentGridLeftTopPseudoVector = PelinNoiseGeneratePseudoRandomVector2D(CurrentGridLeftTopIndex, perlinNoiseSeed);
	FGridPointWithPseudoRandomVector CurrentLeftTop (CurrentGridLeftTopPseudoVector,CurrentGridLeftTopIndex);
	
	//right top ---C
	FVector2D CurrentGridRightTopIndex = FVector2D(CurrentLeftBottom.GridIndex.X+1, CurrentLeftBottom.GridIndex.Y+1);
	FVector2D CurrentGridRightTopPseudoVector = PelinNoiseGeneratePseudoRandomVector2D(CurrentGridRightTopIndex, perlinNoiseSeed);
	FGridPointWithPseudoRandomVector CurrentRightTop (CurrentGridRightTopPseudoVector,CurrentGridRightTopIndex);

	//FVector2D PPos = FVector2D(i,j);
	FVector2D PPos = FVector2D((i-CurrentLeftBottom.GridIndex.X*PixelInEachGridX)/(PixelInEachGridX), (j-CurrentLeftBottom.GridIndex.Y*PixelInEachGridY)/(PixelInEachGridY));
	FVector2D AP = (PPos - FVector2D(0,0));
	FVector2D BP = (PPos - FVector2D(0,1));
	FVector2D CP = (PPos - FVector2D(1,1));
	FVector2D DP = (PPos - FVector2D(1,0));

	float ADotProduct = FVector2D::DotProduct(CurrentLeftBottom.GridPseudoRandomVector,AP);
	float BDotProduct = FVector2D::DotProduct(CurrentLeftTop.GridPseudoRandomVector,BP);
	float CDotProduct = FVector2D::DotProduct(CurrentRightTop.GridPseudoRandomVector, CP);
	float DDotProduct = FVector2D::DotProduct(CurrentRightBottom.GridPseudoRandomVector, DP);

	float ADLerp = PerlinNoiseLerp(ADotProduct, DDotProduct, PPos.X);
	float BCLerp = PerlinNoiseLerp(BDotProduct, CDotProduct, PPos.X);
	float NoiseValue = PerlinNoiseLerp(ADLerp, BCLerp, PPos.Y);

	NoiseValue = (NoiseValue + 1.0)/2;

	return  NoiseValue;
}

void UMyBlueprintFunctionLibrary::FractualBrownMotion(int32 octaves, int32 initialFrequency, float initialStrength,UTexture2D* Texture2D)
{
	const int32 Width = Texture2D->GetSizeX();
	const int32 Height = Texture2D->GetSizeY();
	FByteBulkData* RawImageDataOut = &Texture2D->GetPlatformData()->Mips[0].BulkData;
	FColor* FormatedImageDataOut = static_cast<FColor*>(RawImageDataOut->Lock(LOCK_READ_WRITE));


	
	for(int32 i =0;i<Width; i++)
	{
		for(int32 j =0; j<Height;j++)
		{
			
			float NoiseValue = 0;
			float frequency = initialFrequency;
			float strength = initialStrength;
			

			for (int32 k = 0; k < octaves; k++)
			{
				strength /= 2; 
				float layerNoise = PerlinNoiseCalculation(Texture2D, frequency, PerlinNoiseSeed + frequency, FVector2D(i, j));
				NoiseValue += layerNoise * strength;

				frequency *= 2; 
				
			}

			NoiseValue = FMath::Clamp(NoiseValue,0.0f,1.0f);

			FColor PixelColor = FColor(NoiseValue * 255, NoiseValue * 255, NoiseValue * 255, 255);
			FormatedImageDataOut[(j * Width) + i] = PixelColor;

			UE_LOG(LogTemp, Log, TEXT("NoiseValue at (%d, %d): %f"), i, j, NoiseValue);
			
			
	


		}
	}

	RawImageDataOut->Unlock();
	Texture2D->UpdateResource();
	
}


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


void UMyBlueprintFunctionLibrary::VoronoiCalculation(UTexture2D* Texture2D)
{
	InitializeClosestCellVoronoiSeedXY(Texture2D);
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
					const FVector2D CurrentCellPixelXY = FVector2D(PixelsInEachCellX* CurrentCellXY.X,PixelsInEachCellY* CurrentCellXY.Y);
					const int32 RandomX = FMath::Floor(FMath::Lerp(0, PixelsInEachCellX, Vector2DtoGeneratePseudoRandomVector2D(CurrentCellPixelXY).X));
					const int32 RandomY = FMath::Floor(FMath::Lerp(0, PixelsInEachCellY, Vector2DtoGeneratePseudoRandomVector2D(CurrentCellPixelXY).Y));
						
					FVector2D VoronoiSeedPixelXY = FVector2D((CurrentCellPixelXY.X + RandomX), (CurrentCellPixelXY.Y + RandomY));
					
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
			

			
			
			const float NoiseColor = Vector2DtoGeneratePseudoRandomVector1D(ClosestCell, FVector2D(289.89, 38.02), 4.98, 60.90);
			const FVector Color = Vector1DtoGeneratePseudoRandomVector3D(NoiseColor);
			const FColor PixelDrawCol = FColor(Color.X * 255, Color.Y * 255,Color.Z * 255, 255);


			const FColor PixelColor = PixelDrawCol;
			FormatedImageDataOut[(Y * Width) + X] = PixelColor;
			
		}
	}

	
	for(int X1 =0; X1<Width; X1++)
	{
		for (int Y1 =0; Y1<Height; Y1++)
		{
			ClosestCellVoronoiSeedXY[X1][0] = FVector2D(Width/2,0);
			ClosestCellVoronoiSeedXY[X1][Height - 1] = FVector2D(Width/2, Height-1);
			ClosestCellVoronoiSeedXY[0][Y1] =  FVector2D(0,Height/2);
			ClosestCellVoronoiSeedXY[Width - 1][Y1] = FVector2D(Width-1, Height/2);
		}
	}
	RawImageDataOut->Unlock();
	Texture2D->UpdateResource();
}
//
// void setPIxelvalue(int x, int y,int dx,int dy)
// {
// 	
// 	((int16*)(FormatedImageDataOut[(Y * Width) + X]))[0]=dx;
// 	((int16*)(FormatedImageDataOut[(Y * Width) + X]))[1]=dy;
// }
void UMyBlueprintFunctionLibrary::CalculateVertices(UTexture2D* Texture2D)
{
	const int32 Width = Texture2D->GetSizeX();
	const int32 Height = Texture2D->GetSizeY();
	Vertices.Empty();

	for(int X=1; X<Width-1; X++)
	{
		for (int Y =1; Y<Height-1; Y++)
		{
			
			TSet<FVector2D> UniqueSeeds;
			for(int i = -1; i <=1; i++)
			{
				for(int j = -1; j <= 1; j++)
				{
					
					FVector2D CurrentClosestSeed = ClosestCellVoronoiSeedXY[X+i][Y+j];
					
					UniqueSeeds.Add(CurrentClosestSeed);
	
				}
			}

			

			if(UniqueSeeds.Num() >= 3)
			{
				
				Vertices.Add(FVector2D(X, Y));
				
				
			}
	
		}
	}


}

void UMyBlueprintFunctionLibrary::Add4VerticesOnWholeTextureCorner(UTexture2D* Texture2D)
{
	const int32 Width = Texture2D->GetSizeX();
	const int32 Height = Texture2D->GetSizeY();
	const FVector2D UpperLeft = FVector2D(1.0, Height-2.0);
	const FVector2D DownLeft = FVector2D(1.0, 1.0);
	const FVector2D UpperRight = FVector2D(Width-2.0, Height-2.0);
	const FVector2D DownRight = FVector2D(Width-2.0, 1.0);
	Vertices.Add(UpperLeft);
	Vertices.Add(DownLeft);
	Vertices.Add(UpperRight);
	Vertices.Add(DownRight);
	
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

	
}


void UMyBlueprintFunctionLibrary::AssignCellNumbers(UTexture2D* Texture2D)
{
	const int32 Width = Texture2D->GetSizeX();
	const int32 Height = Texture2D->GetSizeY();

	int32 CellNumber = 0;
	int32 SpecialCellNumber = 10000;

	VerticesWithUniqueCellNumber.Empty();


	for (int v = 0; v < Vertices.Num(); v++)
	{
		FVerticesEdgesStruct CurrentVerticesEdgesStruct;
		CurrentVerticesEdgesStruct.VertexPosition = Vertices[v];
		CurrentVerticesEdgesStruct.CurrentCellsUniqueNumbers.Empty();

		
	
		const int32 X = Vertices[v].X;
		const int32 Y = Vertices[v].Y;

		
		for (int i = -1; i <= 1; i++)
		{
			for (int j = -1; j <= 1; j++)
			{
				

				FVector2D NeighborSeed = ClosestCellVoronoiSeedXY[X + i][Y + j];
				if(
					NeighborSeed == FVector2D(Width/2,0) ||
					NeighborSeed == FVector2D(Width/2, Height-1)||
					NeighborSeed == FVector2D(0,Height/2)||
					NeighborSeed == FVector2D(Width-1, Height/2))
				{
					{
						
						if (!CellUniqueNumbers.Contains(NeighborSeed))
						{
							CellUniqueNumbers.Add(NeighborSeed, SpecialCellNumber++);
						}
						CurrentVerticesEdgesStruct.CurrentCellsUniqueNumbers.Add(CellUniqueNumbers[NeighborSeed]);
					}
				}
				
				else
				{
					if (!CellUniqueNumbers.Contains(NeighborSeed))
					{
						CellUniqueNumbers.Add(NeighborSeed, CellNumber++);
					}
					CurrentVerticesEdgesStruct.CurrentCellsUniqueNumbers.Add(CellUniqueNumbers[NeighborSeed]);
				}
				
			}
		}

		
		
		VerticesWithUniqueCellNumber.Add(CurrentVerticesEdgesStruct);
		
	
	}
	
}

/*void UMyBlueprintFunctionLibrary::MergeCloseVertices(float MergeDistance)
{
	TArray<bool> isVertexMerged;
	isVertexMerged.Init(false, VerticesWithUniqueCellNumber.Num());
	for (int i = 0; i < VerticesWithUniqueCellNumber.Num(); i++)
	{
		if(isVertexMerged[i]) continue;
		FVerticesEdgesStruct CurrentVertexWithUniqueCellNumber = VerticesWithUniqueCellNumber[i];
		TArray<FVerticesEdgesStruct> ClusterVertices;
		TSet<int32> CellNums;
		for(int Num :VerticesWithUniqueCellNumber[i].CurrentCellsUniqueNumbers)
		{
			CellNums.Add(Num);
		}
		ClusterVertices.Add(CurrentVertexWithUniqueCellNumber);
		for (int j = i + 1; j < VerticesWithUniqueCellNumber.Num(); j++)
		{
			if(isVertexMerged[j]) continue;
			FVector2D DistanceVector = CurrentVertexWithUniqueCellNumber.VertexPosition-VerticesWithUniqueCellNumber[j].VertexPosition;
			const float Dist = DistanceVector.Length();
			if(Dist<MergeDistance)
			{
				for(int Num :VerticesWithUniqueCellNumber[j].CurrentCellsUniqueNumbers)
				{
					CellNums.Add(Num);
				}
				
				ClusterVertices.Add(VerticesWithUniqueCellNumber[j]);
				isVertexMerged[j] = true;
			}
				
		}

		FVector2D MergedPosition = FVector2D(0,0);
		FVerticesEdgesStruct MostAccurateVertex;

		for(int X = 0; X<ClusterVertices.Num(); X++)
		{
			MergedPosition +=  ClusterVertices[X].VertexPosition;
		}

		MergedPosition /= ClusterVertices.Num();
		float Distance = FLT_MAX;
		

		for(int Y = 0; Y<ClusterVertices.Num(); Y++)
		{
			FVerticesEdgesStruct CurrentVertex = ClusterVertices[Y];
			const float CurrentDist = (CurrentVertex.VertexPosition-MergedPosition).Length();
			if(CurrentDist < Distance)
			{
				Distance = CurrentDist;
				MostAccurateVertex = CurrentVertex;
			
				
			}
		}
		//MergedPosition.X = FMath::RoundToInt(MergedPosition.X);
		//MergedPosition.Y = FMath::RoundToInt(MergedPosition.Y);
		FVerticesEdgesStruct NewStruct;
		NewStruct.VertexPosition = MostAccurateVertex.VertexPosition;
		NewStruct.CurrentCellsUniqueNumbers = CellNums;
		MergedVerticesEdges.Add(NewStruct);
		
		
		





		
	}
	UE_LOG(LogTemp, Warning, TEXT("Number of vertices: %i"), MergedVerticesEdges.Num());
	
}*/


void UMyBlueprintFunctionLibrary::MergeSameCornerVertices()
{
	TArray<bool> isVertexClustered;
	isVertexClustered.Init(false, VerticesWithUniqueCellNumber.Num());
	MergedCornerVerticesEdges.Empty();
	
	for (int i = 0; i < VerticesWithUniqueCellNumber.Num(); i++)
	{
		if(isVertexClustered[i]) continue;
		FVerticesEdgesStruct CurrentVertex = VerticesWithUniqueCellNumber[i];
		
		TArray<FVerticesEdgesStruct> ToCluster;
		ToCluster.Empty();
		ToCluster.Add(CurrentVertex);
		
		for(int j = i+1; j < VerticesWithUniqueCellNumber.Num(); j++)
		{
			if(isVertexClustered[j]) continue; 
			if(CurrentVertex.IsTwoEquivalent(VerticesWithUniqueCellNumber[j]))
			{
				ToCluster.Add(VerticesWithUniqueCellNumber[j]);
				isVertexClustered[j] = true;	
			}
			
		}

		
		FVector2D MergedPosition = FVector2D(0,0);
		for(int k = 0; k<ToCluster.Num();k++)
		{
			MergedPosition += ToCluster[k].VertexPosition;
		}

		MergedPosition /= ToCluster.Num();
		float Distance = FLT_MAX;

		FVerticesEdgesStruct MostAccurateVertex;
		for(int Y = 0; Y<ToCluster.Num(); Y++)
		{
			FVerticesEdgesStruct CurrentClusterVertex = ToCluster[Y];
			const float CurrentDist = (CurrentClusterVertex.VertexPosition-MergedPosition).Length();
			if(ToCluster.Num()==2)
			{
				MostAccurateVertex = CurrentClusterVertex;
				break;
			}
			
			if(CurrentDist < Distance)
			{
				Distance = CurrentDist;
				MostAccurateVertex = CurrentClusterVertex;
			
				
			}
		}

		FVerticesEdgesStruct NewStruct = MostAccurateVertex;

		MergedCornerVerticesEdges.Add(NewStruct);
		
	}
	UE_LOG(LogTemp, Warning, TEXT("Number of vertices: %i"), MergedCornerVerticesEdges.Num());
	
}

void UMyBlueprintFunctionLibrary::Merge4CellCountVertices()
{
	TArray<bool> isVertexClustered;
	isVertexClustered.Init(false, MergedCornerVerticesEdges.Num());
	
	
	for (int32 i = 0; i < MergedCornerVerticesEdges.Num(); i++)
	{
		FVerticesEdgesStruct CurrentVertex = MergedCornerVerticesEdges[i];
		if(CurrentVertex.CurrentCellsUniqueNumbers.Num() >= 4)
		{
			for(int32 j = 0; j<MergedCornerVerticesEdges.Num(); j++)
			{
				if(j!=i && CurrentVertex.IsContainOtherArray(MergedCornerVerticesEdges[j]))
				{
					isVertexClustered[j] = true;
					
				}
			}
		}
	
	}
	for(int32 k = 0; k< MergedCornerVerticesEdges.Num();k++)
	{
		if(!isVertexClustered[k])
		{
			Merged4CellCountVerticesEdges.Add(MergedCornerVerticesEdges[k]);
		}
	}
	
	MergeSpecialCaseWithFourOrMoreVertices(Merged4CellCountVerticesEdges);
	
	UE_LOG(LogTemp, Warning, TEXT("Total vertices merged: %d"), Merged4CellCountVerticesEdges.Num());
}

void UMyBlueprintFunctionLibrary::MergeSpecialCaseWithFourOrMoreVertices(TArray<FVerticesEdgesStruct>& Array)
{

	for (int32 i = 0; i < Array.Num(); i++)
	{
		FVerticesEdgesStruct CurrentVertex = Array[i];
		if(CurrentVertex.CurrentCellsUniqueNumbers.Num() >= 4)
		{
			for(int32 j = 0; j<Array.Num(); j++)
			{
				if(j!=i && CurrentVertex.IsContainMoreThan3Element(Array[j]))
				{
					TSet<int32> CombinedCellNum;
					CombinedCellNum.Empty();
					CombinedCellNum.Append(CurrentVertex.CurrentCellsUniqueNumbers);
					CombinedCellNum.Append(Array[j].CurrentCellsUniqueNumbers);
					Array[i].CurrentCellsUniqueNumbers.Empty();
					for(int32 Element : CombinedCellNum)
					{
						Array[i].CurrentCellsUniqueNumbers.Add(Element);
					}
					
					Array.RemoveAt(j);
					
					
				}
			}
		}
	
	}
}


void UMyBlueprintFunctionLibrary::DrawMergedVerticesOnTexture2D(UTexture2D* Texture2D, FColor color)
{
	const int32 Width = Texture2D->GetSizeX();
	const int32 Height = Texture2D->GetSizeY();
	FByteBulkData* RawImageDataOut = &Texture2D->GetPlatformData()->Mips[0].BulkData;
	FColor* FormatedImageDataOut = static_cast<FColor*>(RawImageDataOut->Lock(LOCK_READ_WRITE));
	for(int i = 0; i<Merged4CellCountVerticesEdges.Num(); i++)
	{
        
		for (int X = 0; X < Width; X++)
		{
			for(int Y = 0; Y < Height; Y++)
			{
				
				if(Merged4CellCountVerticesEdges[i].VertexPosition.X == X && Merged4CellCountVerticesEdges[i].VertexPosition.Y ==Y)
				{
					FormatedImageDataOut[(Y * Width) + X] = color;
				}
			}
		}
	}
	RawImageDataOut->Unlock();
	Texture2D->UpdateResource();
}





void UMyBlueprintFunctionLibrary::PrintVertexPosAndUniqueCellNumber()
{
	for(FVerticesEdgesStruct Element :Merged4CellCountVerticesEdges)
	{
		FString VertexPositionStr = FString::Printf(TEXT("Vertex Position: (%f, %f)"), Element.VertexPosition.X, Element.VertexPosition.Y);
		FString CellNumbersStr = TEXT("Unique Cell Numbers: ");
    
		for (const int Number : Element.CurrentCellsUniqueNumbers)
		{
			CellNumbersStr += FString::Printf(TEXT("%d "), Number);
		}
    
		UE_LOG(LogTemp, Warning, TEXT("%s | %s"), *VertexPositionStr, *CellNumbersStr);
		
		
	}

	UE_LOG(LogTemp, Warning, TEXT("Number of pairs: %i"), Merged4CellCountVerticesEdges.Num());

	
}


void UMyBlueprintFunctionLibrary::DrawVertexPositionsAndCellNumbersOnHUD(AHUD* HUD, APlayerController* PlayerController)
{
	

	
	for (const FVerticesEdgesStruct& Element : Merged4CellCountVerticesEdges)
	{
		FVector2D ScreenSpace;

		PlayerController->ProjectWorldLocationToScreen(FVector(Element.VertexPosition.X, Element.VertexPosition.Y, 0.0f), ScreenSpace);

		//FString Text = FString::Printf(TEXT("Vertex Position: (%f, %f) | Cell Numbers: "), Element.VertexPosition.X, Element.VertexPosition.Y);
		FString Text;
		for (const int32 CellNumber : Element.CurrentCellsUniqueNumbers)
		{
			Text += FString::Printf(TEXT("%d "), CellNumber);
		}

		HUD->DrawText(Text,FLinearColor::White, ScreenSpace.X,ScreenSpace.Y);
		
	}
}


void UMyBlueprintFunctionLibrary::GroupVerticesWithSharedCells(UTexture2D* Texture2D)
{
	const int32 Width = Texture2D->GetSizeX();
	const int32 Height = Texture2D->GetSizeY();
	
	for(int i = 0; i<Merged4CellCountVerticesEdges.Num();i++)
	{
		FVerticesEdgesStruct CurrentVertex = Merged4CellCountVerticesEdges[i];
		int32 X = CurrentVertex.VertexPosition.X;
		int32 Y = CurrentVertex.VertexPosition.Y;
		
		for(int j = i+1; j<Merged4CellCountVerticesEdges.Num();j++)
		{
			int SharedCellNumber = 0;
			for(const int CellNumber : CurrentVertex.CurrentCellsUniqueNumbers)
			{
				if(Merged4CellCountVerticesEdges[j].CurrentCellsUniqueNumbers.Contains(CellNumber))
				{
					SharedCellNumber++;
				}
			}

			
			
		
			if(SharedCellNumber>=2)
			{
				FPairedVertices CurrentPairedVertices (CurrentVertex.VertexPosition, Merged4CellCountVerticesEdges[j].VertexPosition);
				bool bIsNewPairUnique = true;
				for(const FPairedVertices& ExistingPair : PairedVertices)
				{
					if(CurrentPairedVertices.IsEquivalent(ExistingPair))
					{
						bIsNewPairUnique = false;
						break;
					}
				}

			
				if(bIsNewPairUnique)
				{
					PairedVertices.Add(CurrentPairedVertices);
				}
			
			}
			
			
		}

		


		
	}
}

void UMyBlueprintFunctionLibrary::PrintPairedVertices()
{
	for (const FPairedVertices& PairedVertex : PairedVertices)
	{
		UE_LOG(LogTemp, Warning, TEXT("First Vertex: (%f, %f), Second Vertex: (%f, %f)"),
			PairedVertex.FirstVertex.X, PairedVertex.FirstVertex.Y,
			PairedVertex.SecondVertex.X, PairedVertex.SecondVertex.Y);
	}
}


void UMyBlueprintFunctionLibrary::DrawDebugEdges(UWorld* World)
{
	for (const FPairedVertices& PairedVertex : PairedVertices)
	{
		
		
		FVector StartPos = FVector(PairedVertex.FirstVertex.X, PairedVertex.FirstVertex.Y, 0.5f);
		FVector EndPos = FVector(PairedVertex.SecondVertex.X, PairedVertex.SecondVertex.Y, 0.5f);
		DrawDebugLine(World, StartPos, EndPos, FColor::Red, false, 10000.0f, 0,0.25f);
		
	}
	
}

//after this, the functions below are aimed to create polygons

void UMyBlueprintFunctionLibrary::ProcessVerticesForPolyCalculation(UTexture2D* Texture2D)
{
	const int32 Width = Texture2D->GetSizeX();
	const int32 Height = Texture2D->GetSizeY();
	

	for (int i =0; i<Merged4CellCountVerticesEdges.Num();i++)
	{

		FVerticesEdgesStruct CurrentVertex  = Merged4CellCountVerticesEdges[i];

		
		if ( CurrentVertex.IsWholeTextureEdge(Width,Height))
		{
			if(CurrentVertex.CurrentCellsUniqueNumbers.Contains(10000))
			{
				CurrentVertex.CurrentCellsUniqueNumbers.Remove(10000);
			}

			if(CurrentVertex.CurrentCellsUniqueNumbers.Contains(10001))
			{
				CurrentVertex.CurrentCellsUniqueNumbers.Remove(10001);
			}

			if(CurrentVertex.CurrentCellsUniqueNumbers.Contains(10002))
			{
				CurrentVertex.CurrentCellsUniqueNumbers.Remove(10002);
			}

			if(CurrentVertex.CurrentCellsUniqueNumbers.Contains(10003))
			{
				CurrentVertex.CurrentCellsUniqueNumbers.Remove(10003);
			}
		}

		ProcessedVertices.Add(CurrentVertex);
		
	}

	
}

void UMyBlueprintFunctionLibrary::AssignVertexUniqueIndex()
{
	for(int V = 0; V<ProcessedVertices.Num(); V++)
	{

		const FVerticesEdgesStruct CurrentVertex = ProcessedVertices[V];
		FVector VertexPosition = FVector(CurrentVertex.VertexPosition.X, CurrentVertex.VertexPosition.Y, 0.0f);
		if(!VertexIndexMap.Contains(VertexPosition))
		{
			int32 NewIndex = VertexIndexMap.Num();
			VertexIndexMap.Add(VertexPosition, NewIndex);
		}
	}
	
}

void UMyBlueprintFunctionLibrary::DistinguishEachCell(UTexture2D* Texture2D)
{
	

	CellWithVerticesArrays.Empty();  
	const int32 Width = Texture2D->GetSizeX();
	const int32 Height = Texture2D->GetSizeY();


	for (int i = 0; i < ProcessedVertices.Num(); i++)
	{
		const FVerticesEdgesStruct& CurrentVertex = ProcessedVertices[i];
		const FVector VtxPos(CurrentVertex.VertexPosition.X, CurrentVertex.VertexPosition.Y, 0.0f);

		
		for (int CellNumber : CurrentVertex.CurrentCellsUniqueNumbers)
		{	
			
			TArray<FVector>& VertexArray = CellWithVerticesArrays.FindOrAdd(CellNumber);
			VertexArray.AddUnique(VtxPos);  
		}
	}


	for (const auto& Pair : CellWithVerticesArrays)
	{
		const int32& CellNumber = Pair.Key;
		const TArray<FVector>& VertexArray = Pair.Value;
		FString VertexList;
		for (const FVector& Vertex : VertexArray)
		{
			VertexList += Vertex.ToString() + TEXT(" ");
		}
		UE_LOG(LogTemp, Log, TEXT("Cell %d: %s"), CellNumber, *VertexList);
	}
		
		/*CurrentCell.Centroid = FVector(0,0,0);
		
		TSet<int32> CommonElements;
		for (int32 Element : VertexInACell[0].CurrentCellsUniqueNumbers)
		{
			CommonElements.Add(Element);
		}

		
		for (int32 e = 1; e < VertexInACell.Num(); e++)
		{
			TSet<int32> CurrentSet;
			
			for (int32 Element : VertexInACell[e].CurrentCellsUniqueNumbers)
			{
				CurrentSet.Add(Element);
			}

			
			CommonElements = CommonElements.Intersect(CurrentSet);
		}

		if (CommonElements.Num() == 1)
		{
			
			int32 Element = *CommonElements.CreateConstIterator();
			CurrentCell.CellIndex = Element;
		}

		Cells.Add(CurrentCell);*/

	
	
}


void UMyBlueprintFunctionLibrary::GroupEdgesInCells()
{
	for(const FPairedVertices& VertexPair : PairedVertices)
	{

		
		for(const TPair<int32, TArray<FVector>>& Cell : CellWithVerticesArrays)
		{

			bool IsFirstVertexFound = false;
			bool IsSecondVertexFound = false;
			
			int32 CellID = Cell.Key;
			TArray<FVector> VerticesArray = Cell.Value;
			for(const FVector& Vertex : VerticesArray)
			{
				if(VertexPair.FirstVertex == FVector2D(Vertex.X, Vertex.Y))
				{
					IsFirstVertexFound = true;
				}

				if(VertexPair.SecondVertex == FVector2D(Vertex.X, Vertex.Y))
				{
					IsSecondVertexFound = true;
				}

				if(IsFirstVertexFound && IsSecondVertexFound)
				{
					break;
				}
			}

			if(IsFirstVertexFound && IsSecondVertexFound)
			{
				if(!CellWithEdgesArrays.Contains(CellID))
				{
					CellWithEdgesArrays.Add(CellID, TArray<FPairedVerticesWith3DInfo>());
				}

				FPairedVerticesWith3DInfo CurrentVertexPair;
				CurrentVertexPair.FirstVertexPosition = FVector(VertexPair.FirstVertex.X, VertexPair.FirstVertex.Y, 0);
				CurrentVertexPair.SecondVertexPosition = FVector(VertexPair.SecondVertex.X, VertexPair.SecondVertex.Y, 0);
				CellWithEdgesArrays[CellID].Add(CurrentVertexPair);
				
				
			}

		
		}
	}

	for (const TPair<int32, TArray<FPairedVerticesWith3DInfo>>& CellPair : CellWithEdgesArrays)
	{
		int32 CellID = CellPair.Key;  
		const TArray<FPairedVerticesWith3DInfo>& Edges = CellPair.Value;  

		
		UE_LOG(LogTemp, Log, TEXT("Cell ID %d has %d edges."), CellID, Edges.Num());

		
		for (const FPairedVerticesWith3DInfo& Edge : Edges)
		{
			UE_LOG(LogTemp, Log, TEXT("Edge between (%f, %f, %f) and (%f, %f, %f)"),
				   Edge.FirstVertexPosition.X, Edge.FirstVertexPosition.Y, Edge.FirstVertexPosition.Z,
				   Edge.SecondVertexPosition.X, Edge.SecondVertexPosition.Y, Edge.SecondVertexPosition.Z);
		}
	}

}

void UMyBlueprintFunctionLibrary::AssignEachCellStruct()
{

	for(const TPair<int32, TArray<FPairedVerticesWith3DInfo>> Allocation : CellWithEdgesArrays)
	{
		
		FCellStruct CurrentCell;
		CurrentCell.CellIndex = Allocation.Key;
		CurrentCell.VertexPositionPairs = Allocation.Value;
		


		TArray<FPairedVerticesWith3DInfo> PairArrays = Allocation.Value;
		TArray<FVertexIndexPairStruct> VertexIndexPairStructs;
		VertexIndexPairStructs.Empty();
		
		for(const FPairedVerticesWith3DInfo Pair: PairArrays)
		{

			FVertexIndexPairStruct CurrentVertexIndexPair;
			CurrentVertexIndexPair.FirstVertexIndex = VertexIndexMap[Pair.FirstVertexPosition];
			CurrentVertexIndexPair.SecondVertexIndex = VertexIndexMap[Pair.SecondVertexPosition];
			VertexIndexPairStructs.Add(CurrentVertexIndexPair);
			
			
			
		}

		CurrentCell.VertexIndexPairs = VertexIndexPairStructs;
		
		
		Cells.Add(CurrentCell);
		
	}
	
}



void UMyBlueprintFunctionLibrary::SortVerticesInCells()
{
	for(int i = 0; i < Cells.Num(); i++)
	{
		FCellStruct& CurrentCell = Cells[i]; 
		TArray<FVector> SortedVertices; 
		TSet<FVector> VisitedVertices; 

		if (CurrentCell.VertexPositionPairs.Num() == 0) continue;


		FVector CurrentVertex = CurrentCell.VertexPositionPairs[0].FirstVertexPosition;
		SortedVertices.Add(CurrentVertex);
		VisitedVertices.Add(CurrentVertex);

		bool bContinue = true;
		while(bContinue) {
			bContinue = false;
			for (auto& Pair : CurrentCell.VertexPositionPairs)
			{
				if (CurrentVertex == Pair.FirstVertexPosition && !VisitedVertices.Contains(Pair.SecondVertexPosition))
				{
					SortedVertices.Add(Pair.SecondVertexPosition);
					VisitedVertices.Add(Pair.SecondVertexPosition);
					CurrentVertex = Pair.SecondVertexPosition;
					bContinue = true;
					break;
				}
				else if (CurrentVertex == Pair.SecondVertexPosition && !VisitedVertices.Contains(Pair.FirstVertexPosition))
				{
					SortedVertices.Add(Pair.FirstVertexPosition);
					VisitedVertices.Add(Pair.FirstVertexPosition);
					CurrentVertex = Pair.FirstVertexPosition;
					bContinue = true;
					break;
				}
			}
		}

		
		CurrentCell.VerticesPosition = SortedVertices;
	}


	
}

void UMyBlueprintFunctionLibrary::PrintCellsArray()
{
	UE_LOG(LogTemp, Log, TEXT("Printing Cells Array..."));
	for (const FCellStruct& Cell : Cells)
	{
		FString VertexPairsInfo;
		for (const FVertexIndexPairStruct& Pair : Cell.VertexIndexPairs)
		{
			VertexPairsInfo += FString::Printf(TEXT("[(%d, %d)], "), Pair.FirstVertexIndex, Pair.SecondVertexIndex);
		}

		FString PositionPairsInfo;
		for (const FPairedVerticesWith3DInfo& Pair : Cell.VertexPositionPairs)
		{
			PositionPairsInfo += FString::Printf(TEXT("[((%s), (%s))], "), 
												 *Pair.FirstVertexPosition.ToString(), *Pair.SecondVertexPosition.ToString());
		}

		FString VtxPos;
		for (const FVector& Vertex : Cell.VerticesPosition)
		{
			VtxPos += FString::Printf(TEXT("(%s) "), *Vertex.ToString());
		}

		UE_LOG(LogTemp, Log, TEXT("Cell ID: %d, Vertex Index Pairs: %s, Vertex Position Pairs: %s, Vertices Positions: %s"),
			   Cell.CellIndex,
			   *VertexPairsInfo,
			   *PositionPairsInfo,
			   *VtxPos);
	}
	UE_LOG(LogTemp, Log, TEXT("Finished Printing Cells Array."));
}


//////////////////////////////////////////////below is for poly calculation

void UMyBlueprintFunctionLibrary::SetCityCenterHeightSigma(float maxHeight, FVector2D centerPos, float sigmaX,
	float sigmaY)
{
	MaxHeight = maxHeight;
	CityCenterPos = centerPos;
	SigmaX = sigmaX;
	SigmaY = sigmaY;
	
	
}

void UMyBlueprintFunctionLibrary::SetRoadWidth(float roadWidth)
{
	RoadWidth = roadWidth;
}

void UMyBlueprintFunctionLibrary::SetPavementWidth(float pavementWidth)
{
	PavementWidth = pavementWidth;
}

void UMyBlueprintFunctionLibrary::SetInstantiateObjects(UInstancedStaticMeshComponent* tree,
	UInstancedStaticMeshComponent* streetLight)
{
	Tree = tree;
	StreetLight = streetLight;
}

float UMyBlueprintFunctionLibrary::UseNormalDistributionToGetBuildingHeight(FVector2D CurrentPos)
{
	const float exponent = FMath::Exp(-(FMath::Square(CurrentPos.X - CityCenterPos.X) / (2 * SigmaX * SigmaX) 
							+ FMath::Square(CurrentPos.Y - CityCenterPos.Y) / (2 * SigmaY * SigmaY)));
	UE_LOG(LogTemp, Warning, TEXT("exponent: %f"), exponent);

	return MaxHeight * exponent;
	
	
}


float UMyBlueprintFunctionLibrary::CalculatePolygonArea(const TArray<FVector>& VerticesPos, EPlane Plane)
{
	float Area = 0.0f;
	int N = VerticesPos.Num();
	for (int i = 0; i < N; i++)
	{
		const FVector& Current = VerticesPos[i];
		const FVector& Next = VerticesPos[(i + 1) % N];
		switch (Plane) {
		case EPlane::XY:
			Area += (Current.X * Next.Y - Next.X * Current.Y);
			break;
		case EPlane::XZ:
			Area += (Current.X * Next.Z - Next.X * Current.Z);
			break;
		case EPlane::YZ:
			Area += (Current.Y * Next.Z - Next.Y * Current.Z);
			break;
		}
	}
	return 0.5f * Area;
}

void UMyBlueprintFunctionLibrary::CheckWindingOrder(TArray<FVertexData>& VtxData, EPlane Plane)
{
	TArray<FVector> VerticesForAreaCalc;
	for (const FVertexData EachVtxData : VtxData)
	{
		VerticesForAreaCalc.Add(EachVtxData.VtxPos);
	}
	const float Area = CalculatePolygonArea(VerticesForAreaCalc, Plane);
	if (Area > 0)  
	{
		Algo::Reverse(VtxData);
		
	}
	
}

void UMyBlueprintFunctionLibrary::MakeTriangle(TArray<int32> VtxIndex)
{
	   
	/*for (int i = 0; i < VtxIndex.Num(); i += 3)
	{
		Triangles.Add(VtxIndex[i]);
		Triangles.Add(VtxIndex[i + 1]);
		Triangles.Add(VtxIndex[i + 2]);
	}*/
	
    

}


int32 UMyBlueprintFunctionLibrary::AddVertex(FVertexData VertexData)
{
	
	const int32 VertexIndex = WholeVertices.Num();
	VertexData.VtxIndex = VertexIndex;
	
	WholeVertices.Add(VertexData.VtxPos);
	Normal.Add(VertexData.VtxNormal);
	UV0.Add(VertexData.VtxUV0);
	UV1.Add(VertexData.VtxUV1);
	UV2.Add(VertexData.VtxUV2);
	
	return VertexData.VtxIndex;
}

void  UMyBlueprintFunctionLibrary::DivideQuadIntoTriangle(TArray<int32> TwoBaseVerticesIndex, TArray<int32> TwoMiddleVerticesIndex, TArray<int32>& DownLeftTriangle, TArray<int32>& DownRightTriangle)
{


	DownLeftTriangle.Add(TwoMiddleVerticesIndex[1]);
	DownLeftTriangle.Add(TwoMiddleVerticesIndex[0]);
	DownLeftTriangle.Add(TwoBaseVerticesIndex[0]);
	
	

	DownRightTriangle.Add(TwoMiddleVerticesIndex[1]);
	DownRightTriangle.Add(TwoBaseVerticesIndex[0]);
	DownRightTriangle.Add(TwoBaseVerticesIndex[1]);
	

	
}

FVector UMyBlueprintFunctionLibrary::RoundVector(FVector Vec, float Precision)
{
	return FVector(FMath::RoundToFloat(Vec.X / Precision) * Precision,
				   FMath::RoundToFloat(Vec.Y / Precision) * Precision,
				   FMath::RoundToFloat(Vec.Z / Precision) * Precision);
}

void UMyBlueprintFunctionLibrary::ExtrudePolygon(TArray<int32> BaseTriangle, TArray<FVertexData> BaseVtxData, TArray<int32>& ExtrudeTriangles)
{
	
	ExtrudeTriangles.Empty();
	
	TArray<int32> NewVtxIndexArray;
	NewVtxIndexArray.Empty();
	TArray<FVertexData> NewVtxDataArray;
	NewVtxDataArray.Empty();
	//setup building height
	FVector2D BuildingCenter = FVector2D(BaseVtxData[0].VtxPos.X,BaseVtxData[0].VtxPos.Y);
	float BuildingHeight = UseNormalDistributionToGetBuildingHeight(BuildingCenter);
	float FakeNoise = (0.75f +(1.25f-0.85f)*Vector2DtoGeneratePseudoRandomVector1D(BuildingCenter, FVector2D(2.463, 10.313), 6.24, 19.02));
	BuildingHeight*=FakeNoise;
	
	for(int i =0; i<BaseVtxData.Num();i++)
	{
		FVector TopNewVtxPos = BaseVtxData[i].VtxPos;
		//float Height = 20.0f;
		TopNewVtxPos.Z += BuildingHeight;
		FVector NewVtxNormal = FVector(0,0,1);
		FVector2D NewVtxUV0 = FVector2D(0,0);
		FVector2D NewVtxUV1 = FVector2D(TopNewVtxPos.X/UVScale, TopNewVtxPos.Y/UVScale);
		//roof
		FVector2D NewVtxUV2 = FVector2D(1,0);
		FVertexData NewVtxData(TopNewVtxPos, 0, NewVtxUV0, NewVtxUV1, NewVtxUV2,NewVtxNormal );
		NewVtxData.VtxIndex = AddVertex(NewVtxData);
		NewVtxIndexArray.Add(NewVtxData.VtxIndex);
		NewVtxDataArray.Add(NewVtxData);
		
	}


	TArray<int32> TwoBaseVtx;
	TwoBaseVtx.Empty();
	//first
	FVector BaseFirstVtxPos = BaseVtxData[1].VtxPos;
	FVector2D BaseFirstVtxUV0 = FVector2D(0,0);
	FVector2D BaseFirstVtxUV1 = FVector2D(BaseFirstVtxPos.X/UVScale, BaseFirstVtxPos.Z/UVScale);
	//building side
	FVector2D BaseFirstVtxUV2  = FVector2D(0.5,0);
	FVector Upward = FVector(0,0,1);
	FVector V0 = BaseVtxData[2].VtxPos - BaseVtxData[1].VtxPos;
	FVector BaseFirstVtxNormal = FVector::CrossProduct(Upward, V0).GetSafeNormal();

	FVertexData BaseFirstVtxData(BaseFirstVtxPos, 0,BaseFirstVtxUV0,BaseFirstVtxUV1, BaseFirstVtxUV2 ,BaseFirstVtxNormal );
	BaseFirstVtxData.VtxIndex = AddVertex(BaseFirstVtxData);

	//second
	FVector BaseSecondVtxPos = BaseVtxData[2].VtxPos;
	float Dist3 =(BaseSecondVtxPos - BaseFirstVtxPos).Length();
	FVector2D BaseSecondVtxUV0 = FVector2D(1*Dist3,0);
	FVector2D BaseSecondVtxUV1 = FVector2D(BaseSecondVtxPos.X/UVScale, BaseSecondVtxPos.Z/UVScale);
	//building side
	FVector2D BaseSecondVtxUV2  = FVector2D(0.5,0);
	//FVector V1 = BaseVtxData[1].VtxPos - BaseVtxData[2].VtxPos;
	//FVector BaseSecondVtxNormal = FVector::CrossProduct(Upward, V1).GetSafeNormal();

	FVertexData BaseSecondVtxData(BaseSecondVtxPos, 0,BaseSecondVtxUV0,BaseSecondVtxUV1, BaseSecondVtxUV2,BaseFirstVtxNormal );
	BaseSecondVtxData.VtxIndex = AddVertex(BaseSecondVtxData);

	
	TwoBaseVtx.Add(BaseFirstVtxData.VtxIndex);
	TwoBaseVtx.Add(BaseSecondVtxData.VtxIndex);


	TArray<int32> TwoTopVtx;
	TwoTopVtx.Empty();

	//top first
	FVector TopFirstVtxPos = NewVtxDataArray[1].VtxPos;
	FVector2D TopFirstVtxUV0 = FVector2D(0,1*BuildingHeight);
	FVector2D TopFirstVtxUV1 = FVector2D(TopFirstVtxPos.X/UVScale, TopFirstVtxPos.Z/UVScale);
	//building side
	FVector2D TopFirstVtxUV2  = FVector2D(0.5,0);
	//FVector V2 = NewVtxDataArray[2].VtxPos - NewVtxDataArray[1].VtxPos;
	//FVector TopFirstVtxNormal = FVector::CrossProduct(Upward, V2).GetSafeNormal();

	FVertexData TopFirstVtxData(TopFirstVtxPos, 0,TopFirstVtxUV0 ,TopFirstVtxUV1, TopFirstVtxUV2,BaseFirstVtxNormal);
	TopFirstVtxData.VtxIndex = AddVertex(TopFirstVtxData);

	//top second
	FVector TopSecondVtxPos = NewVtxDataArray[2].VtxPos;
	FVector2D TopSecondVtxUV0 = FVector2D(1*Dist3,1*BuildingHeight);
	FVector2D TopSecondVtxUV1 = FVector2D(TopSecondVtxPos.X/UVScale, TopSecondVtxPos.Z/UVScale);
	//building side
	FVector2D TopSecondVtxUV2  = FVector2D(0.5,0);
	//FVector V3 = NewVtxDataArray[1].VtxPos - NewVtxDataArray[2].VtxPos;
	//FVector TopSecondVtxNormal = FVector::CrossProduct(Upward, V3).GetSafeNormal();

	FVertexData TopSecondVtxData(TopSecondVtxPos, 0,TopSecondVtxUV0 ,TopSecondVtxUV1, TopSecondVtxUV2 ,BaseFirstVtxNormal);
	TopSecondVtxData.VtxIndex = AddVertex(TopSecondVtxData);

	

	TwoTopVtx.Add(TopFirstVtxData.VtxIndex);
	TwoTopVtx.Add(TopSecondVtxData.VtxIndex);

	TArray<int32> FirstSideTriangle;
	FirstSideTriangle.Empty();
	TArray<int32> SecondSideTriangle;
	SecondSideTriangle.Empty();

	DivideQuadIntoTriangle(TwoBaseVtx, TwoTopVtx, FirstSideTriangle, SecondSideTriangle);

	
	ExtrudeTriangles.Append(NewVtxIndexArray);
	ExtrudeTriangles.Append(FirstSideTriangle);
	ExtrudeTriangles.Append(SecondSideTriangle);

	
}

FVector UMyBlueprintFunctionLibrary::CalculateBisector(FVector VtxA, FVector VtxB, FVector VtxC, bool IsFirstSubdivide)
{
	
	const FVector DirBA = (VtxA - VtxB).GetSafeNormal();
	const FVector DirBC = (VtxC - VtxB).GetSafeNormal();
	
	FVector BiSector = (DirBA + DirBC).GetSafeNormal();

	//see the angle is convex or concave
	const float CrossProductZ = DirBA.X * DirBC.Y - DirBA.Y * DirBC.X;
	const float DotProduct = FVector::DotProduct(DirBA, DirBC);

	//if two vector is opposite(angle 180), rotate the first vector clockwise
	if(DotProduct <= -0.99 && DotProduct >= -1.01)
	{
		BiSector = FVector(-DirBA.Y, DirBA.X, 0.0f);
	}

	//if angle is concave
	else if(CrossProductZ < 0.0f)
	{
		BiSector  = -BiSector;
	}
	const float Angle = FMath::Acos(FMath::Clamp(DotProduct, -1.0f, 1.0f));
	const float HalfAngle = Angle/2;
	float Length = RoadWidth/FMath::Sin(HalfAngle);
	if(!IsFirstSubdivide)
	{
		Length = (PavementWidth+RoadWidth)/FMath::Sin(HalfAngle);
	}

	const FVector BisectorWithLength = BiSector * Length + VtxB;
	UE_LOG(LogTemp, Log, TEXT("DirBA: %s, DirBC: %s, BiSector: %s, Angle: %f, HalfAngle: %f, Length: %f, BisctorWithLenghth: %s"),
	*DirBA.ToString(), *DirBC.ToString(), *BiSector.ToString(), FMath::RadiansToDegrees(Angle), FMath::RadiansToDegrees(HalfAngle), Length, *BisectorWithLength.ToString());
	
	return BisectorWithLength;
	
	 
}

void UMyBlueprintFunctionLibrary::InstantiateObject(UInstancedStaticMeshComponent* StaticMesh, FVector InstanceLocation)
{
	InstanceLocation.Z+=0.01;
	const FTransform InstantiateTransform  = FTransform(FRotator(0,0,0),InstanceLocation, FVector(0.005, 0.005,0.005));

	StaticMesh->AddInstance(InstantiateTransform);

}

void UMyBlueprintFunctionLibrary::ScatterPointsInSquare(FVector BaseLeftPos, FVector BaseRightPos, FVector TopLeftPos,FVector TopRightPos,int32 NumberOfObjects)
{
	constexpr int32 Slider = 0.2;
	const FVector LeftMiddlePos = BaseLeftPos+Slider*(TopLeftPos-BaseLeftPos);
	const FVector RightMiddlePos = BaseRightPos+Slider*(TopRightPos-BaseRightPos);
	//divide into n points
	
	const FVector Dist = RightMiddlePos - LeftMiddlePos;
	for(int i = 1; i<NumberOfObjects; i++)
	{
		const float Ratio = static_cast<float>(i) / static_cast<float>(NumberOfObjects);
		const FVector InstantiateLocation = LeftMiddlePos + Ratio*Dist;
		if(i%2!=0)
		{
			InstantiateObject(Tree, InstantiateLocation);
		}

		else
		{
			InstantiateObject(StreetLight, InstantiateLocation);
		}
		
	}
	
}


void UMyBlueprintFunctionLibrary::TriangleFanSecondSubdivide(FVector Pos1, FVector Pos2, TArray<int32> VtxIndex, TArray<FVertexData> VtxData, bool IsEdge, TArray<int32>& MidLeftTriangle, TArray<int32>& MidRightTriangle, TArray<int32>& TopTriangleToExtrude, TArray<FVertexData>& TopTriangleDataToExtrude)
{
	TArray<int32> TwoBase;
	TwoBase.Empty();
	
	TArray<int32> TwoMid;
	TwoMid.Empty();

	const FVector CenterPos = VtxData[0].VtxPos;
	const FVector FirstPos = VtxData[1].VtxPos;
	const FVector SecondPos = VtxData[2].VtxPos;

	//mid first and center
	FVector MidBetweenCenterAndFirstVtxPos = Pos1;
	FVector MidBetweenCenterAndFirstVtxNormal = FVector(0,0,1);
	FVector V0 = MidBetweenCenterAndFirstVtxPos - FirstPos;
	FVector V1 = SecondPos - FirstPos;
	float DotProduct = FVector::DotProduct(V0, V1);
	float BaseLength = V1.Length();
	float FirstProjectionLength = DotProduct / BaseLength;
	//float Dist1 =FMath::Sqrt(FMath::Pow((MidBetweenCenterAndFirstVtxPos-FirstPos).Length(), 2) - FMath::Pow(FirstProjectionLength ,2));
	FVector2D MidBetweenCenterAndFirstVtxUV0 = FVector2D(1*FirstProjectionLength,1);
	FVector2D MidBetweenCenterAndFirstVtxUV1 = FVector2D(MidBetweenCenterAndFirstVtxPos.X/UVScale, MidBetweenCenterAndFirstVtxPos.Y/UVScale);
	FVector2D MidBetweenCenterAndFirstVtxUV2 = FVector2D(2.0,0);
	if(IsEdge)
	{
		MidBetweenCenterAndFirstVtxUV2 = FVector2D(1.5,0);
	}
	FVertexData MidBetweenCenterAndFirstVtxData(MidBetweenCenterAndFirstVtxPos, 0, MidBetweenCenterAndFirstVtxUV0, MidBetweenCenterAndFirstVtxUV1, MidBetweenCenterAndFirstVtxUV2, MidBetweenCenterAndFirstVtxNormal);
	MidBetweenCenterAndFirstVtxData.VtxIndex = AddVertex(MidBetweenCenterAndFirstVtxData);

	//mid second and center
	FVector MidBetweenCenterAndSecondVtxPos = Pos2;
	FVector MidBetweenCenterAndSecondVtxNormal = FVector(0,0,1);
	FVector V2 = MidBetweenCenterAndSecondVtxPos - SecondPos;
	FVector V3 = FirstPos - SecondPos;
	float DotProduct1 = FVector::DotProduct(V2, V3);
	float BaseLength2 = V3.Length();
	float SecondProjectionLength = DotProduct1 / BaseLength2;
	//float Dist2 =FMath::Sqrt(FMath::Pow((MidBetweenCenterAndSecondVtxPos-SecondPos).Length(), 2) - FMath::Pow(SecondProjectionLength,2));
	FVector2D MidBetweenCenterAndSecondVtxUV0 = FVector2D(BaseLength2 -1*SecondProjectionLength,1);
	FVector2D MidBetweenCenterAndSecondVtxUV1 = FVector2D(MidBetweenCenterAndSecondVtxPos.X/UVScale, MidBetweenCenterAndSecondVtxPos.Y/UVScale);
	FVector2D MidBetweenCenterAndSecondVtxUV2 = FVector2D(2.0,0);
	if(IsEdge)
	{
		MidBetweenCenterAndSecondVtxUV2 = FVector2D(1.5,0);
	}
	FVertexData MidBetweenCenterAndSecondVtxData(MidBetweenCenterAndSecondVtxPos, 0,MidBetweenCenterAndSecondVtxUV0,MidBetweenCenterAndSecondVtxUV1, MidBetweenCenterAndSecondVtxUV2,MidBetweenCenterAndSecondVtxNormal);
	MidBetweenCenterAndSecondVtxData.VtxIndex = AddVertex(MidBetweenCenterAndSecondVtxData);
	
	
	//change uv2
	FVertexData BaseLeftVtxData = VtxData[1];
	BaseLeftVtxData.VtxUV0 = FVector2D(0.0,0.0);
	BaseLeftVtxData.VtxUV2 = FVector2D(2.0, 0);
	BaseLeftVtxData.VtxIndex = AddVertex(BaseLeftVtxData);
	TwoBase.Add(BaseLeftVtxData.VtxIndex);

	FVertexData BaseRightVtxData = VtxData[2];
	BaseRightVtxData.VtxUV0 = FVector2D(1.0,0.0);
	BaseRightVtxData.VtxUV2 = FVector2D(2.0, 0);
	BaseRightVtxData.VtxIndex = AddVertex(BaseRightVtxData);
	TwoBase.Add(BaseRightVtxData.VtxIndex);
	

	TwoMid.Add(MidBetweenCenterAndFirstVtxData.VtxIndex);
	TwoMid.Add(MidBetweenCenterAndSecondVtxData.VtxIndex);
	
	//

	TopTriangleToExtrude.Empty();

	TopTriangleToExtrude.Add(VtxIndex[0]);
	TopTriangleToExtrude.Add(MidBetweenCenterAndFirstVtxData.VtxIndex);
	TopTriangleToExtrude.Add(MidBetweenCenterAndSecondVtxData.VtxIndex);

	TopTriangleDataToExtrude.Empty();
	TopTriangleDataToExtrude.Add(VtxData[0]);
	TopTriangleDataToExtrude.Add(MidBetweenCenterAndFirstVtxData);
	TopTriangleDataToExtrude.Add(MidBetweenCenterAndSecondVtxData);

	

	//
	MidLeftTriangle.Empty();
	
	MidRightTriangle.Empty();

	

	//create quad
	
	DivideQuadIntoTriangle(TwoBase, TwoMid, MidLeftTriangle,MidRightTriangle);

	ScatterPointsInSquare(BaseLeftVtxData.VtxPos, BaseRightVtxData.VtxPos, MidBetweenCenterAndFirstVtxData.VtxPos,MidBetweenCenterAndSecondVtxData.VtxPos, 4 );

}

void UMyBlueprintFunctionLibrary::TriangleFanFirstSubdivide(FVertexData PreVtx, FVertexData NextNextVtx, TArray<int32> VtxIndex, TArray<FVertexData> VtxData, bool IsEdge)
{
	TArray<int32> TwoBase;
	TwoBase.Empty();
	
	TArray<int32> TwoMid;
	TwoMid.Empty();

	const FVector CenterPos = VtxData[0].VtxPos;
	const FVector FirstPos = VtxData[1].VtxPos;
	const FVector SecondPos = VtxData[2].VtxPos;

	//mid first and center
	FVector MidBetweenCenterAndFirstVtxPos = CalculateBisector(PreVtx.VtxPos,FirstPos,SecondPos, true);
	FVector MidBetweenCenterAndFirstVtxNormal = FVector(0,0,1);
	FVector V0 = MidBetweenCenterAndFirstVtxPos - FirstPos;
	FVector V1 = SecondPos - FirstPos;
	float DotProduct = FVector::DotProduct(V0, V1);
	float BaseLength = V1.Length();
	float FirstProjectionLength = DotProduct / BaseLength;
	//float Dist1 =FMath::Sqrt(FMath::Pow((MidBetweenCenterAndFirstVtxPos-FirstPos).Length(), 2) - FMath::Pow(FirstProjectionLength ,2));
	FVector2D MidBetweenCenterAndFirstVtxUV0 = FVector2D(1*FirstProjectionLength,1);
	FVector2D MidBetweenCenterAndFirstVtxUV1 = FVector2D(MidBetweenCenterAndFirstVtxPos.X/UVScale, MidBetweenCenterAndFirstVtxPos.Y/UVScale);
	FVector2D MidBetweenCenterAndFirstVtxUV2 = FVector2D(0,0);
	if(IsEdge)
	{
		MidBetweenCenterAndFirstVtxUV2 = FVector2D(1.5,0);
	}
	FVertexData MidBetweenCenterAndFirstVtxData(MidBetweenCenterAndFirstVtxPos, 0, MidBetweenCenterAndFirstVtxUV0, MidBetweenCenterAndFirstVtxUV1, MidBetweenCenterAndFirstVtxUV2, MidBetweenCenterAndFirstVtxNormal);
	MidBetweenCenterAndFirstVtxData.VtxIndex = AddVertex(MidBetweenCenterAndFirstVtxData);

	//mid second and center
	FVector MidBetweenCenterAndSecondVtxPos = CalculateBisector(FirstPos, SecondPos, NextNextVtx.VtxPos, true);
	FVector MidBetweenCenterAndSecondVtxNormal = FVector(0,0,1);
	FVector V2 = MidBetweenCenterAndSecondVtxPos - SecondPos;
	FVector V3 = FirstPos - SecondPos;
	float DotProduct1 = FVector::DotProduct(V2, V3);
	float BaseLength2 = V3.Length();
	float SecondProjectionLength = DotProduct1 / BaseLength2;
	//float Dist2 =FMath::Sqrt(FMath::Pow((MidBetweenCenterAndSecondVtxPos-SecondPos).Length(), 2) - FMath::Pow(SecondProjectionLength,2));
	FVector2D MidBetweenCenterAndSecondVtxUV0 = FVector2D(BaseLength2 -1*SecondProjectionLength,1);
	FVector2D MidBetweenCenterAndSecondVtxUV1 = FVector2D(MidBetweenCenterAndSecondVtxPos.X/UVScale, MidBetweenCenterAndSecondVtxPos.Y/UVScale);
	FVector2D MidBetweenCenterAndSecondVtxUV2 = FVector2D(0,0);
	if(IsEdge)
	{
		MidBetweenCenterAndSecondVtxUV2 = FVector2D(1.5,0);
	}
	FVertexData MidBetweenCenterAndSecondVtxData(MidBetweenCenterAndSecondVtxPos, 0,MidBetweenCenterAndSecondVtxUV0,MidBetweenCenterAndSecondVtxUV1, MidBetweenCenterAndSecondVtxUV2,MidBetweenCenterAndSecondVtxNormal);
	MidBetweenCenterAndSecondVtxData.VtxIndex = AddVertex(MidBetweenCenterAndSecondVtxData);
	
	

	TwoBase.Add(VtxIndex[1]);
	TwoBase.Add(VtxIndex[2]);

	TwoMid.Add(MidBetweenCenterAndFirstVtxData.VtxIndex);
	TwoMid.Add(MidBetweenCenterAndSecondVtxData.VtxIndex);

	
	//
	TArray<int32> AllTriangles;
	AllTriangles.Empty();
	
	//deliver to subdivide 2
	TArray<int32> TopTriangle;
	TopTriangle.Empty();

	TopTriangle.Add(VtxIndex[0]);
	TopTriangle.Add(MidBetweenCenterAndFirstVtxData.VtxIndex);
	TopTriangle.Add(MidBetweenCenterAndSecondVtxData.VtxIndex);

	TArray<FVertexData> TopTriangleVtxData;
	TopTriangleVtxData.Empty();
	TopTriangleVtxData.Add(VtxData[0]);
	TopTriangleVtxData.Add(MidBetweenCenterAndFirstVtxData);
	TopTriangleVtxData.Add(MidBetweenCenterAndSecondVtxData);
	

	

	//
	TArray<int32> DownLeftTriangle;
	DownLeftTriangle.Empty();

	

	TArray<int32> DownRightTriangle;
	DownRightTriangle.Empty();

	//create quad
	
	DivideQuadIntoTriangle(TwoBase, TwoMid, DownLeftTriangle,DownRightTriangle );


	//
	//AllTriangles.Append(TopTriangle);
	AllTriangles.Append(DownLeftTriangle);
	AllTriangles.Append(DownRightTriangle);

	//subdivide 2
	TArray<int32> TopTriangleExtrude;
	TopTriangleExtrude.Empty();
	TArray<int32> MiddleLeftTriangle;
	MiddleLeftTriangle.Empty();
	TArray<int32> MiddleRightTriangle;
	MiddleRightTriangle.Empty();
	TArray<FVertexData> VertexDataToExtrude;
	VertexDataToExtrude.Empty();
	

	//set up midpoint pos for the subdivide 2
	FVector LeftVtxPos = CalculateBisector(PreVtx.VtxPos,FirstPos,SecondPos, false);
	FVector RightVtxPos = CalculateBisector(FirstPos, SecondPos, NextNextVtx.VtxPos,  false);
	TriangleFanSecondSubdivide(LeftVtxPos, RightVtxPos, TopTriangle, TopTriangleVtxData, IsEdge, MiddleLeftTriangle, MiddleRightTriangle,TopTriangleExtrude, VertexDataToExtrude);

	//add triangles
	AllTriangles.Append(TopTriangleExtrude);
	AllTriangles.Append(MiddleLeftTriangle);
	AllTriangles.Append(MiddleRightTriangle);
	
	
	TArray<int32> ExtrudeTriangles;
	ExtrudeTriangles.Empty();



	//if it not beach, dont create building
	if(!IsEdge)
	{
		ExtrudePolygon(TopTriangleExtrude,VertexDataToExtrude,ExtrudeTriangles);
	}
	
	
	AllTriangles.Append(ExtrudeTriangles);

	Triangles.Append(AllTriangles);
	
	

	
}

FVector UMyBlueprintFunctionLibrary::FindKeyByValue(const TMap<FVector, int32>& Map, int32 ValueToFind)
{
	for (const TPair<FVector, int32>& Pair : Map)
	{
		if (Pair.Value == ValueToFind)
		{
			return Pair.Key;
		}
	}
	return FVector();
}

void UMyBlueprintFunctionLibrary::CreateVoronoiShapePolygon(UProceduralMeshComponent* ProceduralMesh, UMaterialInterface* MaterialInstance)
{
	WholeVertices.Empty();
	WholeVerticesIndex.Empty();
	//GlobalVertexIndexMap.Empty();
	Triangles.Empty();
	UV0.Empty();
	UV1.Empty();
	UV2.Empty();
	Normal.Empty();

	//this array to store the vtx that are not the beach edge to sea, but the edge between road and beach
	TArray<FVector> BeachInnerVtx;
	BeachInnerVtx.Empty();

	

	for(FCellStruct& CurrentCell : Cells)
	{
		bool IsThisCellOnEdge = false;

		
		
		//check if the cell on the edge, make the beach
		for(int i =0;i<CurrentCell.VerticesPosition.Num(); i++)
		{

			if(CurrentCell.VerticesPosition[i].X== 1||CurrentCell.VerticesPosition[i].X== TextureResolutionInX-2 ||CurrentCell.VerticesPosition[i].Y ==1 || CurrentCell.VerticesPosition[i].Y ==TextureResolutionInY -2 )
			{
				IsThisCellOnEdge =  true;
				//create slope
				CurrentCell.VerticesPosition[i].Z -= 5.0f;
			}
			
		}
		//add beach vtx between road and beach
		if(IsThisCellOnEdge)
		{
			for(int i =0;i<CurrentCell.VerticesPosition.Num(); i++)
			{

				if(CurrentCell.VerticesPosition[i].X!= 1&&CurrentCell.VerticesPosition[i].X!= TextureResolutionInX-2&&CurrentCell.VerticesPosition[i].Y !=1 && CurrentCell.VerticesPosition[i].Y !=TextureResolutionInY -2 )
				{
					BeachInnerVtx.Add(CurrentCell.VerticesPosition[i]);
					
				}
			
			}
		}
		

		
		//get center point 
		FVector CentroidPos = CurrentCell.CalculateIncenter();
		FVector CenterNormal(0, 0, 1); 
		FVector2D CenterUV0(0.0f, 0.0f); 
		FVector2D CenterUV1(CentroidPos.X / UVScale, CentroidPos.Y / UVScale);
		//road
		FVector2D CenterUV2(0, 0);
		if(IsThisCellOnEdge)
		{
			CenterUV2 = FVector2D(1.5,0);
		}
		
		FVertexData CentroidVertexData(CentroidPos,0, CenterUV0,CenterUV1,CenterUV2, CenterNormal );
		CentroidVertexData.VtxIndex = AddVertex(CentroidVertexData);
		
		//
		TArray<FVertexData> CellVerticesData;
		CellVerticesData.Empty();

		//map index
		for(int i =0;i<CurrentCell.VerticesPosition.Num(); i++)
		{
			FVector VtxPos = CurrentCell.VerticesPosition[i];
			FVector VtxNormal(0, 0, 1); 
			FVector2D VtxUV0(0.0f, 0.0f); 
			FVector2D VtxUV1(VtxPos.X / UVScale, VtxPos.Y / UVScale); 
			FVector2D VtxUV2(0, 0);
			if(IsThisCellOnEdge)
			{
				VtxUV2 = FVector2D(1.5,0);
			}

			FVertexData CurrentVtxData(VtxPos, 0, VtxUV0, VtxUV1, VtxUV2, VtxNormal);
			CurrentVtxData.VtxIndex = AddVertex(CurrentVtxData);
			CellVerticesData.Add(CurrentVtxData);
			
		}
		
		//check winding order
		CheckWindingOrder(CellVerticesData, EPlane::XY);

		
		
		//for each triangle
		for(int i =0;i<CellVerticesData.Num();i++)
		{
			TArray<FVertexData> SingleTriangleData;
			SingleTriangleData.Empty();
			FVertexData CenterVtxData = CentroidVertexData;
			FVertexData FirstVtxData = CellVerticesData[i];
			FVertexData NextVtxData = CellVerticesData[(i+1)%CellVerticesData.Num()];

			//to calculate the bisectors
			FVertexData PrevVtxData = CellVerticesData[(i - 1 + CellVerticesData.Num()) % CellVerticesData.Num()];
			FVertexData NextNextVtxData = CellVerticesData[(i + 2) % CellVerticesData.Num()];

			//change the UV from(0,0) to(1,0)
			if(NextVtxData.VtxUV0 == FVector2D(0.0f,0.0f))
			{
				float Dist = (NextVtxData.VtxPos - FirstVtxData.VtxPos).Length();
				NextVtxData.VtxUV0 = FVector2D(1*Dist,0.0f);
				NextVtxData.VtxIndex = AddVertex(NextVtxData);
			}
			
			SingleTriangleData.Add(CenterVtxData);
			SingleTriangleData.Add(FirstVtxData);
			SingleTriangleData.Add(NextVtxData);

			TArray<int32> SingleTriangleIndex;
			SingleTriangleIndex.Empty();
			
			const int32 CenterIndex = CentroidVertexData.VtxIndex;
			const int32 FirstVtxIndex = FirstVtxData.VtxIndex;
			const int32 NextVtxIndex = NextVtxData.VtxIndex;
			SingleTriangleIndex.Add(CenterIndex); 
			SingleTriangleIndex.Add(FirstVtxIndex);
			SingleTriangleIndex.Add(NextVtxIndex);


			TriangleFanFirstSubdivide(PrevVtxData, NextNextVtxData,SingleTriangleIndex,SingleTriangleData,IsThisCellOnEdge);
			
		}
	}
	//blend uv2 from road to beach
	AdjustNearCellUV2ToMakeBeach(BeachInnerVtx);
	ProceduralMesh->CreateMeshSection(0, WholeVertices, Triangles, Normal, UV0,UV1,UV2,UV0, TArray<FColor>(), TArray<FProcMeshTangent>(), true);
	
	if (MaterialInstance)
	{
		ProceduralMesh->SetMaterial(0, MaterialInstance);
	}
	
}

void UMyBlueprintFunctionLibrary::AdjustNearCellUV2ToMakeBeach(TArray<FVector> vtx)
{
	for(int i =0; i<WholeVertices.Num();i++)
	{
		for(int j =0; j <vtx.Num(); j++)
		{
			if(WholeVertices[i] == vtx[j])
			{
				UV2[i]= FVector2D(1.5,0);
			}
		}
	}
}


void UMyBlueprintFunctionLibrary::ClearAllArrays()
{

	VoronoiSeeds.Empty();
	Vertices.Empty();
	MergedVertices.Empty();
	ClosestCellVoronoiSeedXY.Empty();
    VerticesWithUniqueCellNumber.Empty();
	MergedCornerVerticesEdges.Empty();
	Merged4CellCountVerticesEdges.Empty();
	CellUniqueNumbers.Empty();
    PairedVertices.Empty();
	//below is for poly calculation
	ProcessedVertices.Empty();
	VertexIndexMap.Empty();
	SameCellVertices.Empty();
	CellWithVerticesArrays.Empty();
	CellWithEdgesArrays.Empty();
	Cells.Empty();
	//Make Triangles
	WholeVertices.Empty();
	Triangles.Empty();
	WholeVerticesIndex.Empty();
	//static TMap<FVertexData, int32> GlobalVertexIndexMap;
	UV0.Empty();
	UV1.Empty();
	UV2.Empty();
	Normal.Empty();
}


