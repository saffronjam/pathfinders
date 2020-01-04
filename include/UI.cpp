#include "UI.hpp"

UI::FormBase::FormBase(Graphics &gfx, InputUtility &iu, sf::Vector2f const &position)
    : FormBase(iu, gfx)
{
    m_position = position;
}

UI::FormBase::FormBase(Graphics &gfx, InputUtility &iu, std::vector<sf::Vector2f> positions)
    : FormBase(iu, gfx)
{
    m_positions = positions;
}

UI::FormBase::FormBase(InputUtility &iu, Graphics &gfx)
    : m_gfx(gfx),
      m_iu(iu),
      m_fillColor(sf::Color(240, 240, 240, 255)),
      m_fillColorFaded(sf::Color(240, 240, 240, 100)),
      m_altFillColor(sf::Color(160, 160, 160, 255)),
      m_altFillColorFaded(sf::Color(160, 160, 160, 100)),
      m_outlineColor(sf::Color(100, 100, 100, 255)),
      m_outlineColorFaded(sf::Color(100, 100, 100, 100)),
      m_outlineThickness(2.0f),
      m_isHidden(false),
      m_isActive(true),
      m_colorChangeInhib(true)
{
}

UI::Button::Button(Graphics &gfx, InputUtility &iu, std::function<void()> caller, std::string const &description, sf::Vector2f const &position)
    : FormBase(gfx, iu, position),
      m_caller(caller),
      m_buttonBox(sf::Vector2f(40.0f, 15.0f)),
      m_description(description, *m_gfx.allFonts[Graphics::FontMapping::FontArial], 7),
      m_isBeingClicked(false)
{
    m_buttonBox.setFillColor(m_fillColor);
    m_buttonBox.setOutlineThickness(m_outlineThickness);
    m_buttonBox.setOutlineColor(m_outlineColor);
    m_buttonBox.setPosition(m_position);

    sf::Vector2f descPos = m_position;
    descPos.x += m_buttonBox.getSize().x / 2.0f - m_description.getLocalBounds().width / 1.5f;
    descPos.x += m_buttonBox.getSize().y / 2.0f - m_description.getLocalBounds().height / 1.5f;
    m_description.setPosition(descPos);
    m_description.setFillColor(sf::Color::Black);
}

void UI::Button::Update()
{
    if (m_isActive)
    {
        UpdateButtonBox();
    }

    if (!m_isActive && m_colorChangeInhib)
    {
        m_colorChangeInhib = false;
        m_buttonBox.setFillColor(m_fillColorFaded);
    }
    else if (m_isActive && !m_colorChangeInhib)
    {
        m_colorChangeInhib = true;
        m_buttonBox.setFillColor(m_fillColor);
    }
}

void UI::Button::Draw()
{
    if (!m_isHidden)
    {
        m_gfx.Render(m_buttonBox);
        m_gfx.Render(m_description);
    }
}

void UI::Button::UpdateButtonBox()
{
    sf::Vector2f outline(m_outlineThickness, m_outlineThickness);
    sf::FloatRect hitbox = sf::FloatRect(m_buttonBox.getPosition(), m_buttonBox.getSize() + outline * 2.0f);
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) &&
        hitbox.contains((sf::Vector2f)sf::Mouse::getPosition(m_gfx.GetRenderWindow())))
    {
        m_isBeingClicked = true;
    }
    else if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) &&
             m_isBeingClicked &&
             hitbox.contains((sf::Vector2f)sf::Mouse::getPosition(m_gfx.GetRenderWindow())))
    {
        m_isBeingClicked = false;
        m_caller();
    }
}

UI::Checkbox::Checkbox(Graphics &gfx, InputUtility &iu, bool &value, bool const &start, sf::Vector2f const &position, std::function<void()> invokeOnTrue, std::function<void()> invokeOnFalse)
    : FormBase(gfx, iu, position),
      m_value(value),
      m_invokeOnTrue(invokeOnTrue),
      m_invokeOnFalse(invokeOnFalse),
      m_checkbox(sf::Vector2f(10.0f, 10.0f)),
      m_clickInhib(true)
{
    m_value = start;
    m_checkbox.setFillColor(m_fillColor);
    m_checkbox.setOutlineThickness(m_outlineThickness);
    m_checkbox.setOutlineColor(m_outlineColor);
    m_checkbox.setPosition(m_position);
}

void UI::Checkbox::Update()
{
    if (m_isActive)
    {
        UpdateCheckbox();
    }

    if (!m_isActive && m_colorChangeInhib)
    {
        m_colorChangeInhib = false;
        m_checkbox.setFillColor(m_fillColorFaded);
    }
    else if (m_isActive && !m_colorChangeInhib)
    {
        m_colorChangeInhib = true;
        m_checkbox.setFillColor(m_fillColor);
    }
}

void UI::Checkbox::Draw()
{
    if (!m_isHidden)
    {
        m_gfx.Render(m_checkbox);
        if (m_value)
        {
            m_gfx.RenderLine(m_position, m_position + m_checkbox.getSize(), m_outlineColor);
            m_gfx.RenderLine(m_position.x + m_checkbox.getSize().x, m_position.y, m_position.x, m_position.y + m_checkbox.getSize().y, m_outlineColor);
        }
    }
}

void UI::Checkbox::UpdateCheckbox()
{
    sf::Vector2f outline(m_outlineThickness, m_outlineThickness);
    sf::FloatRect hitbox = sf::FloatRect(m_checkbox.getPosition(), m_checkbox.getSize() + outline * 2.0f);
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && hitbox.contains((sf::Vector2f)sf::Mouse::getPosition(m_gfx.GetRenderWindow())))
    {
        if (m_value && m_clickInhib)
        {
            m_clickInhib = false;
            m_value = false;
            if (m_invokeOnFalse)
                m_invokeOnFalse();
        }
        else if (!m_value && m_clickInhib)
        {
            m_clickInhib = false;
            m_value = true;
            if (m_invokeOnTrue)
                m_invokeOnTrue();
        }
    }
    else if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
    {
        m_clickInhib = true;
    }
}

UI::RadioButtonSet::RadioButtonSet(Graphics &gfx, InputUtility &iu, std::vector<bool *> values, std::vector<bool> start, std::vector<sf::Vector2f> positions, std::function<void()> invokeOnChange)
    : FormBase(gfx, iu, positions),
      m_values(values),
      m_invokeOnChange(invokeOnChange),
      m_radius(4.0f),
      m_clickInhib(true)
{
    size_t smallestSize = std::min(values.size(), start.size());
    smallestSize = std::min(smallestSize, positions.size());
    for (size_t i = 0; i < smallestSize; i++)
    {
        *m_values[i] = start[i];
        m_radioButtons.push_back(sf::CircleShape(m_radius));
        *m_values[i] ? m_radioButtons[i].setFillColor(m_fillColor) : m_radioButtons[i].setFillColor(m_altFillColor);
        m_radioButtons[i].setOutlineColor(m_outlineColor);
        m_radioButtons[i].setOutlineThickness(m_outlineThickness);
        m_radioButtons[i].setPosition(m_positions[i]);
    }
}

void UI::RadioButtonSet::Update()
{
    if (m_isActive)
    {
        UpdateRadioButtons();
    }

    if (!m_isActive && m_colorChangeInhib)
    {
        m_colorChangeInhib = false;
        for (size_t i = 0; i < m_radioButtons.size(); i++)
        {
            *m_values[i] ? m_radioButtons[i].setFillColor(m_altFillColorFaded) : m_radioButtons[i].setFillColor(m_fillColorFaded);
            m_radioButtons[i].setOutlineColor(m_outlineColorFaded);
        }
    }
    else if (m_isActive && !m_colorChangeInhib)
    {
        m_colorChangeInhib = true;
        for (size_t i = 0; i < m_radioButtons.size(); i++)
        {
            *m_values[i] ? m_radioButtons[i].setFillColor(m_altFillColor) : m_radioButtons[i].setFillColor(m_fillColor);
            m_radioButtons[i].setOutlineColor(m_outlineColor);
        }
    }
}

void UI::RadioButtonSet::Draw()
{
    if (!m_isHidden)
    {
        for (int i = 0; i < (int)m_radioButtons.size(); i++)
        {
            m_gfx.Render(m_radioButtons[i]);
        }
    }
}

void UI::RadioButtonSet::UpdateRadioButtons()
{
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && m_clickInhib)
    {
        m_clickInhib = false;

        sf::Vector2f mousePos = (sf::Vector2f)sf::Mouse::getPosition(m_gfx.GetRenderWindow());
        int index = -1;
        for (int i = 0; i < (int)m_radioButtons.size(); i++)
        {
            sf::Vector2f outline(m_outlineThickness, m_outlineThickness);
            if (vf::Distance(mousePos, m_radioButtons[i].getPosition() + sf::Vector2f(m_radius, m_radius) + outline) < m_radius + m_outlineThickness)
            {
                index = i;
                break;
            }
        }
        if (index != -1)
        {
            std::vector<bool> old;
            for (size_t i = 0; i < m_values.size(); i++)
            {
                old.push_back(*m_values[i]);
            }
            for (int i = 0; i < (int)m_radioButtons.size(); i++)
            {
                if (i == index)
                {
                    if (!*m_values[i])
                    {
                        *m_values[i] = true;
                    }
                    m_radioButtons[i].setFillColor(m_fillColor);
                }
                else
                {
                    if (*m_values[i])
                    {
                        *m_values[i] = false;
                    }
                    m_radioButtons[i].setFillColor(m_altFillColor);
                }
            }
            for (size_t i = 0; i < m_values.size(); i++)
            {
                if (old[i] != *m_values[i])
                {
                    if (m_invokeOnChange)
                        m_invokeOnChange();
                    break;
                }
            }
        }
    }
    else if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
    {
        m_clickInhib = true;
    }
}

template <typename T>
UI::Slider<T>::Slider(Graphics &gfx, InputUtility &iu, T &value, T const &low, T const &high, T const &start, sf::Vector2f const &position)
    : FormBase(gfx, iu, position),
      m_value(value),
      m_low(low),
      m_high(high),
      m_bar(sf::Vector2f(140.0f, 20.0f)),
      m_slider(sf::Vector2f(45.0f, 20.0f)),
      m_mouseDeltaX(0.0f),
      m_engaged(false),
      m_missed(false)
{

    //m_bar
    m_bar.setPosition(position);
    m_bar.setFillColor(m_fillColor);
    m_bar.setOutlineColor(m_outlineColor);
    m_bar.setOutlineThickness(m_outlineThickness);

    //m_slider
    sf::Vector2f sliderPos(0.0f, m_bar.getPosition().y);
    sliderPos.x = gf::Map(start, m_low, m_high, m_bar.getPosition().x, m_bar.getPosition().x + m_bar.getSize().x - m_slider.getSize().x);
    sliderPos.x = gf::Constrain(sliderPos.x, m_bar.getPosition().x, m_bar.getPosition().x + m_bar.getSize().x - m_slider.getSize().x);
    m_slider.setPosition(sliderPos);
    UpdateValue();
    m_slider.setFillColor(m_altFillColor);

    //m_valueText
    UpdateValueText();
    m_valueText.setFont(*m_gfx.allFonts[Graphics::FontMapping::FontArial]);
    m_valueText.setCharacterSize(16);
    UpdateTextPosition();
}

template <typename T>
void UI::Slider<T>::Update()
{
    if (m_isActive)
    {
        m_colorChangeInhib = true;
        UpdateSliderPosition();
        UpdateValue();
        UpdateValueText();
        UpdateTextPosition();
    }

    if (!m_isActive && m_colorChangeInhib)
    {
        m_colorChangeInhib = false;
        m_bar.setFillColor(m_fillColorFaded);
        m_bar.setOutlineColor(m_outlineColorFaded);
        m_slider.setFillColor(m_fillColorFaded);
        m_valueText.setFillColor(m_fillColorFaded);
    }
    else if (m_isActive && !m_colorChangeInhib)
    {
        m_colorChangeInhib = true;
        m_bar.setFillColor(m_fillColor);
        m_bar.setOutlineColor(m_outlineColor);
        m_slider.setFillColor(m_fillColor);
        m_valueText.setFillColor(m_fillColor);
    }
}

template <typename T>
void UI::Slider<T>::Draw()
{
    if (!m_isHidden)
    {
        m_gfx.Render(m_bar);
        m_gfx.Render(m_slider);
        m_gfx.Render(m_valueText);
    }
}

template <typename T>
void UI::Slider<T>::UpdateSliderPosition()
{
    sf::Vector2f mousePos = (sf::Vector2f)sf::Mouse::getPosition(m_gfx.GetRenderWindow());
    sf::FloatRect hitbox = sf::FloatRect(m_slider.getPosition(), m_slider.getSize());

    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
    {
        if (hitbox.contains(mousePos))
        {
            m_engaged = true;
        }
        else if (!hitbox.contains(mousePos) && !m_engaged)
        {
            m_missed = true;
        }
        if (m_engaged && !m_missed)
        {
            sf::Vector2f slider_newPos = m_slider.getPosition();
            slider_newPos.x = mousePos.x - m_mouseDeltaX;
            m_slider.setPosition(slider_newPos);

            sf::Vector2f potentialFix = m_slider.getPosition();
            potentialFix.x = gf::Constrain(m_slider.getPosition().x, m_bar.getPosition().x, m_bar.getPosition().x + m_bar.getSize().x - m_slider.getSize().x);
            m_slider.setPosition(potentialFix);
        }
    }
    else
    {
        m_mouseDeltaX = mousePos.x - m_slider.getPosition().x;
        m_engaged = false;
        m_missed = false;
    }
}
template <typename T>
void UI::Slider<T>::UpdateValue()
{
    m_value = gf::Map(m_slider.getPosition().x, m_bar.getPosition().x, m_bar.getPosition().x + m_bar.getSize().x - m_slider.getSize().x, m_low, m_high);
}
template <typename T>
void UI::Slider<T>::UpdateValueText()
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(1) << m_value;
    std::string text = stream.str();
    m_valueText.setString(text);
}
template <typename T>
void UI::Slider<T>::UpdateTextPosition()
{
    sf::Vector2f valueTextPos = m_slider.getPosition();
    valueTextPos.x += (m_slider.getSize().x - m_valueText.getLocalBounds().width) / 2.0f;
    valueTextPos.y += (m_slider.getSize().y - m_valueText.getLocalBounds().height) / 2.0f;
    m_valueText.setPosition(valueTextPos);
}

UI::UI(Graphics &gfx, InputUtility &iu)
    : m_gfx(gfx),
      m_iu(iu)
{
}

void UI::Update(sf::Time dt)
{
    for (auto &intSlider : m_intSliders)
    {
        intSlider.second.Update();
    }
    for (auto &floatSlider : m_floatSliders)
    {
        floatSlider.second.Update();
    }
    for (auto &checkbox : m_checkboxes)
    {
        checkbox.second.Update();
    }
    for (auto &radioButtonSet : m_radioButtonSets)
    {
        radioButtonSet.second.Update();
    }
    for (auto &button : m_buttons)
    {
        button.second.Update();
    }
}

void UI::Draw()
{
    for (auto &intSlider : m_intSliders)
    {
        intSlider.second.Draw();
    }
    for (auto &floatSlider : m_floatSliders)
    {
        floatSlider.second.Draw();
    }
    for (auto &checkbox : m_checkboxes)
    {
        checkbox.second.Draw();
    }
    for (auto &radioButtonSet : m_radioButtonSets)
    {
        radioButtonSet.second.Draw();
    }
    for (auto &button : m_buttons)
    {
        button.second.Draw();
    }
}

void UI::CreateStaticButton(int const &key, std::function<void()> caller, std::string const &description, sf::Vector2f const &position)
{
    m_buttons.emplace(key, Button(m_gfx, m_iu, caller, description, position));
}

void UI::CreateStaticCheckbox(int const &key, bool &value, bool const &start, sf::Vector2f const &position, std::function<void()> invokeOnTrue, std::function<void()> invokeOnFalse)
{
    m_checkboxes.emplace(key, Checkbox(m_gfx, m_iu, value, start, position, invokeOnTrue, invokeOnFalse));
}

void UI::CreateStaticRadioButtonSet(int const &key, std::vector<bool *> values, std::vector<bool> start, std::vector<sf::Vector2f> positions, std::function<void()> invokeOnChange)
{
    m_radioButtonSets.emplace(key, RadioButtonSet(m_gfx, m_iu, values, start, positions, invokeOnChange));
}

void UI::CreateStaticIntSlider(int const &key, int &value, int const &low, int const &high, int const &start, sf::Vector2f const &position)
{
    m_intSliders.emplace(key, Slider<int>(m_gfx, m_iu, value, low, high, start, position));
}

void UI::CreateStaticFloatSlider(int const &key, float &value, float const &low, float const &high, float const &start, sf::Vector2f const &position)
{
    m_floatSliders.emplace(key, Slider<float>(m_gfx, m_iu, value, low, high, start, position));
}