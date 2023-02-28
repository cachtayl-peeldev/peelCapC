#include <iostream>
#include <chrono>
#include <thread>
#include "../MocapApi/include/MocapApi.h"
namespace MocapApi {
    MCP_INTERFACE EMCPError MCP_CALLTYPE MCPGetGenericInterface(const char* pchInterfaceVersion, void** ppInterface);
}

int main() {
    using namespace std::this_thread; // sleep_for, sleep_until
    using namespace std::chrono; // nanoseconds, system_clock, seconds

    // create application
    MocapApi::MCPApplicationHandle_t application;
    MocapApi::IMCPApplication* mcpApplication = nullptr;
    MocapApi::EMCPError mcpError = MocapApi::MCPGetGenericInterface(MocapApi::IMCPApplication_Version,
        reinterpret_cast<void**>(&mcpApplication));
    //ReturnIFError();

    mcpError = mcpApplication->CreateApplication(&application);
    //ReturnIFError();

    // Appsettings
    MocapApi::IMCPSettings* mcpSettings = nullptr;
    mcpError = MocapApi::MCPGetGenericInterface(MocapApi::IMCPSettings_Version,
        reinterpret_cast<void**>(&mcpSettings));
    //ReturnIFError();

    MocapApi::MCPSettingsHandle_t mcpSettingsHandle = 0;
    mcpError = mcpSettings->CreateSettings(&mcpSettingsHandle);
    // ReturnIFError();

    mcpError = mcpSettings->SetSettingsBvhData(MocapApi::BvhDataType_Binary, mcpSettingsHandle);
    // ReturnIFError();

    mcpError = mcpSettings->SetSettingsBvhRotation(MocapApi::BvhRotation_YXZ, mcpSettingsHandle);
    // ReturnIFError();

    MocapApi::EMCPBvhTransformation EnableTrans = MocapApi::BvhTransformation_Enable;
    mcpError = mcpSettings->SetSettingsBvhTransformation(EnableTrans, mcpSettingsHandle);
    // ReturnIFError();

    //Enable BVH Capture -> TCP Protocol(hardcoded local IP for now)
    const char* IPAddress = "127.0.0.1";
    uint16_t Port = 7001;
    
    mcpError = mcpSettings->SetSettingsTCP(IPAddress, Port, mcpSettingsHandle);
    // ReturnIFError();

    //set settings
    mcpError = mcpApplication->SetApplicationSettings(mcpSettingsHandle, application);
    // ReturnIFError();

    //open the application
    mcpError = mcpApplication->OpenApplication(application);

    // ReturnIFError();
    
    //create a command interface
    MocapApi::IMCPCommand* CommandInterface = nullptr;
    mcpError = MocapApi::MCPGetGenericInterface(MocapApi::IMCPCommand_Version,
        reinterpret_cast<void**>(&CommandInterface));
     //ReturnIFError();
    MocapApi::MCPCommandHandle_t commandHandle;

    //create startrecord command
    mcpError = CommandInterface->CreateCommand(MocapApi::CommandStartRecored, &commandHandle);
    
    //commandHandle must be updated before queue
    sleep_for(seconds(1));
    
    //queue the command
    mcpError = mcpApplication->QueuedServerCommand(commandHandle, application);
    
    //finish queue before free command
    sleep_for(seconds(1));
    
    //free command 
    mcpError = CommandInterface->DestroyCommand(commandHandle);

    //free settings 
    mcpError = mcpSettings->DestroySettings(mcpSettingsHandle);
    
    //close the application entity
    mcpError = mcpApplication->CloseApplication(application);

    //This will close the Axis Studios App
    //mcpError = mcpApplication->DestroyApplication(application);
    // ReturnIFError();
}
