#pragma once
#include <string>

class RenderDevice;
class UIRenderer;

int RunUiCaptureMode(RenderDevice& device, UIRenderer& ui, const std::string& output_dir,
                     const std::string& manifest_path = "");
