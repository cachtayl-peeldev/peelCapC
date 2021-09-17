// Copyright 2021 Noitom Technology Co., Ltd. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "MocapStructs.h"
#include "GameFramework/Actor.h"
#include "MocapAppManager.generated.h"

class MOCAPAPIADAPTER_API MocapAppVisitor
{
public:
    virtual void Visit(UMocapApp* App) = 0;
    virtual ~MocapAppVisitor() {};
};

class MOCAPAPIADAPTER_API FMocapAppManager
{
public:
    static FMocapAppManager& GetInstance();

    bool AddMocapApp(UMocapApp* App);
    bool RemoveMocapApp(UMocapApp* App);
    UMocapApp* GetMocapAppByName(FName AppName);
    void EachRunningApp(MocapAppVisitor& Visitor);

	const FMocapTracker* GetTracker(FName TrackerName);
	const FMocapRigidBody* GetRigidBody(FName RigidName);
    const FMocapAvatar* GetAvatarData(FName AvatarName);
    void OnRecieveMocapData(FName DataName, UMocapApp* theApp);
    bool IsNameUsedByApp(FName DataName, UMocapApp* theApp);

    static FName CombineNameWithAppName(const FString& Name, UMocapApp* App);
    static FString ReduceAppName(FName Name);

private:
    static FMocapAppManager* s_instance;
    static bool NewAppUseShortName;

    mutable FCriticalSection CriticalSection;
    TMap<FName, UMocapApp*> RunningApps;
    TMap<FName, UMocapApp*> NameResolver;
    FMocapAppManager() {};
    FMocapAppManager(const FMocapAppManager& rh) = delete;
    FMocapAppManager& operator=(const FMocapAppManager& rh) = delete;
};


UCLASS()
class MOCAPAPIADAPTER_API AMocapAppManager : public AActor
{
    GENERATED_BODY()

public:
    AMocapAppManager();
    AMocapAppManager(const FObjectInitializer& ObjectInitializer);

    ~AMocapAppManager();

    UFUNCTION(BlueprintCallable, Category = MocapApi)
    AMocapAppManager* GetInstance();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MocapApi|Setting" )
    TArray<FMCAppSettings> MocapAppSettings;

    UPROPERTY(Category = CaliActor, VisibleAnywhere, BlueprintReadOnly)
    class USceneComponent* DefaultSceneRoot;

    // Called when the game starts
    virtual void BeginPlay() override;

    // Called every frame
    virtual void Tick(float DeltaSeconds) override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "MocapApi|Setting")
    bool AutoStartMocapApp;

    UFUNCTION(BlueprintCallable, Category=MocapApi)
    bool StartApplication(const FMCAppSettings& AppSettings);
    
    UFUNCTION(BlueprintCallable, Category=MocapApi)
    void DestroyApplication(const FString& AppName);

    UFUNCTION(BlueprintCallable, Category=MocapApi)
    void DestroyAllApplications();

    UFUNCTION(BlueprintCallable, Category=MocapApi)
    UMocapApp* GetMocapApp(const FString& AppName);

    UFUNCTION(BlueprintCallable, Category = MocapApi)
    void DumpApp(const FString& AppName);

    UFUNCTION(BlueprintCallable, Category = MocapApi)
    void GetAvatarBuildinBoneNameAndParents(TArray<FName>& BoneNames, TArray<int>& Parents);

private:
 
    void DestroyApp(UMocapApp*App);

    //TMap<FString, UMocapApp*> Applications;

    static TArray<FName> AvatarBoneNames;
    static TArray<int> AvatarBoneParents;

    void InitializeDefaults();
};
