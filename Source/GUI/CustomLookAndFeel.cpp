#include "CustomLookAndFeel.h"

// Define colors
const juce::Colour CustomLookAndFeel::backgroundDark = juce::Colour(0xFF2A2A2A);
const juce::Colour CustomLookAndFeel::backgroundMid = juce::Colour(0xFF3A3A3A);
const juce::Colour CustomLookAndFeel::backgroundLight = juce::Colour(0xFF555555);
const juce::Colour CustomLookAndFeel::accentColor = juce::Colour(0xFFE9DB45);
const juce::Colour CustomLookAndFeel::accentColorBright = juce::Colour(0xFFFFF86B);
const juce::Colour CustomLookAndFeel::textColor = juce::Colour(0xFFDDDDDD);
const juce::Colour CustomLookAndFeel::textColorDim = juce::Colour(0xFF888888);
const juce::Colour CustomLookAndFeel::knobColor = juce::Colour(0xFF707070);
const juce::Colour CustomLookAndFeel::knobTrackColor = juce::Colour(0xFF252525);

CustomLookAndFeel::CustomLookAndFeel() {
    setColour(juce::ResizableWindow::backgroundColourId, backgroundDark);
    setColour(juce::Label::textColourId, textColor);
    setColour(juce::ComboBox::backgroundColourId, backgroundMid);
    setColour(juce::ComboBox::textColourId, textColor);
    setColour(juce::ComboBox::outlineColourId, backgroundLight);
    setColour(juce::ComboBox::arrowColourId, accentColor);
    setColour(juce::PopupMenu::backgroundColourId, backgroundMid);
    setColour(juce::PopupMenu::textColourId, textColor);
    setColour(juce::PopupMenu::highlightedBackgroundColourId, accentColor);
    setColour(juce::TextButton::buttonColourId, backgroundLight);
    setColour(juce::TextButton::textColourOffId, textColor);
    setColour(juce::TextButton::textColourOnId, textColor);
    setColour(juce::GroupComponent::outlineColourId, backgroundLight);
    setColour(juce::GroupComponent::textColourId, textColor);
    setColour(juce::Slider::textBoxTextColourId, textColorDim);
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}

void CustomLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                          float sliderPosProportional, float rotaryStartAngle,
                                          float rotaryEndAngle, juce::Slider& slider) {
    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(6.0f);
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto centreX = bounds.getCentreX();
    auto centreY = bounds.getCentreY();
    auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

    // Outer track circle
    g.setColour(knobTrackColor);
    g.fillEllipse(centreX - radius, centreY - radius, radius * 2.0f, radius * 2.0f);

    // Arc background
    float arcRadius = radius * 0.82f;
    float arcThickness = 3.5f;
    juce::Path arcBackground;
    arcBackground.addCentredArc(centreX, centreY, arcRadius, arcRadius,
                                 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour(backgroundLight.withAlpha(0.6f));
    g.strokePath(arcBackground, juce::PathStrokeType(arcThickness, juce::PathStrokeType::curved,
                                                      juce::PathStrokeType::rounded));

    // Arc value
    juce::Path arcValue;
    arcValue.addCentredArc(centreX, centreY, arcRadius, arcRadius,
                            0.0f, rotaryStartAngle, angle, true);
    g.setColour(accentColor);
    g.strokePath(arcValue, juce::PathStrokeType(arcThickness, juce::PathStrokeType::curved,
                                                 juce::PathStrokeType::rounded));

    // Knob center
    auto knobRadius = radius * 0.58f;
    g.setColour(knobColor);
    g.fillEllipse(centreX - knobRadius, centreY - knobRadius, knobRadius * 2.0f, knobRadius * 2.0f);

    // Subtle knob highlight
    g.setColour(juce::Colours::white.withAlpha(0.06f));
    g.fillEllipse(centreX - knobRadius * 0.8f, centreY - knobRadius * 0.9f,
                  knobRadius * 1.6f, knobRadius * 1.0f);

    // Pointer
    juce::Path pointer;
    auto pointerLength = radius * 0.48f;
    auto pointerThickness = 2.5f;
    pointer.addRoundedRectangle(-pointerThickness * 0.5f, -pointerLength,
                                 pointerThickness, pointerLength * 0.55f, 1.5f);
    pointer.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
    g.setColour(accentColorBright);
    g.fillPath(pointer);
}

void CustomLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                                          float sliderPos, float minSliderPos, float maxSliderPos,
                                          juce::Slider::SliderStyle style, juce::Slider& slider) {
    if (style == juce::Slider::LinearVertical) {
        auto trackWidth = 5.0f;
        auto trackX = x + (width - trackWidth) * 0.5f;

        // Background track
        g.setColour(knobTrackColor);
        g.fillRoundedRectangle(trackX, static_cast<float>(y), trackWidth, static_cast<float>(height), 2.5f);

        // Value track
        g.setColour(accentColor);
        auto valueHeight = height - (sliderPos - y);
        g.fillRoundedRectangle(trackX, sliderPos, trackWidth, valueHeight, 2.5f);

        // Thumb
        auto thumbRadius = 7.0f;
        g.setColour(knobColor);
        g.fillEllipse(x + (width - thumbRadius * 2) * 0.5f, sliderPos - thumbRadius, thumbRadius * 2, thumbRadius * 2);
        g.setColour(accentColorBright);
        auto innerR = thumbRadius * 0.55f;
        g.fillEllipse(x + (width - innerR * 2) * 0.5f, sliderPos - innerR, innerR * 2, innerR * 2);
    } else {
        auto trackHeight = 5.0f;
        auto trackY = y + (height - trackHeight) * 0.5f;

        g.setColour(knobTrackColor);
        g.fillRoundedRectangle(static_cast<float>(x), trackY, static_cast<float>(width), trackHeight, 2.5f);

        g.setColour(accentColor);
        g.fillRoundedRectangle(static_cast<float>(x), trackY, sliderPos - x, trackHeight, 2.5f);

        auto thumbRadius = 7.0f;
        g.setColour(knobColor);
        g.fillEllipse(sliderPos - thumbRadius, y + (height - thumbRadius * 2) * 0.5f, thumbRadius * 2, thumbRadius * 2);
        g.setColour(accentColorBright);
        auto innerR = thumbRadius * 0.55f;
        g.fillEllipse(sliderPos - innerR, y + (height - innerR * 2) * 0.5f, innerR * 2, innerR * 2);
    }
}

void CustomLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                                      int buttonX, int buttonY, int buttonW, int buttonH,
                                      juce::ComboBox& box) {
    auto cornerSize = 4.0f;
    auto bounds = juce::Rectangle<int>(0, 0, width, height).toFloat().reduced(0.5f, 0.5f);

    g.setColour(backgroundMid);
    g.fillRoundedRectangle(bounds, cornerSize);

    g.setColour(isButtonDown ? accentColor : backgroundLight);
    g.drawRoundedRectangle(bounds, cornerSize, 1.0f);

    // Arrow
    auto arrowZone = juce::Rectangle<int>(width - 18, 0, 14, height).toFloat();
    juce::Path path;
    path.addTriangle(arrowZone.getX() + 1.0f, arrowZone.getCentreY() - 2.5f,
                     arrowZone.getRight() - 1.0f, arrowZone.getCentreY() - 2.5f,
                     arrowZone.getCentreX(), arrowZone.getCentreY() + 2.5f);
    g.setColour(accentColor);
    g.fillPath(path);
}

void CustomLookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                                          bool shouldDrawButtonAsHighlighted,
                                          bool shouldDrawButtonAsDown) {
    auto bounds = button.getLocalBounds().toFloat().reduced(1.0f);
    auto isOn = button.getToggleState();

    g.setColour(isOn ? accentColor.withAlpha(0.85f) : backgroundMid);
    g.fillRoundedRectangle(bounds, 3.0f);

    g.setColour(isOn ? accentColorBright : backgroundLight);
    g.drawRoundedRectangle(bounds, 3.0f, 1.0f);

    g.setColour(isOn ? backgroundDark : textColorDim);
    g.setFont(11.0f);
    g.drawText(button.getButtonText(), bounds, juce::Justification::centred);
}

void CustomLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                              const juce::Colour& backgroundColour,
                                              bool shouldDrawButtonAsHighlighted,
                                              bool shouldDrawButtonAsDown) {
    auto bounds = button.getLocalBounds().toFloat().reduced(1.0f);

    auto baseColour = shouldDrawButtonAsDown ? accentColor
                    : shouldDrawButtonAsHighlighted ? backgroundLight.brighter(0.1f)
                    : backgroundLight;

    g.setColour(baseColour);
    g.fillRoundedRectangle(bounds, 3.0f);

    g.setColour(shouldDrawButtonAsHighlighted ? accentColor.withAlpha(0.6f) : backgroundLight.brighter(0.2f));
    g.drawRoundedRectangle(bounds, 3.0f, 1.0f);
}

void CustomLookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label) {
    g.fillAll(label.findColour(juce::Label::backgroundColourId));

    if (!label.isBeingEdited()) {
        auto textColour = label.findColour(juce::Label::textColourId);
        g.setColour(textColour);
        g.setFont(getLabelFont(label));

        auto textArea = label.getBorderSize().subtractedFrom(label.getLocalBounds());
        g.drawFittedText(label.getText(), textArea, label.getJustificationType(),
                         juce::jmax(1, static_cast<int>(textArea.getHeight() / 12.0f)),
                         label.getMinimumHorizontalScale());
    }
}

void CustomLookAndFeel::drawGroupComponentOutline(juce::Graphics& g, int width, int height,
                                                   const juce::String& text,
                                                   const juce::Justification& position,
                                                   juce::GroupComponent& group) {
    auto cornerSize = 6.0f;
    auto bounds = juce::Rectangle<float>(0, 0, static_cast<float>(width), static_cast<float>(height));

    g.setColour(backgroundMid.withAlpha(0.5f));
    g.fillRoundedRectangle(bounds.reduced(1.0f), cornerSize);

    g.setColour(backgroundLight);
    g.drawRoundedRectangle(bounds.reduced(1.0f), cornerSize, 1.0f);

    if (text.isNotEmpty()) {
        g.setColour(accentColor);
        g.setFont(13.0f);
        auto textBounds = juce::Rectangle<float>(10.0f, 2.0f, static_cast<float>(width) - 20.0f, 18.0f);
        g.drawText(text, textBounds, juce::Justification::left);
    }
}

juce::Font CustomLookAndFeel::getLabelFont(juce::Label& label) {
    return juce::Font(11.5f);
}

juce::Font CustomLookAndFeel::getComboBoxFont(juce::ComboBox& box) {
    return juce::Font(12.0f);
}

juce::Font CustomLookAndFeel::getTextButtonFont(juce::TextButton& button, int buttonHeight) {
    return juce::Font(12.0f);
}
