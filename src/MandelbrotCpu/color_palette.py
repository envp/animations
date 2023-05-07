#!/usr/bin/env python3

# Taken from: https://stackoverflow.com/a/64165414/2857070
# Dumps the color palette as a file that can be #included to raylib
# Requires defining a MAKE_COLOR(r, g, b, a) macro before including

import io
import argparse
import textwrap
from typing import List, Literal, Union
import numpy as np
from dataclasses import dataclass
from scipy.interpolate import pchip_interpolate


@dataclass
class ColorPalette:
    reds: np.array
    greens: np.array
    blues: np.array


# The palette is taken from [1]
# [1]: https://stackoverflow.com/a/25816111/2857070
def _generate_ultra_fractal_palette(num_levels: int) -> ColorPalette:
    # fmt: off
    X_OBSERVED: List[float] = [0, 0.0625, 0.16, 0.42, 0.6425, 0.8575, 0.92875, 1.0]
    R_OBSERVED: List[float] = [0, 0     , 32  , 237   , 255   , 0   , 0      , 0]
    G_OBSERVED: List[float] = [0, 7     , 107 , 255   , 170   , 7   , 5      , 0]
    B_OBSERVED: List[float] = [0, 100   , 203 , 255   , 0     , 100 , 0      , 0]

    # fmt: on
    start, end = min(X_OBSERVED), max(X_OBSERVED)
    xs_interp = np.linspace(start, end, num=num_levels)
    r_interp = pchip_interpolate(X_OBSERVED, R_OBSERVED, xs_interp)
    g_interp = pchip_interpolate(X_OBSERVED, G_OBSERVED, xs_interp)
    b_interp = pchip_interpolate(X_OBSERVED, B_OBSERVED, xs_interp)
    return ColorPalette(r_interp, g_interp, b_interp)


def _guassian(xs, mu, sigma):
    return np.exp(-(((xs - mu) / sigma) ** 2))


def _generate_guassian_color_palette(num_levels: int):
    r_mu, r_sig = (num_levels * 160/255,num_levels *  50/255)
    g_mu, g_sig = (num_levels * 120/255,num_levels *  50/255)
    b_mu, b_sig = (num_levels * 65/255, num_levels * 35/255)

    xs = np.arange(num_levels)
    rs = 255 * _guassian(xs, r_mu, r_sig)
    gs = 255 * _guassian(xs, g_mu, g_sig)
    bs = 255 * _guassian(xs, b_mu, b_sig)
    return ColorPalette(rs, gs, bs)


PaletteKind = Union[Literal["gaussian"], Literal["ultra-fractal"]]


def generate_color_palette(num_levels: int, palette: PaletteKind) -> ColorPalette:
    if palette == "gaussian":
        return _generate_guassian_color_palette(num_levels)
    elif palette == "ultra-fractal":
        return _generate_ultra_fractal_palette(num_levels)
    raise ValueError("Unhandled palette kind")


def dump_color_palette_inc(colors: ColorPalette, output_file: io.TextIOWrapper) -> None:

    contents = "#if defined(MAKE_COLOR)"
    num_levels = len(colors.reds)
    for idx in range(num_levels):
        r = round(colors.reds[idx])
        g = round(colors.greens[idx])
        b = round(colors.blues[idx])
        contents += f"\nMAKE_COLOR({r}, {g}, {b}, 255),"

    contents += textwrap.dedent(
        """
        #else
        #error Define MAKE_COLOR(r, g, b, a) to use this include.
        #endif
        """
    )
    output_file.write(contents)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Dumps mandelbrot color palette as an includable file"
    )
    parser.add_argument(
        "-n",
        "--num-levels",
        type=int,
        required=True,
        help="Number of levels in the palette",
    )
    parser.add_argument(
        "-p",
        "--palette",
        choices=("gaussian", "ultra-fractal"),
        required=True,
        help="The color palette to use for rendering",
    )
    parser.add_argument("-o", "--output", type=argparse.FileType(mode="wt"))
    args = parser.parse_args()
    colors = generate_color_palette(args.num_levels, args.palette)
    dump_color_palette_inc(colors, args.output)
