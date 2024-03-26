#include "UI/PrimaryUI.h"
#include "UI/Window.h"
#include "Assets.h"
#include "Defines.h"
#include "GameConnectionManager.h"

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
{
	m_Assets = new AssetCollection();
	m_LastDrawTime = UpdateTimer::GetCurrentTime();
}

PrimaryUI::~PrimaryUI()
{
	delete m_Assets;
	m_Assets = nullptr;
}

void PrimaryUI::Render(Window& window)
{
	// Calc delta time
	TimeDurationNS deltaTimeNS = UpdateTimer::GetCurrentTime() - m_LastDrawTime;
	m_Assets->m_DeltaTimeS = (float)((double)deltaTimeNS / 1000000000.0);
	m_Assets->m_FramesS += m_Assets->m_DeltaTimeS;
	m_Assets->m_FramesRemainderS = std::fmod(m_Assets->m_FramesRemainderS + m_Assets->m_DeltaTimeS, 1.0);

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

	// Prompt waiting for connection
	std::string loadingText = "";
	if (m_Assets->m_FramesRemainderS >= 0.25)
		loadingText += ".";
	if (m_Assets->m_FramesRemainderS >= 0.5)
		loadingText += ".";
	if (m_Assets->m_FramesRemainderS >= 0.75)
		loadingText += ".";

	if (!GameConnectionManager::Instance().AnyConnectionsActive())
	{
		m_Assets->DrawLeftAlignedText(
			gfx,
			"Waiting for Game to connect " + loadingText,
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
		int connectionCount = GameConnectionManager::Instance().ActiveConnectionCount();
		std::string connectionText = (connectionCount <= 1) ? "connected" : ("connected (" + std::to_string(connectionCount) + ")");

		m_Assets->DrawCenteredText(
			gfx,
			"Ready to go!",
			c_CentreOffset + sf::Vector2f(0, -55),
			16,
			m_Assets->m_LightFontColour
		);

		m_Assets->DrawCenteredText(
			gfx,
			connectionText,
			c_CentreOffset + sf::Vector2f(0, 60),
			16,
			sf::Color::Green
		);
	}


	// Draw poketch overlay last
	sf::Sprite sprite;
	sprite.setOrigin(sf::Vector2f(c_ViewWidth / 2, c_ViewHeight / 2));
	sprite.setTexture(m_Assets->m_PoketchOverlay);

	gfx.draw(sprite);

	gfx.setView(gfx.getDefaultView());

	m_LastDrawTime = UpdateTimer::GetCurrentTime();
}