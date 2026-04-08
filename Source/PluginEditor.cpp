#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TripleBiquadAudioProcessorEditor::TripleBiquadAudioProcessorEditor (TripleBiquadAudioProcessor& p)
    : AudioProcessorEditor (&p),
      audioProcessor (p),
      stage1 (p, 1, juce::Colour (BiquadColors::ACCENT_1)),
      stage2 (p, 2, juce::Colour (BiquadColors::ACCENT_2)),
      stage3 (p, 3, juce::Colour (BiquadColors::ACCENT_3)),
      freqDisplay (p)
{
    addAndMakeVisible (stage1);
    addAndMakeVisible (stage2);
    addAndMakeVisible (stage3);
    addAndMakeVisible (freqDisplay);

    // Wider, taller: knobs across the top, analyzer taking up the lower ~55%
    setSize (760, 540);
    setResizable (false, false);
}

TripleBiquadAudioProcessorEditor::~TripleBiquadAudioProcessorEditor() {}

//==============================================================================
void TripleBiquadAudioProcessorEditor::paint (juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat();

    // ── Deep background ──────────────────────────────────────────────────────
    g.setColour (juce::Colour (BiquadColors::BG_DARK));
    g.fillRoundedRectangle (b, 12.f);

    // Scanline texture
    g.setColour (juce::Colour (0x03FFFFFF));
    for (int y = 0; y < getHeight(); y += 3)
        g.drawHorizontalLine (y, 0.f, (float)getWidth());

    // Top sheen
    juce::ColourGradient sheen (juce::Colour (0x10FFFFFF), 0, 0,
                                 juce::Colour (0x00000000), 0, 70, false);
    g.setGradientFill (sheen);
    g.fillRoundedRectangle (b, 12.f);

    // ── Header ────────────────────────────────────────────────────────────────
    {
        const float hh = 44.f;
        juce::ColourGradient hGrad (juce::Colour (0xFF1C1C28), 0, 0,
                                     juce::Colour (0xFF111118), 0, hh, false);
        g.setGradientFill (hGrad);
        g.fillRect (0.f, 0.f, (float)getWidth(), hh);

        g.setColour (juce::Colour (0xFF232330));
        g.drawHorizontalLine ((int)hh, 0.f, (float)getWidth());

        // Plugin name
        g.setFont (juce::Font (juce::Font::getDefaultMonospacedFontName(), 14.f, juce::Font::bold));
        g.setColour (juce::Colour (BiquadColors::TEXT_MAIN));
        g.drawText ("TRIPLE BIQUAD", 20, 0, 200, (int)hh, juce::Justification::centredLeft);

        // Subtitle
        g.setFont (juce::Font (juce::Font::getDefaultMonospacedFontName(), 10.f, juce::Font::plain));
        g.setColour (juce::Colour (BiquadColors::TEXT_DIM));
        g.drawText ("6th ORDER CASCADED LOWPASS", 168, 0, 280, (int)hh, juce::Justification::centredLeft);

        // Stage color dots
        const juce::Colour dots[] = {
            juce::Colour (BiquadColors::ACCENT_1),
            juce::Colour (BiquadColors::ACCENT_2),
            juce::Colour (BiquadColors::ACCENT_3)
        };
        for (int i = 0; i < 3; ++i)
        {
            const float dx = getWidth() - 72.f + i * 22.f;
            const float dy = hh * 0.5f - 5.f;
            g.setColour (dots[i]);
            g.fillEllipse (dx, dy, 10.f, 10.f);
            g.setColour (dots[i].withAlpha (0.3f));
            g.drawEllipse (dx, dy, 10.f, 10.f, 1.f);
        }

        // Decorative divider line right side
        g.setColour (juce::Colour (0xFF252535));
        g.drawLine (getWidth() - 110.f, 9.f, getWidth() - 10.f, 9.f, 0.5f);
        g.drawLine (getWidth() - 110.f, hh - 9.f, getWidth() - 10.f, hh - 9.f, 0.5f);
    }

    // ── Divider between knob row and freq display ─────────────────────────────
    // This is drawn as a slightly recessed separator
    const int knobSectionBottom = 44 + 12 + 168 + 12; // header + pad + stageH + pad
    g.setColour (juce::Colour (0xFF1A1A24));
    g.fillRect (0, knobSectionBottom, getWidth(), 2);
    g.setColour (juce::Colour (0xFF2A2A3C));
    g.fillRect (0, knobSectionBottom + 2, getWidth(), 1);

    // ── Outer border ──────────────────────────────────────────────────────────
    g.setColour (juce::Colour (0xFF262636));
    g.drawRoundedRectangle (b.reduced (0.5f), 12.f, 1.f);

    // ── Footer ────────────────────────────────────────────────────────────────
    {
        const float fh = 20.f;
        g.setColour (juce::Colour (0xFF090910));
        g.fillRect (0.f, (float)(getHeight()) - fh, (float)getWidth(), fh);
        g.setFont (juce::Font (juce::Font::getDefaultMonospacedFontName(), 7.5f, juce::Font::plain));
        g.setColour (juce::Colour (BiquadColors::TEXT_DIM));
        g.drawText ("BUTTERWORTH TOPOLOGY  ·  SAMPLE-ACCURATE  ·  STEREO",
                    0, getHeight() - (int)fh, getWidth(), (int)fh - 2,
                    juce::Justification::centred);
    }
}

//==============================================================================
void TripleBiquadAudioProcessorEditor::resized()
{
    const int headerH   = 44;
    const int footerH   = 20;
    const int padOuter  = 14;   // outer horizontal padding
    const int padInner  = 12;   // vertical padding between sections
    const int stageH    = 168;  // height of each stage panel (horizontal layout)
    const int stageGap  = 10;

    // ── Knob row ──────────────────────────────────────────────────────────────
    const int knobRowY  = headerH + padInner;
    const int totalW    = getWidth() - padOuter * 2;
    const int stageW    = (totalW - stageGap * 2) / 3;

    stage1.setBounds (padOuter,                          knobRowY, stageW, stageH);
    stage2.setBounds (padOuter + stageW + stageGap,     knobRowY, stageW, stageH);
    stage3.setBounds (padOuter + (stageW + stageGap)*2, knobRowY, stageW, stageH);

    // ── Freq display ──────────────────────────────────────────────────────────
    const int freqY     = knobRowY + stageH + padInner * 2 + 2; // +2 for the divider
    const int freqH     = getHeight() - freqY - footerH - padInner;

    freqDisplay.setBounds (padOuter, freqY, getWidth() - padOuter * 2, freqH);
}
