# Biquads
Multi-mode Biquad filter for audio analysis purposes

+ Under construction
+ Currently only implementing 12dB low pass, high pass, and band pass using switchable transforms (much more to come) - there is currently no parameter smoothing, so BE AWARE that modulating parameters in real-time WILL create loud clicks and pops while passing audio.
+ This filter is subject to amplitude and phase warping of the frequency spectrum approaching the Nyquist frequency.

Please see my "Orfanidis Biquad" repository for further information meanwhile.

![Biquads](https://github.com/StoneyDSP/Biquads/blob/b96cfa63046e2122e3c016f16a318ab9aa8c5e13/Res/Biquads.png)
