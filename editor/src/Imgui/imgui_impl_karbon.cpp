
#include "imgui.h"
#include "imgui_impl_karbon.h"
#include <karbon/drive.h>


void
ImGui_ImplKarbon_Shutdown()
{

}

void
ImGui_ImplKarbon_NewFrame()
{
        uint64_t evt = 0;
        kd_events_get(&evt);
        
        if(evt & KD_EVENT_INPUT_MS) {
                kd_mouse_desc ms_desc;
                kd_input_get_mice(&ms_desc);

                auto &io = ImGui::GetIO();
                io.MousePos = ImVec2(
                        (float)ms_desc.ms_state[0].x,
                        (float)ms_desc.ms_state[0].y);
                
                uint8_t *keys = ms_desc.ms_state[0].keys;

                bool left_down = !!(keys[KD_MS_LEFT] & KD_KEY_DOWN);
                io.MouseDown[0] = left_down;
                
                bool right_down = !!(keys[KD_MS_RIGHT] & KD_KEY_DOWN);
                io.MouseDown[1] = right_down;
                
                bool mid_down = !!(keys[KD_MS_MIDDLE] & KD_KEY_DOWN);
                io.MouseDown[2] = mid_down;
        }
}
