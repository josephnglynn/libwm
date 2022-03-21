import 'package:flutter/material.dart';
import 'package:palette_generator/palette_generator.dart';
import 'dart:math' as math;

class Swatches extends StatelessWidget {
  const Swatches({Key? key, required this.generator}) : super(key: key);

  // The PaletteGenerator that contains all of the swatches that we're going
  // to display.
  final PaletteGenerator generator;

  @override
  Widget build(BuildContext context) {
    final List<Widget> swatches = <Widget>[];
    //The generator field can be null, if so, we return an empty container
    if (generator == null || generator.colors.isEmpty) {
      return Container();
    }
    //Loop through the colors in the PaletteGenerator and add them to the list of swatches above
    for (Color color in generator.colors) {
      swatches.add(PaletteSwatch(color: color));
    }
    return Column(
      mainAxisAlignment: MainAxisAlignment.center,
      mainAxisSize: MainAxisSize.min,
      crossAxisAlignment: CrossAxisAlignment.center,
      children: <Widget>[
        //All the colors,
        Wrap(
          children: swatches,
        ),
        //The colors with ranking
        Container(height: 30.0),
        PaletteSwatch(label: 'Dominant', color: generator.dominantColor?.color),
        PaletteSwatch(
            label: 'Light Vibrant', color: generator.lightVibrantColor?.color),
        PaletteSwatch(label: 'Vibrant', color: generator.vibrantColor?.color),
        PaletteSwatch(
            label: 'Dark Vibrant', color: generator.darkVibrantColor?.color),
        PaletteSwatch(
            label: 'Light Muted', color: generator.lightMutedColor?.color),
        PaletteSwatch(label: 'Muted', color: generator.mutedColor?.color),
        PaletteSwatch(
            label: 'Dark Muted', color: generator.darkMutedColor?.color),
      ],
    );
  }
}

@immutable
class PaletteSwatch extends StatelessWidget {
  // Creates a PaletteSwatch.
  //
  // If the [color] argument is omitted, then the swatch will show a
  // placeholder instead, to indicate that there is no color.
  const PaletteSwatch({
    Key? key,
    this.color,
    this.label,
  }) : super(key: key);

  // The color of the swatch. May be null.
  final Color? color;

  // The optional label to display next to the swatch.
  final String? label;

  @override
  Widget build(BuildContext context) {
    // Compute the "distance" of the color swatch and the background color
    // so that we can put a border around those color swatches that are too
    // close to the background's saturation and lightness. We ignore hue for
    // the comparison.
    final HSLColor hslColor = HSLColor.fromColor(color ?? Colors.transparent);
    final HSLColor backgroundAsHsl = HSLColor.fromColor(Colors.black);
    final double colorDistance = math.sqrt(
        math.pow(hslColor.saturation - backgroundAsHsl.saturation, 2.0) +
            math.pow(hslColor.lightness - backgroundAsHsl.lightness, 2.0));

    Widget swatch = Padding(
      padding: const EdgeInsets.all(2.0),
      child: color == null
          ? const Placeholder(
              fallbackWidth: 34.0,
              fallbackHeight: 20.0,
              color: Color(0xff404040),
              strokeWidth: 2.0,
            )
          : Container(
              decoration: BoxDecoration(
                  color: color,
                  border: Border.all(
                    width: 1.0,
                    color: Colors.white,
                    style: colorDistance < 0.2
                        ? BorderStyle.solid
                        : BorderStyle.none,
                  )),
              width: 34.0,
              height: 20.0,
            ),
    );

    if (label != null) {
      swatch = ConstrainedBox(
        constraints: const BoxConstraints(maxWidth: 130.0, minWidth: 130.0),
        child: Row(
          mainAxisAlignment: MainAxisAlignment.start,
          children: <Widget>[
            swatch,
            Container(width: 5.0),
            Text(label ?? ''),
          ],
        ),
      );
    }
    return swatch;
  }
}
