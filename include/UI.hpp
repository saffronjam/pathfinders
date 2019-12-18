#pragma once

#include <sstream>
#include <iomanip>
#include <map>
#include <functional>

#include "InputUtility.hpp"

class UI
{
private:
    class FormBase
    {
    public:
        FormBase(Graphics &gfx, InputUtility &iu, sf::Vector2f const &position = sf::Vector2f(0.0f, 0.0f));
        FormBase(Graphics &gfx, InputUtility &iu, std::vector<sf::Vector2f> positions);

    private:
        FormBase(InputUtility &iu, Graphics &gfx);

    public:
        virtual void Update() = 0;
        virtual void Draw() = 0;

        void SetIsHidden(bool const &isHidden) { m_isHidden = isHidden; }
        void SetIsActive(bool const &isActive) { m_isActive = isActive; }

        bool GetIsHidden() { return m_isHidden; }
        bool GetIsActive() { return m_isActive; }

    protected:
        Graphics &m_gfx;
        InputUtility &m_iu;

        //everything else
        sf::Vector2f m_position;
        //radio buttons
        std::vector<sf::Vector2f> m_positions;

        sf::Color m_fillColor;
        sf::Color m_fillColorFaded;

        sf::Color m_altFillColor;
        sf::Color m_altFillColorFaded;

        sf::Color m_outlineColor;
        sf::Color m_outlineColorFaded;
        float m_outlineThickness;

        sf::Text m_title;

        bool m_isHidden;
        bool m_isActive;
        bool m_colorChangeInhib;
    };

    class Button : public FormBase
    {
    public:
        Button(Graphics &gfx,
               InputUtility &iu,
               std::function<void()> caller,
               std::string const &description,
               sf::Vector2f const &position = sf::Vector2f(0.0f, 0.0f));

        void Update() override;
        void Draw() override;

    private:
        void UpdateButtonBox();

    private:
        std::function<void()> m_caller;

        sf::RectangleShape m_buttonBox;
        sf::Text m_description;

        bool m_isBeingClicked;
    };

    class Checkbox : public FormBase
    {
    public:
        Checkbox(Graphics &gfx,
                 InputUtility &iu,
                 bool &value,
                 bool const &start,
                 sf::Vector2f const &position = sf::Vector2f(0.0f, 0.0f));

        void Update() override;
        void Draw() override;

        bool GetTicked() { return m_value; }

    private:
        void UpdateCheckbox();

    private:
        bool &m_value;

        sf::RectangleShape m_checkbox;

        bool m_clickInhib;
    };

    class RadioButtonSet : public FormBase
    {
    public:
        RadioButtonSet(Graphics &gfx,
                       InputUtility &iu,
                       std::vector<bool *> values,
                       std::vector<bool> start,
                       std::vector<sf::Vector2f> positions);

        void Update() override;
        void Draw() override;

    private:
        void UpdateRadioButtons();

    private:
        std::vector<bool *> m_values;

        std::vector<sf::CircleShape> m_radioButtons;
        float m_radius;

        bool m_clickInhib;
    };

    template <typename T>
    class Slider : public FormBase
    {
    public:
        Slider(Graphics &gfx,
               InputUtility &iu,
               T &value,
               T const &low = (T)0,
               T const &high = (T)10,
               T const &start = (T)50,
               sf::Vector2f const &position = sf::Vector2f(0.0f, 0.0f));

        void Update() override;
        void Draw() override;

    private:
        void UpdateSliderPosition();
        void UpdateValue();
        void UpdateValueText();
        void UpdateTextPosition();

    private:
        T &m_value;
        T m_low;
        T m_high;

        sf::RectangleShape m_bar;
        sf::RectangleShape m_slider;
        sf::Text m_valueText;

        float m_mouseDeltaX;

        bool m_engaged;
        bool m_missed;
    };

public:
    enum UI_Type
    {
        UI_Button,
        UI_CheckBox,
        UI_RadioButtonSet,
        UI_IntSlider,
        UI_FloatSlider
    };

public:
    UI(Graphics &gfx, InputUtility &iu);

    void Update(sf::Time dt);
    void Draw();

    auto GetButton(int const &key) { return m_buttons.find(key); }
    auto GetCheckbox(int const &key) { return m_checkboxes.find(key); }
    auto GetRadioButtonSet(int const &key) { return m_radioButtonSets.find(key); }
    auto GetIntSlider(int const &key) { return m_intSliders.find(key); }
    auto GetFloatSlider(int const &key) { return m_floatSliders.find(key); }

    void CreateStaticButton(int const &key, std::function<void()> caller, std::string const &description, sf::Vector2f const &position = sf::Vector2f(0.0f, 0.0f));
    void CreateStaticCheckbox(int const &key, bool &value, bool const &start = false, sf::Vector2f const &position = sf::Vector2f(0.0f, 0.0f));
    void CreateStaticRadioButtonSet(int const &key, std::vector<bool *> values, std::vector<bool> start, std::vector<sf::Vector2f> positions);
    void CreateStaticIntSlider(int const &key, int &value, int const &low = 1, int const &high = 100, int const &start = 50, sf::Vector2f const &position = sf::Vector2f(0.0f, 0.0f));
    void CreateStaticFloatSlider(int const &key, float &value, float const &low = 1.0f, float const &high = 100.0f, float const &start = 50.0f, sf::Vector2f const &position = sf::Vector2f(0.0f, 0.0f));

private:
    Graphics &m_gfx;
    InputUtility &m_iu;

    std::map<int, Button> m_buttons;
    std::map<int, Checkbox> m_checkboxes;
    std::map<int, RadioButtonSet> m_radioButtonSets;
    std::map<int, Slider<int>> m_intSliders;
    std::map<int, Slider<float>> m_floatSliders;
};
