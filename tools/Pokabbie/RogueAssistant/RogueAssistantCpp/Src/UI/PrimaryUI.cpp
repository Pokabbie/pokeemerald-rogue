#include "UI/PrimaryUI.h"
#include "UI/Window.h"
#include "Assets.h"
#include "Defines.h"
#include "GameConnection.h"
#include "GameConnectionManager.h"
#include "Behaviours/MultiplayerBehaviour.h"
#include "Behaviours/HomeBoxBehaviour.h"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

// Dimension is based on the poketch frame asset
static int const c_ViewWidth = 256;
static int const c_ViewHeight = 192;
static double const c_ViewAspectW = 4;
static double const c_ViewAspectH = 3;

static sf::Vector2f const c_CentreOffset(-16, 0);

template <typename T>
static inline void LoadBin2CppAsset(T& output, bin2cpp::File const& file)
{
	output.loadFromMemory(file.getBuffer(), file.getSize());
}

struct AssetCollection
{
	double m_DeltaTimeS = 0.0;
	double m_FramesS = 0.0;
	double m_FramesRemainderS = 0.0;
	std::string m_LoadingSpinnerAnimText;
	std::string m_CursorPosAnimText;

	sf::Color m_ClearColour;
	sf::Color m_DarkFontColour;
	sf::Color m_LightFontColour;
	sf::Font m_Font;
	sf::Texture m_PoketchOverlay;

	AssetCollection()
	{
		m_ClearColour = sf::Color(112, 176, 112);
		m_DarkFontColour = sf::Color(16, 40, 24);
		m_LightFontColour = sf::Color(56, 80, 48);

		LoadBin2CppAsset(m_Font, bin2cpp::getPokemonemeraldproTtfFile());
		m_Font.setSmooth(false);

		LoadBin2CppAsset(m_PoketchOverlay, bin2cpp::getPoketch_framePngFile());
		m_PoketchOverlay.setSmooth(false);
	}

	void DrawCenteredText(sf::RenderWindow& gfx, std::string const& msg, sf::Vector2f pos, int fontSize, sf::Color const& colour)
	{
		sf::Text text;
		text.setFont(m_Font);
		text.setFillColor(colour);
		text.setCharacterSize(fontSize);

		text.setString(msg);
		text.setOrigin(sf::Vector2f(text.getLocalBounds().width / 2, 0));
		text.setPosition(pos);
		gfx.draw(text);
	}

	void DrawLeftAlignedText(sf::RenderWindow& gfx, std::string const& msg, sf::Vector2f pos, int fontSize, sf::Color const& colour)
	{
		sf::Text text;
		text.setFont(m_Font);
		text.setFillColor(colour);
		text.setCharacterSize(fontSize);

		text.setString(msg);
		text.setOrigin(sf::Vector2f(0, 0));
		text.setPosition(pos);
		gfx.draw(text);
	}

	void DrawRightAlignedText(sf::RenderWindow& gfx, std::string const& msg, sf::Vector2f pos, int fontSize, sf::Color const& colour)
	{
		sf::Text text;
		text.setFont(m_Font);
		text.setFillColor(colour);
		text.setCharacterSize(fontSize);

		text.setString(msg);
		text.setOrigin(sf::Vector2f(text.getLocalBounds().width, 0));
		text.setPosition(pos);
		gfx.draw(text);
	}
};

PrimaryUI::PrimaryUI()
	: m_CurrentPage(PageUI::Awaiting)
{
	m_Assets = new AssetCollection();
	m_LastDrawTime = UpdateTimer::GetCurrentClock();
}

PrimaryUI::~PrimaryUI()
{
	delete m_Assets;
	m_Assets = nullptr;
}

void PrimaryUI::Render(Window& window)
{
	// Calc delta time
	TimeDurationNS deltaTimeNS = UpdateTimer::GetCurrentClock() - m_LastDrawTime;
	m_Assets->m_DeltaTimeS = (float)((double)deltaTimeNS / 1000000000.0);
	m_Assets->m_FramesS += m_Assets->m_DeltaTimeS;
	m_Assets->m_FramesRemainderS = std::fmod(m_Assets->m_FramesRemainderS + m_Assets->m_DeltaTimeS, 1.0);

	// Loading spinner text
	m_Assets->m_LoadingSpinnerAnimText = "";
	if (m_Assets->m_FramesRemainderS >= 0.25)
		m_Assets->m_LoadingSpinnerAnimText += ".";
	if (m_Assets->m_FramesRemainderS >= 0.5)
		m_Assets->m_LoadingSpinnerAnimText += ".";
	if (m_Assets->m_FramesRemainderS >= 0.75)
		m_Assets->m_LoadingSpinnerAnimText += ".";

	// Flashing cursor pos
	m_Assets->m_CursorPosAnimText = "";
	if (m_Assets->m_FramesRemainderS >= 0.25)
		m_Assets->m_CursorPosAnimText = "|";
	if (m_Assets->m_FramesRemainderS >= 0.5)
		m_Assets->m_CursorPosAnimText = "";
	if (m_Assets->m_FramesRemainderS >= 0.75)
		m_Assets->m_CursorPosAnimText = "|";


	sf::RenderWindow& gfx = *window.GetHandle();

	// Snap window to aspect ratio
	{
		sf::Vector2u currentWindowSize = gfx.getSize();

		// Snap based on how much window is dragged left or right
		sf::Vector2u snappedWindowSize(
			currentWindowSize.x,
			(u32)std::max(1.0, std::round(currentWindowSize.x / c_ViewAspectW) * c_ViewAspectH)
			//(u32)std::max(1.0, std::round(currentWindowSize.y / c_ViewAspectH) * c_ViewAspectW),
			//currentWindowSize.y
		);

		if (currentWindowSize != snappedWindowSize)
		{
			gfx.setSize(snappedWindowSize);
		}
	}

	// Dimension is based on the poketch frame asset
	sf::View view(sf::FloatRect(-c_ViewWidth / 2, -c_ViewHeight / 2, c_ViewWidth, c_ViewHeight));
	gfx.setView(view);


	gfx.clear(m_Assets->m_ClearColour);


	// Draw title
	m_Assets->DrawCenteredText(
		gfx, 
		"Rogue Assistant",
		c_CentreOffset + sf::Vector2f(0, -86),
		32, 
		m_Assets->m_DarkFontColour
	);

	// Print awaiting connection screen
	if (!GameConnectionManager::Instance().AnyConnectionsActive())
	{
		m_Assets->DrawLeftAlignedText(
			gfx,
			"Waiting for Game to connect " + m_Assets->m_LoadingSpinnerAnimText,
			c_CentreOffset + sf::Vector2f(-74, -55),
			16,
			m_Assets->m_LightFontColour
		);

		m_Assets->DrawLeftAlignedText(
			gfx,
			"How to connect to mGBA:\n"
			"1. Make sure Emerald Rogue is running\n\tin mGBA v0.10.0 or greater\n"
			"2. In mGBA select Tools > Scripting...\n"
			"3. In the new Window, Select\n\tFile > Load Script...\n"
			"4. Locate and select\n\tRogueAssistant_mGBA.lua\n",
			c_CentreOffset + sf::Vector2f(-90, -40),
			16,
			m_Assets->m_LightFontColour
		);

		//m_Assets->DrawCenteredText(
		//	gfx,
		//	"disconnected",
		//	c_CentreOffset + sf::Vector2f(0, 60),
		//	16,
		//	sf::Color::Red
		//);
	}
	else
	{
		// Print connected text
		int connectionCount = GameConnectionManager::Instance().ActiveConnectionCount();
		int prevConnIdx = m_CurrentConnectionIdx;

		if (window.ButtonJustReleased(sf::Keyboard::Tab))
		{
			m_CurrentConnectionIdx++;
		}

		m_CurrentConnectionIdx %= connectionCount;
		ActiveGameConnection& game = GameConnectionManager::Instance().GetGameConnectionAt(m_CurrentConnectionIdx);
		bool hasSwappedConnection = prevConnIdx != m_CurrentConnectionIdx;

		std::string connectionText = "Connected to Game";

		if (connectionCount > 1)
		{
			connectionText += " " + std::to_string(m_CurrentConnectionIdx + 1) + " / " + std::to_string(connectionCount) + " [TAB]";
		}

		m_Assets->DrawCenteredText(
			gfx,
			connectionText,
			c_CentreOffset + sf::Vector2f(0, 60),
			16,
			sf::Color::Green
		);

		// Determine current page

		PageUI newPage = PageUI::Awaiting;
		bool initialLoad = false;

		MultiplayerBehaviour* multiplayer = game.m_Game->FindBehaviour<MultiplayerBehaviour>();
		HomeBoxBehaviour* homebox = game.m_Game->FindBehaviour<HomeBoxBehaviour>();

		if (homebox != nullptr)
		{
			newPage = PageUI::HomeBox;
		}
		else if (multiplayer != nullptr)
		{
			newPage = PageUI::Multiplayer;
		}

		if (m_CurrentPage != newPage || hasSwappedConnection)
		{
			initialLoad = true;
			window.ClearInputText();
		}
		m_CurrentPage = newPage;


		// Render specific page
		switch (m_CurrentPage)
		{
		case PrimaryUI::PageUI::Multiplayer:
			RenderMultiplayerPage(window, multiplayer, initialLoad);
			break;

		case PrimaryUI::PageUI::HomeBox:
			RenderHomeBoxPage(window, homebox, initialLoad);
			break;

		default:
			RenderAwaitingPage(window);
			break;
		}
	}


	// Draw poketch overlay last
	sf::Sprite sprite;
	sprite.setOrigin(sf::Vector2f(c_ViewWidth / 2, c_ViewHeight / 2));
	sprite.setTexture(m_Assets->m_PoketchOverlay);
	gfx.draw(sprite);

	// End draw
	gfx.setView(gfx.getDefaultView());

	m_LastDrawTime = UpdateTimer::GetCurrentClock();
}

void PrimaryUI::RenderAwaitingPage(Window& window)
{
	sf::RenderWindow& gfx = *window.GetHandle();

	// Print state
	m_Assets->DrawCenteredText(
		gfx,
		"Ready to go!",
		c_CentreOffset + sf::Vector2f(0, -55),
		16,
		m_Assets->m_LightFontColour
	);

	//m_Assets->DrawLeftAlignedText(
	//	gfx,
	//	"When Emerald Rogue needs input\nfrom Rogue Assistant, this screen\nwill update",
	//	c_CentreOffset + sf::Vector2f(-90, -30),
	//	16,
	//	m_Assets->m_LightFontColour
	//);
}

void PrimaryUI::RenderMultiplayerPage(Window& window, MultiplayerBehaviour* multiplayer, bool initialLoad)
{
	sf::RenderWindow& gfx = *window.GetHandle();

	// Titlt
	m_Assets->DrawCenteredText(
		gfx,
		"=== Multiplayer ===",
		c_CentreOffset + sf::Vector2f(0, -55),
		16,
		m_Assets->m_LightFontColour
	);

	if (initialLoad)
	{
		if (multiplayer->IsRequestingHostConnection())
		{
			window.SetInputText(std::to_string(MultiplayerBehaviour::c_DefaultPort));
		}
		else
		{

		}
	}

	if (multiplayer->IsAwaitingAddress())
	{
		window.SetInputText(multiplayer->SanitiseConnectionAddress(window.GetInputText()));

		if (multiplayer->IsRequestingHostConnection())
		{
			m_Assets->DrawLeftAlignedText(
				gfx,
				"What Port would you like to host on?\n" + window.GetInputText() + m_Assets->m_CursorPosAnimText + "\n\nPress [ENTER] to continue",
				c_CentreOffset + sf::Vector2f(-90, -30),
				16,
				m_Assets->m_LightFontColour
			);
		}
		else
		{
			m_Assets->DrawLeftAlignedText(
				gfx,
				"What is the address you would like\nto join?\n" + window.GetInputText() + m_Assets->m_CursorPosAnimText + "\n\nPress [ENTER] to continue",
				c_CentreOffset + sf::Vector2f(-90, -30),
				16,
				m_Assets->m_LightFontColour
			);
		}

		if (window.ButtonJustReleased(sf::Keyboard::Return))
		{
			multiplayer->ProvideConnectionAddress(window.GetInputText());
			window.ClearInputText();
		}
	}
	else
	{
		if (multiplayer->IsRequestingHostConnection())
		{
			m_Assets->DrawLeftAlignedText(
				gfx,
				"Hosting on Port:" + std::to_string(multiplayer->GetPort()),
				c_CentreOffset + sf::Vector2f(-90, -40),
				16,
				m_Assets->m_LightFontColour
			);
		}
		else
		{
			if (multiplayer->IsConnected())
			{
				m_Assets->DrawLeftAlignedText(
					gfx,
					"Connected to Host",
					c_CentreOffset + sf::Vector2f(-90, -40),
					16,
					m_Assets->m_LightFontColour
				);
			}
		}

		if (!multiplayer->IsConnected())
		{
			m_Assets->DrawLeftAlignedText(
				gfx,
				"Connection being established " + m_Assets->m_LoadingSpinnerAnimText,
				c_CentreOffset + sf::Vector2f(-90, -30),
				16,
				m_Assets->m_LightFontColour
			);
		}
	}
}

void PrimaryUI::RenderHomeBoxPage(Window& window, HomeBoxBehaviour* homebox, bool initialLoad)
{
	sf::RenderWindow& gfx = *window.GetHandle();

	// Print state
	m_Assets->DrawCenteredText(
		gfx,
		"Transferring Pokémon Boxes",
		c_CentreOffset + sf::Vector2f(0, -55),
		16,
		m_Assets->m_LightFontColour
	);

	if (homebox->IsLoading())
	{
		m_Assets->DrawCenteredText(
			gfx,
			"Loading" + m_Assets->m_LoadingSpinnerAnimText,
			c_CentreOffset + sf::Vector2f(0, -40),
			16,
			m_Assets->m_LightFontColour
		);
	}
	else if (homebox->IsSaving())
	{
		m_Assets->DrawCenteredText(
			gfx,
			"Saving" + m_Assets->m_LoadingSpinnerAnimText,
			c_CentreOffset + sf::Vector2f(0, -40),
			16,
			m_Assets->m_DarkFontColour
		);
	}
}