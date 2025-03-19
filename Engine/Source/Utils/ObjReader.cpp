#include "pch.h"
#include "ObjReader.h"

#include <sstream>

ObjReader::ObjReader()
{
    FilePath = FString("");
}

ObjReader::ObjReader(const FString& InFilePath)
{
    SetFilePath(InFilePath);
}

ObjReader::~ObjReader()
{
    Clear();
}

TArray<float> ObjReader::GetVertex(int32 Idx)
{
    if (0 <= Idx && Idx < Vertices.Num())
    {
        return Vertices[Idx];
    }
    return {};
}

TArray<float> ObjReader::GetNormal(int32 Idx)
{
    if (0 <= Idx && Idx < Normals.Num())
    {
        return Normals[Idx];
    }
    return {};
}

TArray<float> ObjReader::GetUV(int32 Idx)
{
    if (0 <= Idx && Idx < UVs.Num())
    {
        return UVs[Idx];
    }
    return {};
}

TArray<uint32> ObjReader::GetVertexIndices()
{
    TArray<uint32> Indices(Vertices.Num() * 3);
    int Cnt = 0;
    for (TArray<TArray<uint32>>& Face : Faces)
    {
        for (int i = 0; i < 3; ++i)
        {
            Indices[Cnt] = Face[i][0];
            ++Cnt;
        }
    }
    return Indices;
}

void ObjReader::SetFilePath(const FString& InFilePath)
{
    if (CheckFile(InFilePath))
    {
        FilePath = InFilePath;
        ReadFile();
    }
}

void ObjReader::Clear()
{
    for (TArray<float>& Vertex : Vertices)
    {
        Vertex.Empty();
    }
    Vertices.Empty();

    for (TArray<float>& Normal : Normals)
    {
        Normal.Empty();
    }
    Normals.Empty();

    for (TArray<float>& UV : UVs)
    {
        UV.Empty();
    }
    UVs.Empty();

    for (TArray<TArray<uint32>>& Face : Faces)
    {
        for (int i = 0; i < 3; ++i)
        {
            Face[i].Empty();
        }
        Face.Empty();
    }
    Faces.Empty();
}

bool ObjReader::CheckFile(const FString& InFilePath) const
{
    std::string path(InFilePath.c_char());
    std::ifstream f(path);
    return f.good();
}

void ObjReader::ReadFile()
{
    if (!File.is_open())
    {
        File.open(*FilePath);
    }

    std::string Line; // std::string을 참조해야하므로, FString 대신 std::string 사용
    while (std::getline(File, Line))
    {
        std::stringstream TokenStream(Line);
        TArray<std::string> Tokens;
        std::string Token;
        while (std::getline(TokenStream, Token, ' '))
        {
            Tokens.Add(Token);
        }
        const std::string& Key = Tokens[0];

        if (Key == "v")
        {
            TArray<float> Vertex(3);
            Vertex[0] = std::stof(Tokens[1]); // Location X
            Vertex[1] = std::stof(Tokens[2]); // Location Y
            Vertex[2] = -std::stof(Tokens[3]); // Location Z
            Vertices.Add(Vertex);
        }
        else if (Key == "vn")
        {
            TArray<float> Normal(3);
            Normal[0] = std::stof(Tokens[1]); // Normal X
            Normal[1] = std::stof(Tokens[2]); // Normal Y
            Normal[2] = -std::stof(Tokens[3]); // Normal Z
            Normals.Add(Normal);
        }
        else if (Key == "vt")
        {
            TArray<float> UV(2);
            UV[0] = std::stof(Tokens[1]);       // U
            UV[1] = 1.f - std::stof(Tokens[2]); // V; Obj 파일은 오른손 좌표계 기준이므로, 왼손 좌표계의 UV맵 좌표로 변경
            UVs.Add(UV);
        }
        else if (Key == "f")
        {
            TArray<TArray<uint32>> Face(3);
            for (int i = 0; i < 3; ++i)
            {
                Face[i] = TArray<uint32>(3);
                std::stringstream ss(Tokens[i + 1]);
                std::string Val;
                int Cnt = 0;
                while (std::getline(ss, Val, '/'))
                {
                    Face[i][Cnt] = std::stoi(Val) - 1; // 인덱스는 1부터 시작하므로, 1 감소
                    ++Cnt;
                }
            }
            Faces.Add(Face);
        }
    }
    File.close();    
}
