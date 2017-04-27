#include "hvhtab.h"

void HvH::RenderTab()
{
	const char* yTypes[] = {
			"SLOW SPIN", "FAST SPIN", "REVERSE SPIN", "TURBO SPIN", "RANDOM SPIN", "SPIN JITTER", "JITTER", "RANDOM JITTER", "SMALL BACKJITTER", "BACKJITTER", "SIDE", "BACKWARDS", "FORWARDS", "LEFT", "RIGHT", "STATIC", "STATIC JITTER", "STATIC SMALL JITTER", "LBY", "FAKEJITTER", // safe
			"LISP", "LISP SIDE", "LISP JITTER", "ANGEL BACKWARDS", "ANGEL INVERSE", "ANGEL SPIN", "SLOWJITTER", "AUTISM",  "ZEUSJITTER", "JITTER 180", "CUSTOM1", "CUSTOM2" // untrusted
	};

	const char* xTypes[] = {
			"UP", "DOWN", "SWITCH", "DANCE", "FRONT", // safe
			"FAKE UP", "FAKE DOWN", "FAKE SWITCH", "LISP DOWN", "ANGEL DOWN", "ANGEL UP", "DOWNZERO", "UPZERO", "FAKE ZERO", "CUSTOM" // untrusted
	};

	const char* resolverModes[] = {
			"OFF", "FORCE", "DELTA", "STEADY", "TICKMODULO", "POSEPARAM", "ALL"
	};

    const char* customAAModes[] = {
            "NORMAL", "SPIN", "JITTER"
    };

    const char* customAAModes2[] = {
            "NORMAL", "SPIN", "JITTER"
    };

    const char* customPitches[] = {
            "NORMAL", "JITTER"
    };

	ImGui::Columns(2, NULL, true);
	{
		ImGui::BeginChild("HVH1", ImVec2(0, 0), true);
		{
			ImGui::Text("AntiAim");
			ImGui::BeginChild("##ANTIAIM", ImVec2(0, 0), true);
			{
				ImGui::Checkbox("Yaw", &Settings::AntiAim::Yaw::enabled);
				SetTooltip("Enables Yaw AntiAim");
				ImGui::Separator();
				ImGui::Columns(2, NULL, true);
				{
					ImGui::ItemSize(ImVec2(0.0f, 0.0f), 0.0f);
					ImGui::Text("Yaw Fake");
					ImGui::ItemSize(ImVec2(0.0f, 0.0f), 0.0f);
					ImGui::Text("Yaw Actual");
					ImGui::Checkbox("Anti Resolver", &Settings::AntiAim::Yaw::antiResolver);
					SetTooltip("Simple resolver prevention");
				}
				ImGui::NextColumn();
				{
					ImGui::PushItemWidth(-1);
					if (ImGui::Combo("##YFAKETYPE", (int*)& Settings::AntiAim::Yaw::typeFake, yTypes, IM_ARRAYSIZE(yTypes)))
					{
						if (!ValveDSCheck::forceUT && ((*csGameRules) && (*csGameRules)->IsValveDS()) && Settings::AntiAim::Yaw::typeFake >= AntiAimType_Y::LISP)
						{
							Settings::AntiAim::Yaw::typeFake = AntiAimType_Y::SPIN_SLOW;
							ImGui::OpenPopup("Error###UNTRUSTED_AA");
						}
					}

					if (ImGui::Combo("##YACTUALTYPE", (int*)& Settings::AntiAim::Yaw::type, yTypes, IM_ARRAYSIZE(yTypes)))
					{
						if (!ValveDSCheck::forceUT && ((*csGameRules) && (*csGameRules)->IsValveDS()) && Settings::AntiAim::Yaw::type >= AntiAimType_Y::LISP)
						{
							Settings::AntiAim::Yaw::type = AntiAimType_Y::SPIN_SLOW;
							ImGui::OpenPopup("Error###UNTRUSTED_AA");
						}
					}
					ImGui::PopItemWidth();
				}
				ImGui::Columns(1);
				ImGui::Separator();
				ImGui::Checkbox("Pitch", &Settings::AntiAim::Pitch::enabled);
				SetTooltip("Enables Pitch AntiAim");
				ImGui::Separator();
				ImGui::Columns(2, NULL, true);
				{
					ImGui::ItemSize(ImVec2(0.0f, 0.0f), 0.0f);
					ImGui::Text("Pitch Actual");
				}
				ImGui::NextColumn();
				{
					ImGui::PushItemWidth(-1);
					if (ImGui::Combo("##XTYPE", (int*)& Settings::AntiAim::Pitch::type, xTypes, IM_ARRAYSIZE(xTypes)))
					{
						if (!ValveDSCheck::forceUT && ((*csGameRules) && (*csGameRules)->IsValveDS()) && Settings::AntiAim::Pitch::type >= AntiAimType_X::STATIC_UP_FAKE)
						{
							Settings::AntiAim::Pitch::type = AntiAimType_X::STATIC_UP;
							ImGui::OpenPopup("Error###UNTRUSTED_AA");
						}
					}
					ImGui::PopItemWidth();
				}
				ImGui::Columns(1);
				ImGui::Separator();
				ImGui::Text("Disable Antiaim");
				ImGui::Separator();
				ImGui::Checkbox("Knife", &Settings::AntiAim::AutoDisable::knifeHeld);
				SetTooltip("Stops your antiaim while you have your knife out.");
				ImGui::Checkbox("No Enemy", &Settings::AntiAim::AutoDisable::noEnemy);
				SetTooltip("Stops your antiaim when there are no enemies visible.");
				ImGui::Separator();
				ImGui::Text("Other");
				ImGui::Separator();
				ImGui::Columns(1);
				ImGui::Checkbox("Dynamic", &Settings::AntiAim::Yaw::dynamic);
				SetTooltip("Antiaim at target");
				ImGui::Checkbox("Moonwalk", &Settings::AntiAim::moonwalk);
				SetTooltip("Makes you slide visually");
				ImGui::Separator();
				ImGui::Text("Edging");
				ImGui::Separator();
				ImGui::Columns(2, NULL, true);
				{
					
					ImGui::Checkbox("Enabled", &Settings::AntiAim::HeadEdge::enabled);
					SetTooltip("Aims your head into the closest wall");
				
				}
				ImGui::NextColumn();
				{
					ImGui::PushItemWidth(-1);
						ImGui::SliderFloat("##EDGEDISTANCE", &Settings::AntiAim::HeadEdge::distance, 20, 30, "Distance: %0.f");
					ImGui::PopItemWidth();
				}
				ImGui::Columns(1);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(210, 85));
				if (ImGui::BeginPopupModal("Error###UNTRUSTED_AA"))
				{
					ImGui::Text("You cannot use this antiaim type on a VALVE server.");

					ImGui::Checkbox("This is not a VALVE server", &ValveDSCheck::forceUT);

					if (ImGui::Button("OK"))
						ImGui::CloseCurrentPopup();

					ImGui::EndPopup();
				}
				ImGui::PopStyleVar();

				ImGui::EndChild();
			}
			ImGui::EndChild();
		}
	}
	ImGui::NextColumn();
	{
		ImGui::BeginChild("HVH2", ImVec2(0, 0), true);
		{
			ImGui::Text("Resolver");
			ImGui::Separator();
            ImGui::Combo("##RESOLVERMODE", (int*)& Settings::Resolver::mode, resolverModes, IM_ARRAYSIZE(resolverModes));
			ImGui::SliderFloat("##RESOLVERTICKS", &Settings::Resolver::ticks, 0, 16, "Ticks: %0.f");
			ImGui::SliderFloat("##RESOLVERMODULO", &Settings::Resolver::modulo, 0, 16, "Modulo: %0.f");
			ImGui::Separator();
			ImGui::Text("Movement");
			ImGui::Checkbox("Auto Crouch", &Settings::Aimbot::AutoCrouch::enabled);
			SetTooltip("Auto crouch when an enemy is in sight");

            ImGui::Separator();
            if (Settings::AntiAim::Yaw::type == AntiAimType_Y::CUSTOM1 || Settings::AntiAim::Yaw::typeFake == AntiAimType_Y::CUSTOM1)
            {
                ImGui::Text("Custom1 Yaw");
                ImGui::Combo("##CUSTOMAAMODE", (int*)& Settings::AntiAim::Custom::YawOne::mode, customAAModes, IM_ARRAYSIZE(customAAModes));
                if (Settings::AntiAim::Custom::YawOne::mode == AntiAim_CustomModes::NORMAL)
                {
                    ImGui::SliderFloat("##CUSTOMAAANGLE", &Settings::AntiAim::Custom::YawOne::angle, -180, 180, "Angle: %0.f");
                    ImGui::Checkbox("Static", &Settings::AntiAim::Custom::YawOne::isStatic);
                    SetTooltip("Ignore player's eye angle");
                }
                else if (Settings::AntiAim::Custom::YawOne::mode == AntiAim_CustomModes::JITTER)
                {
                    ImGui::SliderFloat("##JITTERRANGEMIN", &Settings::AntiAim::Custom::YawOne::jitterMin, -180, 180, "Min: %0.f");
                    ImGui::SliderFloat("##JITTERRANGEMAX", &Settings::AntiAim::Custom::YawOne::jitterMax, -180, 180, "Max: %0.f");
                    ImGui::Checkbox("Static", &Settings::AntiAim::Custom::YawOne::isStatic);
                    SetTooltip("Ignore player's eye angle");
                }
                else if (Settings::AntiAim::Custom::YawOne::mode == AntiAim_CustomModes::SPIN)
                {
                    ImGui::Checkbox("Slow Spin", &Settings::AntiAim::Custom::YawOne::slowSpin);
                    SetTooltip("Reduce spin speed");
                    if (Settings::AntiAim::Custom::YawOne::slowSpin) {
                        ImGui::SliderFloat("##SPINSLOW", &Settings::AntiAim::Custom::YawOne::spinFactor, 1, 25, "Slow: %0.f");
                    }
                    else
                    {
                        ImGui::SliderFloat("##SPINSPEED", &Settings::AntiAim::Custom::YawOne::spinFactor, 1, 25, "Speed: %0.f");
                    }
                }
                ImGui::Checkbox("Avoid LBY", &Settings::AntiAim::Custom::YawOne::avoidLBY);
            }
            if (Settings::AntiAim::Yaw::type == AntiAimType_Y::CUSTOM2 || Settings::AntiAim::Yaw::typeFake == AntiAimType_Y::CUSTOM2)
            {
                ImGui::Text("Custom2 Yaw");
                ImGui::Combo("##CUSTOMAAMODE2", (int*)& Settings::AntiAim::Custom::YawTwo::mode, customAAModes2, IM_ARRAYSIZE(customAAModes2));
                if (Settings::AntiAim::Custom::YawTwo::mode == AntiAim_CustomModes::NORMAL)
                {
                    ImGui::SliderFloat("##CUSTOMAAANGLE2", &Settings::AntiAim::Custom::YawTwo::angle, -180, 180, "Angle: %0.f");
                    ImGui::Checkbox("Static", &Settings::AntiAim::Custom::YawTwo::isStatic);
                    SetTooltip("Ignore player's eye angle");
                }
                else if (Settings::AntiAim::Custom::YawTwo::mode == AntiAim_CustomModes::JITTER)
                {
                    ImGui::SliderFloat("##JITTERRANGEMIN2", &Settings::AntiAim::Custom::YawTwo::jitterMin, -180, 180, "Min: %0.f");
                    ImGui::SliderFloat("##JITTERRANGEMAX2", &Settings::AntiAim::Custom::YawTwo::jitterMax, -180, 180, "Max: %0.f");
                    ImGui::Checkbox("Static", &Settings::AntiAim::Custom::YawTwo::isStatic);
                    SetTooltip("Ignore player's eye angle");
                }
                else if (Settings::AntiAim::Custom::YawTwo::mode == AntiAim_CustomModes::SPIN)
                {
                    ImGui::Checkbox("Slow Spin", &Settings::AntiAim::Custom::YawTwo::slowSpin);
                    SetTooltip("Reduce spin speed");
                    if (Settings::AntiAim::Custom::YawOne::slowSpin) {
                        ImGui::SliderFloat("##SPINSLOW2", &Settings::AntiAim::Custom::YawTwo::spinFactor, 1, 25, "Slow: %0.f");
                    }
                    else
                    {
                        ImGui::SliderFloat("##SPINSPEED2", &Settings::AntiAim::Custom::YawTwo::spinFactor, 1, 25, "Speed: %0.f");
                    }
                }
                ImGui::Checkbox("Avoid LBY", &Settings::AntiAim::Custom::YawTwo::avoidLBY);
            }
            if (Settings::AntiAim::Pitch::type == AntiAimType_X::CUSTOM)
            {
                ImGui::Text("Custom Pitch");
                ImGui::Combo("##CUSTOMPITCHES", (int*)& Settings::AntiAim::Custom::Pitch::mode, customPitches, IM_ARRAYSIZE(customPitches));
                if (Settings::AntiAim::Custom::Pitch::mode == AntiAim_CustomPitches::NORMAL)
                {
                    ImGui::SliderFloat("##CUSTOMPITCHANGLE", &Settings::AntiAim::Custom::Pitch::angle, -89, 89, "Angle: %0.f");
                }
                else if (Settings::AntiAim::Custom::Pitch::mode == AntiAim_CustomPitches::JITTER)
                {
                    ImGui::SliderFloat("##CUSTOMPITCHJITTERMIN", &Settings::AntiAim::Custom::Pitch::jitterMin, -89, 89, "Min: %0.f");
                    ImGui::SliderFloat("##CUSTOMPITCHJITTERMAX", &Settings::AntiAim::Custom::Pitch::jitterMax, -89, 89, "Max: %0.f");
                }
            }

			ImGui::EndChild();
		}
	}
}
