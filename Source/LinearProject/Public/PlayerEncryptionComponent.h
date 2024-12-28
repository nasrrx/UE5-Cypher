#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerEncryptionComponent.generated.h"

/**
 * A component responsible for handling encryption and decryption operations for an actor.
 * This component uses a customizable substitution cipher and supports additional features for logging and error handling.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LINEARPROJECT_API UPlayerEncryptionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // Constructor
    UPlayerEncryptionComponent();

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

public:
    /**
     * Encrypts the given plaintext using the specified key.
     * @param PlainText The input text to encrypt.
     * @param Key The encryption key (shift amount).
     * @return The encrypted text.
     */
    UFUNCTION(BlueprintCallable, Category = "Encryption")
    FString Encrypt(const FString& PlainText, int32 Key);

    /**
     * Decrypts the given ciphertext using the specified key.
     * @param CipherText The input text to decrypt.
     * @param Key The decryption key (shift amount).
     * @return The decrypted text.
     */
    UFUNCTION(BlueprintCallable, Category = "Encryption")
    FString Decrypt(const FString& CipherText, int32 Key);

    /**
     * Validates the input text to ensure it meets encryption requirements.
     * @param Text The input text to validate.
     * @return True if the text is valid, false otherwise.
     */
    UFUNCTION(BlueprintCallable, Category = "Validation")
    bool ValidateInputText(const FString& Text) const;

    /**
     * Logs the encryption or decryption process for debugging purposes.
     * @param ProcessName The name of the process (e.g., "Encryption", "Decryption").
     * @param InputText The original text.
     * @param OutputText The resulting text.
     * @param Key The key used for the process.
     */
    UFUNCTION(BlueprintCallable, Category = "Logging")
    void LogProcess(const FString& ProcessName, const FString& InputText, const FString& OutputText, int32 Key) const;

private:
    /**
     * Utility function to shift characters for encryption/decryption.
     * @param Char The character to shift.
     * @param ShiftAmount The amount to shift.
     * @param bEncrypt True for encryption, false for decryption.
     * @return The shifted character.
     */
    TCHAR ShiftChar(TCHAR Char, int32 ShiftAmount, bool bEncrypt) const;
};
