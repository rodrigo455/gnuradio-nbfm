# REDHAWK GnuRadio Narrow Band FM Baseband Modulator and Demodulator components
 
## Description

The project contains the source and build script for the REDHAWK Components GrNbFmRx and GrNbFmTx.
It's a proposal of a way to use gnuradio within REDHAWK SDR, thus it requires gnuradio as a dependency.

## A More Complete Integration

This work uses a simple C++ only solution with a template class that inherits gnuradio  sync block class... it's not a complete OTT module, therefore it is limited once gnuradio shines with python, with much more possibility of block usage. Of course there's room for improvement. You can find a more complete GnuRadio-REDHAWK integration from the [Geontech](https://github.com/Geontech)'s repo: **[gnuradio-redhawk](https://github.com/Geontech/gnuradio-redhawk)**

