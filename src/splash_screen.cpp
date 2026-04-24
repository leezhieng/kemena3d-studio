#include "splash_screen.h"

#include <filesystem>

using namespace kemena;
namespace fs = std::filesystem;

// Splash window dimensions
static constexpr float SW      = 900.0f;
static constexpr float SH      = 520.0f;
static constexpr float PANEL_W = 280.0f;
static constexpr float PANEL_X = SW - PANEL_W;

// Right-panel layout constants
static constexpr float LOGO_W       = 200.0f;
static constexpr float LOGO_H       = 58.0f;
static constexpr float LOGO_Y       = 18.0f;
static constexpr float BTN_PAD      = 16.0f;
static constexpr float BTN_W        = PANEL_W - BTN_PAD * 2.0f;
static constexpr float BTN_H        = 38.0f;
static constexpr float BTN_Y1       = LOGO_Y + LOGO_H + 20.0f;
static constexpr float BTN_Y2       = BTN_Y1 + BTN_H + 10.0f;
static constexpr float RECENT_LBL_Y = BTN_Y2 + BTN_H + 18.0f;
static constexpr float RECENT_ITEM_H = 44.0f;
static constexpr float RECENT_START_Y = RECENT_LBL_Y + 26.0f;
static constexpr size_t MAX_RECENT   = 5;

SplashScreen::SplashScreen(kGuiManager* setGui, kAssetManager* assetManager, Manager* setManager)
    : gui(setGui), manager(setManager)
{
    kTexture2D* spl = assetManager->loadTexture2DFromResource(
        "IMAGE_SPLASH", "splash", kTextureFormat::TEX_FORMAT_RGBA);
    if (spl)
    {
        splashGlId = spl->getTextureID();
        texSplash  = (ImTextureRef)(intptr_t)splashGlId;
    }

    kTexture2D* logo = assetManager->loadTexture2DFromResource(
        "IMAGE_KEMENA_LOGO_INV", "logo", kTextureFormat::TEX_FORMAT_RGBA);
    if (logo)
    {
        logoGlId = logo->getTextureID();
        texLogo  = (ImTextureRef)(intptr_t)logoGlId;
    }
}

bool SplashScreen::draw()
{
    if (!open) return false;

    ImGuiIO& io = ImGui::GetIO();

    // Dim everything behind the splash
    ImGui::GetBackgroundDrawList()->AddRectFilled(
        ImVec2(0.0f, 0.0f), io.DisplaySize,
        IM_COL32(0, 0, 0, 160));

    float cx = floorf((io.DisplaySize.x - SW) * 0.5f);
    float cy = floorf((io.DisplaySize.y - SH) * 0.5f);

    // Close when the user clicks outside the splash window
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        ImVec2 mp = ImGui::GetIO().MousePos;
        if (mp.x < cx || mp.x > cx + SW || mp.y < cy || mp.y > cy + SH)
            open = false;
    }

    ImGui::SetNextWindowPos(ImVec2(cx, cy), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(SW, SH));
    ImGui::SetNextWindowBgAlpha(0.0f);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,    ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,      ImVec2(8.0f, 6.0f));

    constexpr ImGuiWindowFlags kFlags =
        ImGuiWindowFlags_NoTitleBar        |
        ImGuiWindowFlags_NoResize          |
        ImGuiWindowFlags_NoMove            |
        ImGuiWindowFlags_NoScrollbar       |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoSavedSettings   |
        ImGuiWindowFlags_NoNav             |
        ImGuiWindowFlags_NoCollapse;

    if (ImGui::Begin("##SplashScreen", nullptr, kFlags))
    {
        ImVec2       wpos = ImGui::GetWindowPos();
        ImDrawList*  dl   = ImGui::GetWindowDrawList();

        // --- Full-window background (splash.png) ---
        if (splashGlId != 0)
            dl->AddImage(texSplash, wpos, ImVec2(wpos.x + SW, wpos.y + SH));

        // --- Right panel semi-transparent overlay ---
        dl->AddRectFilled(
            ImVec2(wpos.x + PANEL_X, wpos.y),
            ImVec2(wpos.x + PANEL_X + PANEL_W, wpos.y + SH),
            IM_COL32(18, 20, 30, 225));

        // --- Logo at top-right (centered in the right panel) ---
        if (logoGlId != 0)
        {
            float lx = wpos.x + PANEL_X + (PANEL_W - LOGO_W) * 0.5f;
            float ly = wpos.y + LOGO_Y;
            dl->AddImage(texLogo, ImVec2(lx, ly), ImVec2(lx + LOGO_W, ly + LOGO_H));
        }

        // --- "New Project" / "Open Project" buttons ---
        ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.18f, 0.42f, 0.80f, 0.92f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.28f, 0.52f, 0.92f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.12f, 0.32f, 0.68f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_Text,          ImVec4(1.0f,  1.0f,  1.0f,  1.00f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);

        ImGui::SetCursorPos(ImVec2(PANEL_X + BTN_PAD, BTN_Y1));
        if (ImGui::Button("  New Project##splash", ImVec2(BTN_W, BTN_H)))
        {
            if (manager->newProject())
                open = false;
        }

        ImGui::SetCursorPos(ImVec2(PANEL_X + BTN_PAD, BTN_Y2));
        if (ImGui::Button("  Open Project##splash", ImVec2(BTN_W, BTN_H)))
        {
            if (manager->openProject())
                open = false;
        }

        ImGui::PopStyleVar();
        ImGui::PopStyleColor(4);

        // --- "Recent Projects" label ---
        ImGui::SetCursorPos(ImVec2(PANEL_X + BTN_PAD, RECENT_LBL_Y));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.60f, 0.62f, 0.68f, 1.0f));
        ImGui::Text("Recent Projects");
        ImGui::PopStyleColor();

        // Thin separator under label
        float sepY = wpos.y + RECENT_LBL_Y + 19.0f;
        dl->AddLine(
            ImVec2(wpos.x + PANEL_X + 8.0f,            sepY),
            ImVec2(wpos.x + PANEL_X + PANEL_W - 8.0f,  sepY),
            IM_COL32(75, 80, 110, 180), 1.0f);

        // --- Recent project entries ---
        if (manager->recentProjects.empty())
        {
            ImGui::SetCursorPos(ImVec2(PANEL_X + BTN_PAD, RECENT_START_Y));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.42f, 0.44f, 0.50f, 0.80f));
            ImGui::Text("No recent projects");
            ImGui::PopStyleColor();
        }
        else
        {
            for (size_t i = 0; i < manager->recentProjects.size() && i < MAX_RECENT; ++i)
            {
                const kString& projPath = manager->recentProjects[i];
                std::string    projName = fs::path(projPath).filename().string();

                float iy = RECENT_START_Y + static_cast<float>(i) * RECENT_ITEM_H;

                ImGui::SetCursorPos(ImVec2(PANEL_X + BTN_PAD, iy));

                ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.0f, 0.0f, 0.0f, 0.00f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.09f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(1.0f, 1.0f, 1.0f, 0.16f));
                ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);

                std::string bid = "##proj" + std::to_string(i);
                if (ImGui::Button(bid.c_str(), ImVec2(BTN_W, RECENT_ITEM_H - 4.0f)))
                {
                    if (manager->openProjectFromPath(projPath))
                        open = false;
                }

                ImGui::PopStyleColor(3);
                ImGui::PopStyleVar();

                // Project name (larger text via AddText)
                ImVec2 imin = ImGui::GetItemRectMin();
                dl->AddText(
                    ImGui::GetFont(), ImGui::GetFontSize(),
                    ImVec2(imin.x + 10.0f, imin.y + 5.0f),
                    IM_COL32(218, 220, 228, 255),
                    projName.c_str());

                // Truncated path (smaller, dimmer)
                std::string disp = projPath;
                if (disp.size() > 36)
                    disp = "..." + disp.substr(disp.size() - 33);
                dl->AddText(
                    ImGui::GetFont(), 11.5f,
                    ImVec2(imin.x + 10.0f, imin.y + 23.0f),
                    IM_COL32(120, 125, 148, 200),
                    disp.c_str());
            }
        }
    }
    ImGui::End();
    ImGui::PopStyleVar(3);

    return open;
}
