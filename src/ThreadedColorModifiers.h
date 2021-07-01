#pragma once
#include <thread>
#include <functional>

/// <summary>
/// 
/// A collection of independently threaded functions for modifying provided values.
/// 
/// </summary>

class ThreadedColorModifiers {
public:
    ThreadedColorModifiers() {}

    // Fade values up or down. 
    // Set custom fade limits by using min & max values
    // NULL arguments won't be processed.
    static inline void FadeToColor(f32_t* r, f32_t* g, f32_t* b, f32_t* a, fan::color target, const int update_ms = 8) {
        f32_t update_increment = 1;

        const int MAX = 1;
        const int MIN = 0;

        if (target.r < MIN) return;
        if (target.g < MIN) return;
        if (target.b < MIN) return;
        if (target.a < MIN) return;

        if (target.r > MAX) return;
        if (target.g > MAX) return;
        if (target.b > MAX) return;
        if (target.a > MAX) return;

        target.r = target.r * 255; // convert target RGBs from 0-1 scale to 0-255
        target.g = target.g * 255;
        target.b = target.b * 255;
        target.a = target.a * 255;

        // define the actual modifier functions
        auto red_modifier = [update_increment, target, update_ms](float* r) {
            bool increasing = false;
            if (*(r) < target.r) {
                increasing = true;
            }
            else increasing = false;

            if (!increasing)
            {
                while (*(r)-update_increment > target.r) {
                    *(r) -= update_increment;
                    std::this_thread::sleep_for(std::chrono::milliseconds(update_ms)); // Modifier update rate
                }
                *(r) = target.r; // completion
            }
            else
            {
                while (*(r)+update_increment < target.r)
                {
                    *(r) += update_increment;
                    std::this_thread::sleep_for(std::chrono::milliseconds(update_ms)); // Modifier update rate
                }
                *(r) = target.r; // completion
            }
        };

        auto green_modifier = [update_increment, target, update_ms](float* g) {
            bool increasing = false;
            if (*(g) < target.g) {
                increasing = true;
            }
            else increasing = false;

            if (!increasing)
            {
                while (*(g)-update_increment > target.g)
                {
                    *(g) -= update_increment;
                    std::this_thread::sleep_for(std::chrono::milliseconds(update_ms)); // Modifier update rate
                }
                *(g) = target.g; // completion
            }
            else {
                while (*(g)+update_increment < target.g)
                {
                    *(g) += update_increment;
                    std::this_thread::sleep_for(std::chrono::milliseconds(update_ms)); // Modifier update rate
                }
                *(g) = target.g; // completion
            }
        };

        auto blue_modifier = [update_increment, target, update_ms](float* b) {
            bool increasing = false;
            if (*(b) < target.b) {
                increasing = true;
            }
            else increasing = false;

            if (!increasing)
            {
                while (*(b)-update_increment > target.b) { // do while result not negative
                    *(b) -= update_increment;
                    std::this_thread::sleep_for(std::chrono::milliseconds(update_ms)); // Modifier update rate
                }
                *(b) = target.b; // completion 
            }
            else {
                while (*(b)+update_increment < target.b)
                {
                    *(b) += update_increment;
                    std::this_thread::sleep_for(std::chrono::milliseconds(update_ms)); // Modifier update rate
                }
                *(b) = target.b; // completion
            }
        };

        auto alpha_modifier = [update_increment, target, update_ms](float* a) {
            bool increasing = false;
            if (*(a) < target.a) {
                increasing = true;
            }
            else increasing = false;

            if (!increasing)
            {
                while (*(a)-update_increment > target.a) {
                    *(a) -= update_increment;
                    std::this_thread::sleep_for(std::chrono::milliseconds(update_ms)); // Modifier update rate
                }
                *(a) = target.a; // completion 
            }
            else {
                while (*(a)+update_increment < target.a)
                {
                    *(a) += update_increment;
                    std::this_thread::sleep_for(std::chrono::milliseconds(update_ms)); // Modifier update rate
                }
                *(a) = target.a; // completion
            }
        };


        // deploy modifiers into detached threads
        if (r != NULL) {
            std::thread red_thread(red_modifier, r);
            red_thread.detach();
        }

        if (g != NULL) {
            std::thread green_thread(green_modifier, g);
            green_thread.detach();
        }

        if (b != NULL) {
            std::thread blue_thread(blue_modifier, b);
            blue_thread.detach();
        }

        if (a != NULL) {
            std::thread alpha_thread(alpha_modifier, a);
            alpha_thread.detach();
        }
    }

    // Fade decreasingly or increasingly until desired min/max rgb values are reached
    static inline void FlipFade(f32_t* r, f32_t* g, f32_t* b, f32_t* a, int min_value = 0, int max_value = 255, bool increasing = false, int update_ms = 2) {
        f32_t update_increment = 1;

        const int MAX = 255;
        const int MIN = 0;

        if (max_value < MIN) max_value = 1;
        if (max_value >= MAX) max_value = 254;

        if (min_value < MIN) min_value = 1;
        if (min_value >= MAX) min_value = 254;

        // define the actual modifier functions
        auto red_modifier = [increasing, update_ms, update_increment, min_value, max_value](float* r) {
            if (!increasing)
            {
                while (*(r)-update_increment > min_value) {
                    *(r) -= update_increment;
                    std::this_thread::sleep_for(std::chrono::milliseconds(update_ms)); // Modifier update rate
                }
                *(r) = min_value; // completion
            }
            else
            {
                while (*(r)+update_increment < max_value)
                {
                    *(r) += update_increment;
                    std::this_thread::sleep_for(std::chrono::milliseconds(update_ms)); // Modifier update rate
                }
                *(r) = max_value; // completion
            }
        };

        auto green_modifier = [increasing, update_ms, update_increment, min_value, max_value](float* g) {
            if (!increasing)
            {
                while (*(g)-update_increment > min_value)
                {
                    *(g) -= update_increment;
                    std::this_thread::sleep_for(std::chrono::milliseconds(update_ms)); // Modifier update rate
                }
                *(g) = min_value; // completion
            }
            else {
                while (*(g)+update_increment < max_value)
                {
                    *(g) += update_increment;
                    std::this_thread::sleep_for(std::chrono::milliseconds(update_ms)); // Modifier update rate
                }
                *(g) = max_value; // completion
            }
        };

        auto blue_modifier = [increasing, update_ms, update_increment, min_value, max_value](float* b) {
            if (!increasing)
            {
                while (*(b)-update_increment > min_value) { // do while result not negative
                    *(b) -= update_increment;
                    std::this_thread::sleep_for(std::chrono::milliseconds(update_ms)); // Modifier update rate
                }
                *(b) = min_value; // completion 
            }
            else {
                while (*(b)+update_increment < max_value)
                {
                    *(b) += update_increment;
                    std::this_thread::sleep_for(std::chrono::milliseconds(update_ms)); // Modifier update rate
                }
                *(b) = max_value; // completion
            }
        };

        auto alpha_modifier = [increasing, update_ms, update_increment, min_value, max_value](float* a) {
            if (!increasing)
            {
                while (*(a)-update_increment > min_value) {
                    *(a) -= update_increment;
                    std::this_thread::sleep_for(std::chrono::milliseconds(update_ms)); // Modifier update rate
                }
                *(a) = min_value; // completion 
            }
            else {
                while (*(a)+update_increment < max_value)
                {
                    *(a) += update_increment;
                    std::this_thread::sleep_for(std::chrono::milliseconds(update_ms)); // Modifier update rate
                }
                *(a) = max_value; // completion
            }
        };


        // launching threads
        if (r != NULL) {
            std::thread red_thread(red_modifier, r);
            red_thread.detach();
        }

        if (g != NULL) {
            std::thread green_thread(green_modifier, g);
            green_thread.detach();
        }

        if (b != NULL) {
            std::thread blue_thread(blue_modifier, b);
            blue_thread.detach();
        }

        if (a != NULL) {
            std::thread alpha_thread(alpha_modifier, a);
            alpha_thread.detach();
        }
    }
};