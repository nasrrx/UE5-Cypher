#include "PlayerEncryptionComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/Engine.h"

// Constructor
UPlayerEncryptionComponent::UPlayerEncryptionComponent()
{
    PrimaryComponentTick.bCanEverTick = false; // No need for ticking
}

// Called when the game starts
void UPlayerEncryptionComponent::BeginPlay()
{
    Super::BeginPlay();

    // Log component initialization
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("PlayerEncryptionComponent initialized."));
    }
}

// Encrypt the given plaintext
FString UPlayerEncryptionComponent::Encrypt(const FString& PlainText, int32 Key)
{
    if (!ValidateInputText(PlainText))
    {
        return TEXT("Invalid input text for encryption.");
    }

    FString EncryptedText;
    for (TCHAR Char : PlainText)
    {
        EncryptedText += ShiftChar(Char, Key, true);
    }

    LogProcess(TEXT("Encryption"), PlainText, EncryptedText, Key);
    return EncryptedText;
}

// Decrypt the given ciphertext
FString UPlayerEncryptionComponent::Decrypt(const FString& CipherText, int32 Key)
{
    if (!ValidateInputText(CipherText))
    {
        return TEXT("Invalid input text for decryption.");
    }

    FString DecryptedText;
    for (TCHAR Char : CipherText)
    {
        DecryptedText += ShiftChar(Char, Key, false);
    }

    LogProcess(TEXT("Decryption"), CipherText, DecryptedText, Key);
    return DecryptedText;
}

// Validate input text
bool UPlayerEncryptionComponent::ValidateInputText(const FString& Text) const
{
    if (Text.IsEmpty())
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Validation failed: Input text is empty."));
        }
        return false;
    }

    for (TCHAR Char : Text)
    {
        if (!FChar::IsAlpha(Char) && !FChar::IsDigit(Char) && !FChar::IsWhitespace(Char))
        {
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Validation failed: Invalid character detected."));
            }
            return false;
        }
    }

    return true;
}

// Log encryption/decryption process
void UPlayerEncryptionComponent::LogProcess(const FString& ProcessName, const FString& InputText, const FString& OutputText, int32 Key) const
{
    if (GEngine)
    {
        FString LogMessage = FString::Printf(TEXT("%s Process:\nInput: %s\nOutput: %s\nKey: %d"), *ProcessName, *InputText, *OutputText, Key);
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, LogMessage);
    }
}

// Shift character for encryption/decryption
TCHAR UPlayerEncryptionComponent::ShiftChar(TCHAR Char, int32 ShiftAmount, bool bEncrypt) const
{
    if (!FChar::IsAlpha(Char))
    {
        // Non-letter characters remain unchanged
        return Char;
    }

    const TCHAR Base = FChar::IsUpper(Char) ? TEXT('A') : TEXT('a');
    const int32 AlphabetSize = 26;

    int32 ShiftedIndex = (Char - Base + (bEncrypt ? ShiftAmount : -ShiftAmount)) % AlphabetSize;

    // Handle negative indices
    if (ShiftedIndex < 0)
    {
        ShiftedIndex += AlphabetSize;
    }

    return Base + ShiftedIndex;
}
