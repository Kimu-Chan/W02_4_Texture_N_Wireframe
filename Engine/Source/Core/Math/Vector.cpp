#include "pch.h" 
#include "Vector.h"

const FVector FVector::ZeroVector = FVector(0.f, 0.f, 0.f);
const FVector FVector::OneVector = FVector(1.f, 1.f, 1.f);

const FVector FVector::UpVector = FVector(0.f, 0.f, 1.f);
const FVector FVector::DownVector = FVector(0.f, 0.f, -1.f);
const FVector FVector::ForwardVector = FVector(1.f, 0.f, 0.f);
const FVector FVector::BackwardVector = FVector(-1.f, 0.f, 0.f);
const FVector FVector::RightVector = FVector(0.f, 1.f, 0.f);
const FVector FVector::LeftVector = FVector(0.f, -1.f, 0.f);

const FVector FVector::XAxisVector = FVector(1.f, 0.f, 0.f);
const FVector FVector::YAxisVector = FVector(0.f, 1.f, 0.f);
const FVector FVector::ZAxisVector = FVector(0.f, 0.f, 1.f);
