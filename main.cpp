#include <iostream>
#include "MocapApi.h"
#include <unordered_map>
#include <functional>

#define ReturnIFError(...) if (mcpError!=MocapApi::Error_None) { \
    LastError = mcpError; \
    ExtraErrorMsg = FString::Printf(TEXT("(%s@%d) Error %d"), __FILE__, __LINE__, (int)mcpError); \
    return; \
}

#define ReturnFalseIFError(...) if (mcpError!=MocapApi::Error_None) { \
    LastError = mcpError; \
    ExtraErrorMsg = FString::Printf(TEXT("(%s@%d) Error %d"), __FILE__, __LINE__, (int)mcpError); \
    return false; \
}

int main(){
    // create application
    MocapApi::MCPApplicationHandle_t application;
    MocapApi::IMCPApplication* mcpApplication = nullptr;
    MocapApi::EMCPError mcpError = MocapApi::MCPGetGenericInterface(MocapApi::IMCPApplication_Version,
        reinterpret_cast<void**>(&mcpApplication));
    ReturnIFError();

    mcpError = mcpApplication->CreateApplication(&application);
    ReturnIFError();

    // Appsettings
    MocapApi::IMCPSettings* mcpSettings = nullptr;
    mcpError = MocapApi::MCPGetGenericInterface(MocapApi::IMCPSettings_Version,
        reinterpret_cast<void**>(&mcpSettings));
    ReturnIFError();

    MocapApi::MCPSettingsHandle_t mcpSettingsHandle = 0;
    mcpError = mcpSettings->CreateSettings(&mcpSettingsHandle);
    ReturnIFError();

    // MocapApi::EMCPBvhData BvhDataFmt = (MocapApi::EMCPBvhData)(AppSettings.bvhDataFormat);
    BvhDataFmt = MocapApi::BvhDataType_Binary;
    mcpError = mcpSettings->SetSettingsBvhData(BvhDataFmt, mcpSettingsHandle);
    ReturnIFError();

    // MocapApi::EMCPBvhRotation RotationOrder = (MocapApi::EMCPBvhRotation)(AppSettings.BvhRotation);
    RotationOrder = MocapApi::BvhRotation_YXZ;
    mcpError = mcpSettings->SetSettingsBvhRotation(RotationOrder, mcpSettingsHandle);
    ReturnIFError();

    MocapApi::EMCPBvhTransformation EnableTrans = MocapApi::BvhTransformation_Enable;
    mcpError = mcpSettings->SetSettingsBvhTransformation(EnableTrans, mcpSettingsHandle);
    ReturnIFError();
    //set udp - hardcoded 7001 port
    mcpError = mcpSettings->SetSettingsUDP(7001, mcpSettingsHandle);
    // mcpError = mcpSettings->SetSettingsUDPServer(IPAddress, Port, mcpSettingsHandle);
    ReturnIFError();
    
    //set settings
    mcpError = mcpApplication->SetApplicationSettings(mcpSettingsHandle, application);
    ReturnIFError();
    
    //free settings memory
    mcpError = mcpSettings->DestroySettings(mcpSettingsHandle);
    ReturnIFError();

    AppHandle = application;
    AppHandleInternal = FString::Printf(TEXT("%lld"), application);

    //open the application
    mcpError = mcpApplication->OpenApplication(application);
    ReturnIFError();
    
    //create a command interface
    MocapApi::IMCPCommand* CommandInterface = nullptr;
    MocapApi::EMCPError mcpError = MocapApi::MCPGetGenericInterface(MocapApi::IMCPCommand_Version,
        reinterpret_cast<void**>(&CommandInterface));
    ReturnIFError();
    MocapApi::MCPCommandHandle_t commandHandle;

    //send start record command
    StartRecoredCommand = MocapApi::CommandStartRecored;
    CommandInterface->CreateCommand(StartRecoredCommand, &commandHandle);
    
    //queue the command
    mcpError = mcpApplication->QueuedServerCommand(commandHandle, application);
    ReturnIFError();

    //destroy it
    CommandInterface->DestroyCommand(commandHandle);

    //close the app
    mcpError = mcpApplication->CloseApplication(application);
    ReturnIFError();
}
