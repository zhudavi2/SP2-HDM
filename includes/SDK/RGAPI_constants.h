namespace SDK
{
	typedef std::pair<unsigned int, unsigned int> ipair;
	typedef std::pair<unsigned int,  std::string> spair;

    namespace RGAPIIntKey
	{ 
		enum Enum
		{         
			HostPort,
			GameHasPassword,
			ModIsOfficial,
			NumPlayer,
			MaxPlayer
		};
	}

	namespace RGAPIStrKey
	{
		enum Enum
		{
			ServerIPAddress,
			ServerName,
			ModName,
			ModVersion,
			ScenarioName
		};
	}
}