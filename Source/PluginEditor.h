#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
// Colour palette
//==============================================================================
namespace BiquadColors
{
    static constexpr uint32_t BG_DARK      = 0xFF0B0B0F;
    static constexpr uint32_t BG_MID       = 0xFF13131A;
    static constexpr uint32_t PANEL_BG     = 0xFF18181F;
    static constexpr uint32_t PANEL_BORDER = 0xFF26263A;
    static constexpr uint32_t KNOB_BODY_HI = 0xFF2C2C3C;
    static constexpr uint32_t KNOB_BODY_LO = 0xFF0D0D14;
    static constexpr uint32_t KNOB_RIM     = 0xFF383850;
    static constexpr uint32_t ACCENT_1     = 0xFF00C8FF;
    static constexpr uint32_t ACCENT_2     = 0xFFFF6B35;
    static constexpr uint32_t ACCENT_3     = 0xFFB8FF47;
    static constexpr uint32_t TEXT_MAIN    = 0xFFD0D0E4;
    static constexpr uint32_t TEXT_DIM     = 0xFF505062;
    static constexpr uint32_t DIVIDER      = 0xFF22222E;
}

//==============================================================================
// Custom LookAndFeel
//==============================================================================
class BiquadLookAndFeel : public juce::LookAndFeel_V4
{
public:
    BiquadLookAndFeel()
    {
        setColour (juce::Slider::thumbColourId,               juce::Colour (BiquadColors::ACCENT_1));
        setColour (juce::Slider::rotarySliderFillColourId,    juce::Colour (BiquadColors::ACCENT_1));
        setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour (BiquadColors::KNOB_RIM));
        setColour (juce::Label::textColourId,                 juce::Colour (BiquadColors::TEXT_MAIN));
    }

    void drawRotarySlider (juce::Graphics& g,
                           int x, int y, int width, int height,
                           float sliderPosProportional,
                           float rotaryStartAngle,
                           float rotaryEndAngle,
                           juce::Slider& slider) override
    {
        const float radius = juce::jmin (width, height) * 0.5f - 5.f;
        const float cx     = x + width  * 0.5f;
        const float cy     = y + height * 0.5f;
        const float angle  = rotaryStartAngle
                             + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
        const juce::Colour accent = accentForSlider (slider);

        // Drop shadow
        {
            juce::Path sp;
            sp.addEllipse (cx - radius - 4, cy - radius - 4,
                           (radius + 4) * 2, (radius + 4) * 2);
            juce::DropShadow ds (juce::Colour (0xBB000000), 14, {0, 4});
            ds.drawForPath (g, sp);
        }

        // Track ring
        const float arcThick = radius * 0.145f;
        {
            juce::Path track;
            track.addCentredArc (cx, cy, radius, radius, 0,
                                  rotaryStartAngle, rotaryEndAngle, true);
            g.setColour (juce::Colour (BiquadColors::KNOB_RIM).withAlpha (0.3f));
            g.strokePath (track, juce::PathStrokeType (arcThick,
                          juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        }

        // Value arc + glow
        {
            juce::Path arc;
            arc.addCentredArc (cx, cy, radius, radius, 0, rotaryStartAngle, angle, true);
            g.setColour (accent.withAlpha (0.18f));
            g.strokePath (arc, juce::PathStrokeType (arcThick + 8,
                          juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
            g.setColour (accent);
            g.strokePath (arc, juce::PathStrokeType (arcThick,
                          juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        }

        // Knob body
        const float kr = radius - arcThick - 5.f;
        {
            juce::ColourGradient grad (juce::Colour (BiquadColors::KNOB_BODY_HI),
                                       cx - kr * 0.35f, cy - kr * 0.35f,
                                       juce::Colour (BiquadColors::KNOB_BODY_LO),
                                       cx + kr * 0.5f,  cy + kr * 0.5f, true);
            g.setGradientFill (grad);
            g.fillEllipse (cx - kr, cy - kr, kr * 2, kr * 2);

            g.setColour (juce::Colour (0xFF3A3A54));
            g.drawEllipse (cx - kr, cy - kr, kr * 2, kr * 2, 1.2f);

            g.setColour (juce::Colour (0xFF0A0A10));
            g.drawEllipse (cx - kr + 1.6f, cy - kr + 1.6f,
                           (kr - 1.6f) * 2, (kr - 1.6f) * 2, 0.8f);

            juce::Path sheen;
            sheen.addCentredArc (cx, cy, kr - 3.5f, kr - 3.5f, 0,
                                  juce::MathConstants<float>::pi * 1.15f,
                                  juce::MathConstants<float>::pi * 1.72f, true);
            g.setColour (juce::Colour (0x28FFFFFF));
            g.strokePath (sheen, juce::PathStrokeType (1.4f));

            const float p1 = kr * 0.22f, p2 = kr * 0.72f;
            const float lx1 = cx + std::sin (angle) * p1, ly1 = cy - std::cos (angle) * p1;
            const float lx2 = cx + std::sin (angle) * p2, ly2 = cy - std::cos (angle) * p2;
            g.setColour (accent.brighter (0.08f));
            g.drawLine (lx1, ly1, lx2, ly2, 2.1f);
            g.setColour (accent);
            g.fillEllipse (lx2 - 2.8f, ly2 - 2.8f, 5.6f, 5.6f);
        }
    }

    juce::Label* createSliderTextBox (juce::Slider& slider) override
    {
        auto* lbl = new juce::Label();
        lbl->setJustificationType (juce::Justification::centred);
        lbl->setFont (juce::Font (juce::Font::getDefaultMonospacedFontName(), 10.f, juce::Font::plain));
        lbl->setColour (juce::Label::textColourId,
                        accentForSlider (slider).withAlpha (0.80f));
        lbl->setColour (juce::Label::backgroundColourId,
                        juce::Colour (0x00000000));
        lbl->setColour (juce::Label::textWhenEditingColourId,
                        juce::Colour (BiquadColors::TEXT_MAIN));
        lbl->setColour (juce::Label::backgroundWhenEditingColourId,
                        juce::Colour (0xDD0E0E18));
        return lbl;
    }

    void drawLabel (juce::Graphics& g, juce::Label& lbl) override
    {
        g.setColour (lbl.findColour (juce::Label::backgroundColourId));
        g.fillRoundedRectangle (lbl.getLocalBounds().toFloat(), 3.f);
        g.setColour (lbl.findColour (juce::Label::textColourId));
        g.setFont (lbl.getFont());
        g.drawFittedText (lbl.getText(), lbl.getLocalBounds().reduced (2),
                          juce::Justification::centred, 1);
    }

    static juce::Colour accentForSlider (juce::Slider& s)
    {
        auto id = s.getComponentID();
        if (id.containsChar ('1')) return juce::Colour (BiquadColors::ACCENT_1);
        if (id.containsChar ('2')) return juce::Colour (BiquadColors::ACCENT_2);
        return juce::Colour (BiquadColors::ACCENT_3);
    }
};

//==============================================================================
// Single knob + label unit
//==============================================================================
class KnobUnit : public juce::Component
{
public:
    KnobUnit (const juce::String& labelText, juce::Colour accent)
    {
        paramLabel.setText (labelText, juce::dontSendNotification);
        paramLabel.setFont (juce::Font (juce::Font::getDefaultMonospacedFontName(), 9.f, juce::Font::plain));
        paramLabel.setColour (juce::Label::textColourId, accent.withAlpha (0.55f));
        paramLabel.setJustificationType (juce::Justification::centred);
        addAndMakeVisible (paramLabel);

        slider.setSliderStyle (juce::Slider::RotaryVerticalDrag);
        slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 76, 16);
        slider.setLookAndFeel (&lnf);
        addAndMakeVisible (slider);
    }

    ~KnobUnit() override { slider.setLookAndFeel (nullptr); }

    juce::Slider& getSlider() { return slider; }

    void resized() override
    {
        auto b = getLocalBounds();
        paramLabel.setBounds (b.removeFromTop (15));
        slider.setBounds (b);
    }

private:
    BiquadLookAndFeel lnf;
    juce::Label  paramLabel;
    juce::Slider slider;
};

//==============================================================================
// Stage panel — horizontal layout: [label | cutoff knob | Q knob]
//==============================================================================
class StagePanel : public juce::Component
{
public:
    StagePanel (TripleBiquadAudioProcessor& proc, int idx, juce::Colour accent)
        : accentColour (accent),
          cutoffKnob ("CUTOFF", accent),
          qKnob      ("Q", accent)
    {
        const juce::String si = juce::String (idx);

        cutoffKnob.getSlider().setComponentID ("cutoff" + si);
        qKnob     .getSlider().setComponentID ("q"      + si);

        cutoffAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
            (proc.apvts, "cutoff" + si, cutoffKnob.getSlider());
        qAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
            (proc.apvts, "q" + si, qKnob.getSlider());

        addAndMakeVisible (cutoffKnob);
        addAndMakeVisible (qKnob);

        stageLabel.setText ("STAGE\n" + si, juce::dontSendNotification);
        stageLabel.setFont (juce::Font (juce::Font::getDefaultMonospacedFontName(), 10.f, juce::Font::bold));
        stageLabel.setColour (juce::Label::textColourId, accent);
        stageLabel.setJustificationType (juce::Justification::centred);
        addAndMakeVisible (stageLabel);
    }

    void paint (juce::Graphics& g) override
    {
        auto b = getLocalBounds().toFloat().reduced (2.f);
        g.setColour (juce::Colour (BiquadColors::PANEL_BG));
        g.fillRoundedRectangle (b, 10.f);
        g.setColour (accentColour.withAlpha (0.22f));
        g.drawRoundedRectangle (b, 10.f, 1.5f);

        // Left accent bar (vertical)
        juce::Path bar;
        bar.addRoundedRectangle (b.getX(), b.getY() + 16, 3.f, b.getHeight() - 32, 1.5f);
        g.setColour (accentColour.withAlpha (0.80f));
        g.fillPath (bar);

        // Inner top sheen
        juce::ColourGradient sheen (juce::Colour (0x28FFFFFF), b.getX(), b.getY(),
                                     juce::Colour (0x00000000), b.getX(), b.getY() + 20, false);
        g.setGradientFill (sheen);
        g.fillRoundedRectangle (b, 10.f);

        // Divider between the two knobs
        const float divX = b.getX() + b.getWidth() * 0.56f;
        g.setColour (juce::Colour (BiquadColors::DIVIDER));
        g.drawLine (divX, b.getY() + 14, divX, b.getBottom() - 14, 1.f);
    }

    void resized() override
    {
        auto b = getLocalBounds().reduced (8);
        stageLabel.setBounds (b.removeFromLeft (46));
        b.removeFromLeft (4);
        // Give cutoff a bit more (60%) since it has longer values
        const int cutoffW = (int)(b.getWidth() * 0.56f);
        cutoffKnob.setBounds (b.removeFromLeft (cutoffW).reduced (2, 0));
        qKnob     .setBounds (b.reduced (2, 0));
    }

private:
    juce::Colour accentColour;
    KnobUnit cutoffKnob, qKnob;
    juce::Label stageLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> cutoffAttach, qAttach;
};

//==============================================================================
// Frequency Response Display
//==============================================================================
class FreqResponseDisplay : public juce::Component,
                             private juce::AudioProcessorValueTreeState::Listener,
                             private juce::Timer
{
public:
    FreqResponseDisplay (TripleBiquadAudioProcessor& proc) : processor (proc)
    {
        for (auto* id : { "cutoff1","q1","cutoff2","q2","cutoff3","q3" })
            proc.apvts.addParameterListener (id, this);
        startTimerHz (30);
        dirty = true;
    }

    ~FreqResponseDisplay() override
    {
        for (auto* id : { "cutoff1","q1","cutoff2","q2","cutoff3","q3" })
            processor.apvts.removeParameterListener (id, this);
    }

    void parameterChanged (const juce::String&, float) override { dirty = true; }
    void timerCallback() override { if (dirty) { recalculate(); dirty = false; repaint(); } }

    void paint (juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();

        // Background
        g.setColour (juce::Colour (0xFF0A0A10));
        g.fillRoundedRectangle (bounds, 10.f);
        g.setColour (juce::Colour (BiquadColors::PANEL_BORDER));
        g.drawRoundedRectangle (bounds.reduced (0.5f), 10.f, 1.f);

        // Plot margins: left wider for dB labels, bottom for freq labels
        const float ml = 50.f, mr = 22.f, mt = 32.f, mb = 32.f;
        const juce::Rectangle<float> plot (bounds.getX() + ml,
                                            bounds.getY() + mt,
                                            bounds.getWidth() - ml - mr,
                                            bounds.getHeight() - mt - mb);

        // Plot background
        g.setColour (juce::Colour (0xFF0D0D15));
        g.fillRect (plot);

        // === Grid ===
        g.saveState();
        g.reduceClipRegion (plot.toNearestInt());

        // dB lines
        for (float dB : { 12.f, 6.f, 0.f, -6.f, -12.f, -24.f, -36.f, -48.f })
        {
            const float yp = plotY (dB, plot);
            if (dB == 0.f) g.setColour (juce::Colour (0xFF2E2E46));
            else           g.setColour (juce::Colour (0xFF181824));
            g.drawHorizontalLine ((int)yp, plot.getX(), plot.getRight());
        }

        // Frequency vertical lines
        const float vFreqs[] = { 50.f,100.f,200.f,500.f,1000.f,2000.f,5000.f,10000.f,20000.f };
        for (float f : vFreqs)
        {
            g.setColour (juce::Colour (0xFF181824));
            g.drawVerticalLine ((int)plotX(f, plot), plot.getY(), plot.getBottom());
        }

        g.restoreState();

        // === dB axis labels ===
        g.setFont (juce::Font (juce::Font::getDefaultMonospacedFontName(), 8.5f, juce::Font::plain));
        for (float dB : { 12.f, 6.f, 0.f, -6.f, -12.f, -24.f, -36.f, -48.f })
        {
            const float yp = plotY (dB, plot);
            g.setColour (dB == 0.f ? juce::Colour (0xFF555570) : juce::Colour (BiquadColors::TEXT_DIM));
            juce::String txt = (dB > 0 ? "+" : "") + juce::String ((int)dB);
            g.drawText (txt,
                        (int)bounds.getX() + 4, (int)(yp - 6),
                        (int)ml - 10, 12,
                        juce::Justification::centredRight);
        }
        // dB unit title
        g.setColour (juce::Colour (BiquadColors::TEXT_DIM));
        g.setFont (juce::Font (juce::Font::getDefaultMonospacedFontName(), 7.5f, juce::Font::plain));
        g.drawText ("dB", (int)bounds.getX() + 4, (int)plot.getY() - 14,
                    (int)ml - 10, 12, juce::Justification::centredRight);

        // === Frequency axis labels ===
        g.setFont (juce::Font (juce::Font::getDefaultMonospacedFontName(), 8.5f, juce::Font::plain));
        g.setColour (juce::Colour (BiquadColors::TEXT_DIM));
        struct FL { float f; const char* t; };
        const FL fLabels[] = { {100.f,"100"},{500.f,"500"},{1000.f,"1k"},
                                {5000.f,"5k"},{10000.f,"10k"},{20000.f,"20k"} };
        for (auto& fl : fLabels)
        {
            const float xp = plotX (fl.f, plot);
            g.drawText (fl.t, (int)(xp - 16), (int)plot.getBottom() + 6, 32, 12,
                        juce::Justification::centred);
        }
        g.setFont (juce::Font (juce::Font::getDefaultMonospacedFontName(), 7.5f, juce::Font::plain));
        g.drawText ("Hz", (int)plot.getRight() + 4, (int)plot.getBottom() + 6,
                    24, 12, juce::Justification::centredLeft);

        // === Curves ===
        g.saveState();
        g.reduceClipRegion (plot.toNearestInt());

        auto drawStage = [&] (const std::vector<float>& mags, juce::Colour col)
        {
            if (mags.empty()) return;
            juce::Path p;
            bool first = true;
            for (int i = 0; i < (int)mags.size(); ++i)
            {
                const float t    = (float)i / (mags.size() - 1);
                const float freq = FREQ_MIN * std::pow (FREQ_MAX / FREQ_MIN, t);
                const float xp   = plotX (freq, plot);
                const float yp   = plotY (juce::Decibels::gainToDecibels (mags[i], -96.f), plot);
                if (first) { p.startNewSubPath (xp, yp); first = false; }
                else          p.lineTo (xp, yp);
            }
            g.setColour (col.withAlpha (0.20f));
            g.strokePath (p, juce::PathStrokeType (1.2f, juce::PathStrokeType::curved));
        };

        drawStage (stage1Mags, juce::Colour (BiquadColors::ACCENT_1));
        drawStage (stage2Mags, juce::Colour (BiquadColors::ACCENT_2));
        drawStage (stage3Mags, juce::Colour (BiquadColors::ACCENT_3));

        // Filled area
        if (combinedPath.getLength() > 0)
        {
            juce::Path filled = combinedPath;
            filled.lineTo (plot.getRight(), plot.getBottom());
            filled.lineTo (plot.getX(),     plot.getBottom());
            filled.closeSubPath();
            juce::ColourGradient fill (juce::Colour (0x2200C8FF), 0, plot.getY(),
                                        juce::Colour (0x0000C8FF), 0, plot.getBottom(), false);
            g.setGradientFill (fill);
            g.fillPath (filled);
        }

        // Combined line
        if (combinedPath.getLength() > 0)
        {
            g.setColour (juce::Colour (0x18FFFFFF));
            g.strokePath (combinedPath, juce::PathStrokeType (5.f, juce::PathStrokeType::curved));
            g.setColour (juce::Colour (0xEEFFFFFF));
            g.strokePath (combinedPath, juce::PathStrokeType (1.8f, juce::PathStrokeType::curved));
        }

        g.restoreState();

        // Plot border
        g.setColour (juce::Colour (BiquadColors::PANEL_BORDER));
        g.drawRect (plot, 1.f);

        // === Header + legend ===
        g.setFont (juce::Font (juce::Font::getDefaultMonospacedFontName(), 9.f, juce::Font::bold));
        g.setColour (juce::Colour (BiquadColors::TEXT_DIM));
        g.drawText ("FREQUENCY RESPONSE",
                    (int)(bounds.getX() + ml), (int)bounds.getY() + 8,
                    200, 14, juce::Justification::centredLeft);

        // Legend (top right)
        const struct { uint32_t col; const char* txt; } lgnd[] = {
            { BiquadColors::ACCENT_1, "S1" },
            { BiquadColors::ACCENT_2, "S2" },
            { BiquadColors::ACCENT_3, "S3" },
        };
        float lx = bounds.getRight() - mr - 6.f;
        const float ly = bounds.getY() + 10.f;
        g.setFont (juce::Font (juce::Font::getDefaultMonospacedFontName(), 8.f, juce::Font::plain));

        // SUM
        g.setColour (juce::Colour (0xBBFFFFFF));
        g.drawText ("SUM", (int)(lx - 28), (int)ly, 26, 12, juce::Justification::centredRight);
        g.fillRect (lx - 30.f, ly + 6.f, 14.f, 2.f);
        lx -= 50.f;

        for (int i = 2; i >= 0; --i)
        {
            g.setColour (juce::Colour (lgnd[i].col).withAlpha (0.65f));
            g.drawText (lgnd[i].txt, (int)(lx - 20), (int)ly, 18, 12, juce::Justification::centredRight);
            g.fillRect (lx - 22.f, ly + 6.f, 14.f, 2.f);
            lx -= 34.f;
        }
    }

    void resized() override { dirty = true; }

private:
    TripleBiquadAudioProcessor& processor;
    juce::Path combinedPath;
    std::vector<float> stage1Mags, stage2Mags, stage3Mags;
    bool dirty = true;

    static constexpr int   NUM_POINTS = 600;
    static constexpr float FREQ_MIN   = 20.f;
    static constexpr float FREQ_MAX   = 20000.f;
    static constexpr float DB_MIN     = -60.f;
    static constexpr float DB_MAX     = 18.f;

    float plotX (float freq, const juce::Rectangle<float>& plot) const
    {
        return plot.getX() + plot.getWidth()
               * std::log10 (freq / FREQ_MIN) / std::log10 (FREQ_MAX / FREQ_MIN);
    }

    float plotY (float dB, const juce::Rectangle<float>& plot) const
    {
        return plot.getBottom() - plot.getHeight() * (dB - DB_MIN) / (DB_MAX - DB_MIN);
    }

    static float biquadMag (float omega,
                             float b0, float b1, float b2,
                             float a0, float a1, float a2)
    {
        const float cW = std::cos (omega), c2W = std::cos (2.f * omega);
        const float sW = std::sin (omega), s2W = std::sin (2.f * omega);
        const float rN = b0 + b1*cW + b2*c2W, iN = -(b1*sW + b2*s2W);
        const float rD = a0 + a1*cW + a2*c2W, iD = -(a1*sW + a2*s2W);
        const float den = rD*rD + iD*iD;
        return den > 1e-30f ? std::sqrt ((rN*rN + iN*iN) / den) : 0.f;
    }

    void recalculate()
    {
        const float sr = 44100.f, pi = juce::MathConstants<float>::pi;
        auto get = [&] (const char* id) {
            return processor.apvts.getRawParameterValue (id)->load();
        };

        struct C { float b0,b1,b2,a0,a1,a2; };
        auto makeLP = [&] (float f, float Q) -> C {
            float w = 2.f*pi*f/sr, a = std::sin(w)/(2.f*Q);
            return { (1-std::cos(w))/2.f, 1-std::cos(w), (1-std::cos(w))/2.f,
                      1+a, -2*std::cos(w), 1-a };
        };

        auto c1 = makeLP (get("cutoff1"), get("q1"));
        auto c2 = makeLP (get("cutoff2"), get("q2"));
        auto c3 = makeLP (get("cutoff3"), get("q3"));

        stage1Mags.resize (NUM_POINTS);
        stage2Mags.resize (NUM_POINTS);
        stage3Mags.resize (NUM_POINTS);
        combinedPath.clear();

        auto b = getLocalBounds().toFloat();
        const float ml = 50.f, mr = 22.f, mt = 32.f, mb = 32.f;
        const juce::Rectangle<float> plot (b.getX() + ml, b.getY() + mt,
                                            b.getWidth() - ml - mr,
                                            b.getHeight() - mt - mb);

        bool first = true;
        for (int i = 0; i < NUM_POINTS; ++i)
        {
            const float t     = (float)i / (NUM_POINTS - 1);
            const float freq  = FREQ_MIN * std::pow (FREQ_MAX / FREQ_MIN, t);
            const float omega = 2.f * pi * freq / sr;

            stage1Mags[i] = biquadMag (omega, c1.b0,c1.b1,c1.b2, c1.a0,c1.a1,c1.a2);
            stage2Mags[i] = biquadMag (omega, c2.b0,c2.b1,c2.b2, c2.a0,c2.a1,c2.a2);
            stage3Mags[i] = biquadMag (omega, c3.b0,c3.b1,c3.b2, c3.a0,c3.a1,c3.a2);

            const float combined = stage1Mags[i] * stage2Mags[i] * stage3Mags[i];
            const float dB  = juce::Decibels::gainToDecibels (combined, -96.f);
            const float xp  = plotX (freq, plot);
            const float yp  = plotY (dB, plot);

            if (first) { combinedPath.startNewSubPath (xp, yp); first = false; }
            else          combinedPath.lineTo (xp, yp);
        }
    }
};

//==============================================================================
// Main Editor
//==============================================================================
class TripleBiquadAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    TripleBiquadAudioProcessorEditor (TripleBiquadAudioProcessor&);
    ~TripleBiquadAudioProcessorEditor() override;
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    TripleBiquadAudioProcessor& audioProcessor;
    StagePanel stage1, stage2, stage3;
    FreqResponseDisplay freqDisplay;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TripleBiquadAudioProcessorEditor)
};
