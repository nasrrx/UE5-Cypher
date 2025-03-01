// PlayerEncryptionComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerEncryptionComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LINEARPROJECT_API UPlayerEncryptionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerEncryptionComponent();

protected:
	virtual void BeginPlay() override;

public:
	// Make EncryptMessage and DecryptMessage BlueprintCallable
	UFUNCTION(BlueprintCallable, Category = "Encryption")
	FString EncryptMessage(const FString& Plaintext);

	UFUNCTION(BlueprintCallable, Category = "Encryption")
	FString DecryptMessage(const FString& Ciphertext);
	
public:
	int ModInverse(int a, int m);
	int Determinant(const TArray<TArray<int>>& KeyMatrix);
	bool IsKeyMatrixValid(TArray<TArray<int>>& KeyMatrix);
	void GetKeyMatrix(TArray<TArray<int>>& KeyMatrix);
	void CalculateInverseMatrix(TArray<TArray<int>>& KeyMatrix, TArray<TArray<int>>& InverseMatrix);
	void DisplayEncryptedMessage(const FString& Ciphertext);
	void DisplayDecryptedMessage(const FString& DecryptedMessage);
    
	TArray<TArray<int>> KeyMatrix;
	TArray<TArray<int>> InverseMatrix;
};
