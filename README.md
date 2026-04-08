Triple Biquad LPF
6th-order stereo cascaded lowpass filter VST3/AU plugin built in JUCE. Three biquad stages in series, each with independent cutoff and Q controls. Coefficients written by hand from Pirkle's book.
Controls (per stage)

Cutoff: 20 Hz – 20 kHz
Q: 0.5 – 12

Math
Each stage runs the standard biquad difference equation:
y[n] = (b0/a0)*x[n] + (b1/a0)*x[n-1] + (b2/a0)*x[n-2] - (a1/a0)*y[n-1] - (a2/a0)*y[n-2]
w0 = 2π * (cutoff / sampleRate) alpha = sin(w0) / (2 * Q)
b0 = (1 - cos(w0)) / 2 b1 = 1 - cos(w0) b2 = (1 - cos(w0)) / 2
a0 = 1 + alpha a1 = -2 * cos(w0) a2 = 1 - alpha
The output of each stage feeds into the next. Three cascaded 2nd-order sections gives an 18 dB/oct slope
overall. Each stage has its own independent state variables per channel, all reset in prepareToPlay.

Building
Open the .jucer file in Projucer, set your JUCE path, and export to your IDE
macOS: ~/Library/Audio/Plug-Ins/VST3/ or ~/Library/Audio/Plug-Ins/Components/
Windows: C:\Program Files\Common Files\VST3\
Rescan in your DAW after building
