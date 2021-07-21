// Copyright 2021 Noitom Technology Co., Ltd. All Rights Reserved.
#include "NeuronLiveLinkSourceFactory.h"
#include "NeuronLiveLinkSource.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "SNeuronLiveLinkSourceFactory.h"

#define LOCTEXT_NAMESPACE "FNeuronLiveLinkModule"

UNeuronLiveLinkSourceFactory::UNeuronLiveLinkSourceFactory (const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

}

FText UNeuronLiveLinkSourceFactory::GetSourceDisplayName () const
{
	return LOCTEXT("SourceDisplayName", "Axis Neuron Live");
}

FText UNeuronLiveLinkSourceFactory::GetSourceTooltip () const
{
	return LOCTEXT("SourceTooltip" ,"Creates a connection to a Axis Neuron UDP Stream!");
}

TSharedPtr<SWidget> UNeuronLiveLinkSourceFactory::BuildCreationPanel( FOnLiveLinkSourceCreated InOnLiveLinkSourceCreated ) const
{
	return SNew(SNeuronLiveLinkSourceFactory)
		.OnOkClicked(SNeuronLiveLinkSourceFactory::FOnOkClicked::CreateUObject(this, &UNeuronLiveLinkSourceFactory::OnOkClicked, InOnLiveLinkSourceCreated));
}

TSharedPtr<ILiveLinkSource> UNeuronLiveLinkSourceFactory::CreateSource( const FString& InConnectionString ) const
{
	return CreateSourceAtRuntime( InConnectionString );
}

TSharedPtr<ILiveLinkSource> UNeuronLiveLinkSourceFactory::CreateSourceAtRuntime( const FString& InConnectionString )
{
	TSharedPtr<FNeuronLiveLinkSource> EmptySource = MakeShared<FNeuronLiveLinkSource>( EForceInit::ForceInit );

	int Comma1 = InConnectionString.Find( TEXT( "," ) );
	if (Comma1 == INDEX_NONE)
	{
		return EmptySource;
	}

	int Comma2 = InConnectionString.Find( TEXT( "," ), ESearchCase::CaseSensitive, ESearchDir::FromStart, Comma1 + 1 );
	if (Comma2 == INDEX_NONE)
	{
		return EmptySource;
	}

	int Comma3 = InConnectionString.Find( TEXT( "," ), ESearchCase::CaseSensitive, ESearchDir::FromStart, Comma2 + 1 );
	if (Comma3 == INDEX_NONE)
	{
		//return EmptySource;
		// if RotationOrder field is not exist, set it to default value which is YXZ.
	}

	bool IsUDP;
	FIPv4Endpoint LocalEndpoint, RemoteEndpoint;

	FString LocalS = InConnectionString.Mid( 0, Comma1 ).TrimStartAndEnd( );
	IsUDP = InConnectionString.Mid( Comma1 + 1, Comma2 - Comma1 - 1 ).TrimStartAndEnd( ).ToBool( );
	FString RemoteS;
	FString RotOrderS;
	if (Comma3 != INDEX_NONE)
	{
		RemoteS = InConnectionString.Mid( Comma2 + 1, Comma3 - Comma2 - 1 ).TrimStartAndEnd( );
		RotOrderS = InConnectionString.Mid( Comma3 + 1 ).TrimStartAndEnd( ).ToUpper( );
	}
	else
	{
		RemoteS = InConnectionString.Mid( Comma2 + 1, InConnectionString.Len() - Comma2 - 1 ).TrimStartAndEnd( );
		RotOrderS = TEXT( "YXZ" );
	}

	if (!FIPv4Endpoint::Parse( LocalS, LocalEndpoint ) ||
		!FIPv4Endpoint::Parse( RemoteS, RemoteEndpoint ))
	{
		return EmptySource;
	}

	EmptySource->Init( LocalEndpoint, IsUDP, RemoteEndpoint, RotOrderS);
	return EmptySource;
}

void UNeuronLiveLinkSourceFactory::OnOkClicked( FIPv4Endpoint InLocalEndpoint, bool InIsUDP, FIPv4Endpoint InRemoteEndpoint, const FString& InOrder, FOnLiveLinkSourceCreated InOnLiveLinkSourceCreated ) const
{
	FString ConnectionStr = FString::Format( TEXT( "{0}, {1}, {2}, {3}" ),
		{
			InLocalEndpoint.ToString( ),
			InIsUDP,
			InRemoteEndpoint.ToString( ),
			InOrder
		}
	);
	InOnLiveLinkSourceCreated.ExecuteIfBound( MakeShared<FNeuronLiveLinkSource>( InLocalEndpoint, InIsUDP, InRemoteEndpoint, InOrder ), ConnectionStr );
}

#undef LOCTEXT_NAMESPACE
