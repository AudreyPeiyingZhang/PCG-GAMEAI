// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBlueprintFunctionLibrary.h"

#include <filesystem>

#include "GameFramework/HUD.h"

UTexture2D* UMyBlueprintFunctionLibrary::CreateTexture2D(int32 Width, int32 Height)
{
	UTexture2D* Texture2D = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8);
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
	Vector2D.X = RandomVector2DtoVector1D(Vector2D, FVector2D(3.453, 2.983), 5.4, 12.12);
	Vector2D.Y = RandomVector2DtoVector1D(Vector2D, FVector2D(5.932, 7.652), -5.8, 4.56);
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
TArray<FVector> UMyBlueprintFunctionLibrary::WholeVerticesInTexture;
TArray<int32> UMyBlueprintFunctionLibrary::Triangles;
TMap<FVector, int32> UMyBlueprintFunctionLibrary::GlobalVertexIndexMap; 


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


void UMyBlueprintFunctionLibrary::VoronoiCalculation(UTexture2D* Texture2D, float CellCount)
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
					const FVector2D CurrentCellPixelXY = FVector2D(PixelsInEachCellX* CurrentCellXY.X,PixelsInEachCellY* CurrentCellXY.Y);
					const int32 RandomX = FMath::Floor(FMath::Lerp(0, PixelsInEachCellX, RandomVector2DtoVector2D(CurrentCellPixelXY).X));
					const int32 RandomY = FMath::Floor(FMath::Lerp(0, PixelsInEachCellY, RandomVector2DtoVector2D(CurrentCellPixelXY).Y));
						
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
			

			
			
			const float NoiseColor = RandomVector2DtoVector1D(ClosestCell, FVector2D(289.89, 38.02), 4.98, 60.90);
			const FVector Color = RandomVector1DtoVector3D(NoiseColor);
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
				if(j!=i && CurrentVertex.IsContainMoreThan3Element(MergedCornerVerticesEdges[j]))
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
	
	UE_LOG(LogTemp, Warning, TEXT("Total vertices merged: %d"), Merged4CellCountVerticesEdges.Num());
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

			
			
		
			if(SharedCellNumber==2)
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


void UMyBlueprintFunctionLibrary::CheckWindingOrder(TArray<int32>&  VtxIndex)
{
	TArray<FVector> VerticesForAreaCalc;
	for (int Index : VtxIndex)
	{
		VerticesForAreaCalc.Add(FindKeyByValue(GlobalVertexIndexMap, Index));
	}
	float Area = CalculatePolygonArea(VerticesForAreaCalc);
	if (Area > 0)  
	{
		Algo::Reverse(VtxIndex);
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


int32 UMyBlueprintFunctionLibrary::AddVertex(FVector VtxPos)
{
	if(!GlobalVertexIndexMap.Contains(VtxPos))
	{
		GlobalVertexIndexMap.Add(VtxPos, WholeVerticesInTexture.Num());
		WholeVerticesInTexture.Add(VtxPos);
		return WholeVerticesInTexture.Num();
	}
	else
	{
		return GlobalVertexIndexMap[VtxPos];
	}
}

void  UMyBlueprintFunctionLibrary::DivideQuadIntoTriangle(TArray<int32> TwoBaseVerticesIndex, TArray<int32> TwoMiddleVerticesIndex, TArray<int32>& DownLeftTriangle, TArray<int32>& DownRightTriangle)
{


	DownLeftTriangle.Add(TwoBaseVerticesIndex[0]);
	DownLeftTriangle.Add(TwoMiddleVerticesIndex[0]);
	DownLeftTriangle.Add(TwoMiddleVerticesIndex[1]);


	DownRightTriangle.Add(TwoBaseVerticesIndex[0]);
	DownRightTriangle.Add(TwoBaseVerticesIndex[1]);
	DownRightTriangle.Add(TwoMiddleVerticesIndex[1]);

	
}

FVector RoundVector(FVector Vec, float Precision = 0.001f)
{
	return FVector(FMath::RoundToFloat(Vec.X / Precision) * Precision,
				   FMath::RoundToFloat(Vec.Y / Precision) * Precision,
				   FMath::RoundToFloat(Vec.Z / Precision) * Precision);
}

void UMyBlueprintFunctionLibrary::TriangleFanSubdivide(TArray<int32> VtxIndex, TArray<FVector> VtxPos)
{
	TArray<int32> TwoBase;
	TwoBase.Empty();
	TwoBase.SetNum(2);
	
	TArray<int32> TwoMid;
	TwoMid.Empty();
	TwoMid.SetNum(2);
	
	const FVector CenterPos = VtxPos[0];
	const FVector FirstPos = VtxPos[1];
	const FVector SecondPos = VtxPos[2];
	
	const FVector MidBetweenCenterAndFirst = (CenterPos + FirstPos)/2;
	const FVector MidBetweenCenterAndSecond = (SecondPos + CenterPos)/2;

	const int32 MidBetweenCenterAndFirstIndex = AddVertex(MidBetweenCenterAndFirst);
	const int32 MidBetweenCenterAndSecondIndex = AddVertex(MidBetweenCenterAndSecond);
	

	TwoBase.Add(VtxIndex[1]);
	TwoBase.Add(VtxIndex[2]);

	TwoMid.Add(MidBetweenCenterAndFirstIndex);
	TwoMid.Add(MidBetweenCenterAndSecondIndex);

	
	//
	TArray<int32> AllTriangles;
	
	//
	TArray<int32> TopTriangle;
	TopTriangle.Empty();
	TopTriangle.SetNum(3);
	TopTriangle.Add(VtxIndex[0]);
	TopTriangle.Add(MidBetweenCenterAndFirstIndex);
	TopTriangle.Add(MidBetweenCenterAndSecondIndex);
	//CheckWindingOrder(TopTriangle);

	//
	TArray<int32> DownLeftTriangle;
	DownLeftTriangle.Empty();
	DownLeftTriangle.SetNum(3);
	

	TArray<int32> DownRightTriangle;
	DownRightTriangle.Empty();
	DownRightTriangle.SetNum(3);
	//create quad
	
	DivideQuadIntoTriangle(TwoBase, TwoMid, DownLeftTriangle,DownRightTriangle );
	//CheckWindingOrder(DownLeftTriangle);
	//CheckWindingOrder(DownRightTriangle);

	//
	AllTriangles.Append(TopTriangle);
	AllTriangles.Append(DownLeftTriangle);
	AllTriangles.Append(DownRightTriangle);

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




float UMyBlueprintFunctionLibrary::CalculatePolygonArea(const TArray<FVector>& VerticesPos)
{
	float Area = 0.0;
	int N = VerticesPos.Num();
	for (int i = 0; i < N; i++)
	{
		const FVector Current = VerticesPos[i];
		const FVector Next = VerticesPos[(i + 1) % N];
		Area += Current.X * Next.Y - Next.X * Current.Y;
	}
	return Area * 0.5;
}

void UMyBlueprintFunctionLibrary::CreateVoronoiShapePolygon(UProceduralMeshComponent* ProceduralMesh)
{

	for(FCellStruct& CurrentCell : Cells)
	{
		TArray<int32> CellVertexIndices;
		TArray<FVector> CellVertexPos;
		//map index
		for(const FVector& Vertex : CurrentCell.VerticesPosition)
		{
			int32 VertexIndex = AddVertex(Vertex);
			CellVertexIndices.Add(VertexIndex);
			CellVertexPos.Add(Vertex);
			
		}

		//get center point 
		FVector CentroidPos = CurrentCell.CalculateCentroid();
		const int32 CentroidIndex = AddVertex(CentroidPos);
		
		//for each triangle
		for(int32 i =0;i<CellVertexIndices.Num();i++)
		{
			TArray<int32> SingleTriangleIndex;
			SingleTriangleIndex.SetNum(3);
			TArray<FVector> SingleTrianglePos;
			SingleTrianglePos.SetNum(3);
			const int32 CenterIndex = CentroidIndex;
			const int32 FirstVtxIndex = CellVertexIndices[i];
			const int32 NextVtxIndex = CellVertexIndices[(i + 1) % CellVertexIndices.Num()];
			SingleTriangleIndex.Add(CenterIndex);
			SingleTriangleIndex.Add(FirstVtxIndex);
			SingleTriangleIndex.Add(NextVtxIndex);

			SingleTrianglePos.Add(CentroidPos);
			SingleTrianglePos.Add(CellVertexPos[i]);
			SingleTrianglePos.Add(CellVertexPos[(i + 1) % CellVertexIndices.Num()]);
			

			TriangleFanSubdivide(SingleTriangleIndex,SingleTrianglePos);
			
		}
		
		

		
		
	}

	ProceduralMesh->CreateMeshSection(0, WholeVerticesInTexture, Triangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), true);
	
}

