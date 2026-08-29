#ifndef SPINNER_H
#define SPINNER_H

#include <ftxui/component/component.hpp>      // for Maybe, Checkbox, Make, Radiobox, Vertical, Dropdown
#include <ftxui/component/component_base.hpp> // for Component, ComponentBase
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/mouse.hpp>
#include <vector>
#include <chrono>
#include <string>

using namespace std::chrono_literals;
using Pattern = std::vector<std::string>;
using Duration = std::chrono::milliseconds;

class ImpSpinner : public ftxui::ComponentBase
{
public:
    ImpSpinner():
    progres_(0.0f),
    delay_(0),
    duration_(250),
    animator_(ftxui::animation::Animator(
        &progres_,
        1.0f,
        duration_,
        ftxui::animation::easing::Linear,
        delay_
    ))
    {
        char_set_ = {
            "|",
            "/",
            "-",
            "\\"};
        tot_frames_ = char_set_.size();
       
    };

    template <size_t N>
    ImpSpinner(std::string (&char_set)[N], Duration duration = 250ms, Duration delay = 0ms):
    char_set_(char_set, char_set+N),
    progres_(0.0f),
    tot_frames_(N),
    delay_(delay),
    duration_(duration),
    animator_(ftxui::animation::Animator(
        &progres_,
        1.0f,
        duration_,
        ftxui::animation::easing::Linear,
        delay_
    ))
    {

    };
    ImpSpinner(Pattern char_set, Duration duration = 250ms, Duration delay = 0ms) : char_set_(char_set),
                                                                      progres_(0.0f),
                                                                      tot_frames_(char_set.size()),
                                                                      delay_(delay),
                                                                      duration_(duration),
                                                                      animator_(ftxui::animation::Animator(&progres_,
                                                                                                           1.0f,
                                                                                                           duration_,
                                                                                                           ftxui::animation::easing::Linear,
                                                                                                           delay_)) 
    {
    };


    ftxui::Element OnRender() override
    {
        
        int pattern_indx = static_cast<int>(progres_ * tot_frames_) % tot_frames_;

        return ftxui::text(char_set_[pattern_indx]);
    }

    void OnAnimation(ftxui::animation::Params& p) override 
    {
        animator_.OnAnimation(p);
        if (animator_.to() == progres_) {  // reached 1.0
            progres_ = 0.f;
            animator_ = ftxui::animation::Animator(&progres_, 1.f, duration_, ftxui::animation::easing::Linear);
        }

        ftxui::animation::RequestAnimationFrame();
}

private:
    bool isRendering{false};
    Pattern char_set_;
    float progres_;
    size_t tot_frames_;
    Duration delay_;
    Duration duration_;
    ftxui::animation::Animator animator_;
};

ftxui::Component Spinner()
{
    return ftxui::Make<ImpSpinner>();
}

ftxui::Component Spinner(Pattern char_set, Duration duration = 250ms, Duration delay = 0ms)
{
    return ftxui::Make<ImpSpinner>(char_set,duration,delay);
}

 template <size_t N>
ftxui::Component Spinner(std::string (&char_set)[N], Duration duration = 250ms, Duration delay = 0ms)
{
    return ftxui::Make<ImpSpinner>(char_set,duration,delay);
}

#endif