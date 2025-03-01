#include "PlayerEncryptionComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetSystemLibrary.h"

UPlayerEncryptionComponent::UPlayerEncryptionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    // Initialize matrices
    KeyMatrix.SetNum(2);
    InverseMatrix.SetNum(2);
}

void UPlayerEncryptionComponent::BeginPlay()
{
    Super::BeginPlay();

    // Initialize key matrix and inverse matrix
    GetKeyMatrix(KeyMatrix);
    CalculateInverseMatrix(KeyMatrix, InverseMatrix);
}

int UPlayerEncryptionComponent::ModInverse(int a, int m)
{
    a = a % m;
    for (int x = 1; x < m; x++)
    {
        if ((a * x) % m == 1)
            return x;
    }
    return -1;
}

int UPlayerEncryptionComponent::Determinant(const TArray<TArray<int>>& Matrix)
{
    return (Matrix[0][0] * Matrix[1][1]) - (Matrix[0][1] * Matrix[1][0]);
}

bool UPlayerEncryptionComponent::IsKeyMatrixValid(TArray<TArray<int>>& Matrix)
{
    int det = Determinant(Matrix);
    int detInverse = ModInverse(det, 26);  // Mod 26 for valid alphabet characters
    return det != 0 && detInverse != -1;
}

void UPlayerEncryptionComponent::GetKeyMatrix(TArray<TArray<int>>& Matrix)
{
    // Ensure the outer array has 2 rows
    Matrix.SetNum(2);

    // Ensure each row has 2 columns
    for (int i = 0; i < 2; ++i)
    {
        Matrix[i].SetNum(2);
    }

    FRandomStream RandomStream;
    RandomStream.GenerateNewSeed();

    do
    {
        for (int i = 0; i < 2; ++i)
        {
            for (int j = 0; j < 2; ++j)
            {
                Matrix[i][j] = RandomStream.RandRange(1, 26); // Range adjusted for letter characters
            }
        }
    } while (!IsKeyMatrixValid(Matrix));
}

void UPlayerEncryptionComponent::CalculateInverseMatrix(TArray<TArray<int>>& Matrix, TArray<TArray<int>>& Inverse)
{
    // Ensure the outer array has 2 rows
    Inverse.SetNum(2);

    // Ensure each row has 2 columns
    for (int i = 0; i < 2; ++i)
    {
        Inverse[i].SetNum(2);
    }

    // Calculate the determinant and its modular inverse
    int det = Determinant(Matrix);
    int detInverse = ModInverse(det, 26);  // Mod 26 for valid alphabet characters

    // Check if determinant is valid
    if (det == 0 || detInverse == -1)
    {
        UE_LOG(LogTemp, Error, TEXT("Matrix determinant is invalid. Cannot calculate inverse."));
        return;
    }

    // Calculate the adjugate matrix
    Inverse[0][0] = Matrix[1][1];
    Inverse[0][1] = -Matrix[0][1];
    Inverse[1][0] = -Matrix[1][0];
    Inverse[1][1] = Matrix[0][0];

    // Multiply by determinant inverse and apply modulo 26
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            Inverse[i][j] = (Inverse[i][j] * detInverse + 26) % 26;
            if (Inverse[i][j] < 0)  // Ensure non-negative values
                Inverse[i][j] += 26;
        }
    }
}

FString UPlayerEncryptionComponent::EncryptMessage(const FString& Plaintext)
{
    TArray<int32> PlaintextValues;
    FString Ciphertext = "";

    // Convert plaintext to numerical values
    for (int i = 0; i < Plaintext.Len(); i++)
    {
        TCHAR c = Plaintext[i];
        int value = 0;

        if (FChar::IsAlpha(c))
        {
            c = FChar::ToUpper(c);
            value = c - 'A' + 1;
            PlaintextValues.Add(value);
        }
        else if (c == ' ')
        {
            PlaintextValues.Add(0); // Space is treated as 0
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Invalid character detected. Please enter only letters or spaces."));
            return "NULL";
        }
    }

    // Add padding if necessary to make the message length even
    if (PlaintextValues.Num() % 2 != 0)
    {
        PlaintextValues.Add(0); // Add padding for space as 0
    }

    // Process the plaintext values in pairs
    for (int i = 0; i < PlaintextValues.Num(); i += 2)
    {
        // Initialize MessageVector and CipherMatrix
        TArray<TArray<int>> MessageVector;
        MessageVector.Add(TArray<int>{ PlaintextValues[i] });
        MessageVector.Add(TArray<int>{ PlaintextValues[i + 1] });

        TArray<TArray<int>> CipherMatrix;
        CipherMatrix.Add(TArray<int>{ 0 });
        CipherMatrix.Add(TArray<int>{ 0 });

        // Perform matrix multiplication: CipherMatrix = KeyMatrix * MessageVector
        for (int k = 0; k < 2; k++)
        {
            for (int x = 0; x < 2; x++)
            {
                CipherMatrix[k][0] += KeyMatrix[k][x] * MessageVector[x][0];
            }
            CipherMatrix[k][0] = (CipherMatrix[k][0] % 26 + 26) % 26; // Ensure modulo 26
        }

        // Convert CipherMatrix back to characters
        for (int k = 0; k < 2; k++)
        {
            if (CipherMatrix[k][0] == 0) // Handle space separately
            {
                Ciphertext += ' ';
            }
            else
            {
                Ciphertext += TCHAR(CipherMatrix[k][0] + 'A' - 1); // Convert to character
            }
        }
    }

    DisplayEncryptedMessage(Ciphertext);
    return Ciphertext;
}

FString UPlayerEncryptionComponent::DecryptMessage(const FString& Ciphertext)
{
    FString DecryptedMessage = "";

    // Ensure the ciphertext is in uppercase
    FString UpperCiphertext = Ciphertext.ToUpper();

    // Check if the ciphertext length is even
    if (UpperCiphertext.Len() % 2 != 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Ciphertext length is odd. Padding with an extra space."));
        UpperCiphertext += " "; // Add a space as padding
    }

    // Process the ciphertext in pairs
    for (int i = 0; i < UpperCiphertext.Len(); i += 2)
    {
        // Skip spaces
        if (UpperCiphertext[i] == ' ')
        {
            DecryptedMessage += ' ';
            i++;
            continue;
        }

        // Initialize CipherMatrix and MessageVector
        TArray<TArray<int>> CipherMatrix;
        CipherMatrix.Add(TArray<int>{ UpperCiphertext[i] - 'A' + 1 });
        CipherMatrix.Add(TArray<int>{ UpperCiphertext[i + 1] - 'A' + 1 });

        TArray<TArray<int>> MessageVector;
        MessageVector.Add(TArray<int>{ 0 });
        MessageVector.Add(TArray<int>{ 0 });

        // Perform matrix multiplication: MessageVector = InverseMatrix * CipherMatrix
        for (int k = 0; k < 2; k++)
        {
            for (int x = 0; x < 2; x++)
            {
                MessageVector[k][0] += InverseMatrix[k][x] * CipherMatrix[x][0];
            }
            MessageVector[k][0] = (MessageVector[k][0] % 26 + 26) % 26; // Ensure modulo 26
        }

        // Convert MessageVector back to characters
        for (int k = 0; k < 2; k++)
        {
            if (MessageVector[k][0] == 0)
            {
                DecryptedMessage += ' '; // Handle space separately
            }
            else
            {
                DecryptedMessage += TCHAR(MessageVector[k][0] + 'A' - 1); // Convert to character
            }
        }
    }

    return DecryptedMessage;
}

void UPlayerEncryptionComponent::DisplayEncryptedMessage(const FString& Message)
{
    UE_LOG(LogTemp, Log, TEXT("Encrypted Message: %s"), *Message);
}
