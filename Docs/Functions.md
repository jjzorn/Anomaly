# Functions

As described in [Events](Events.md), games for Anomaly are written by responding to events. For
this, it is necessary that there are some functions that actually control the game state.

## reload()

When this function is called, all content and scripts will be reloaded before the next tick.

## start_text_input(player)

This function enables text input, meaning that the player will be able to input text (using a
screen keyboard on mobile devices).

## stop_text_input(player)

```stop_text_input``` disables text input again (potentially hiding the screen keyboard), and
clears whatever the player has entered so far (the so-called composition).

## get_composition(player)

This returns the current text input by ```player```.

## get_sprite_width(sprite)

```get_sprite_width``` returns the width of the sprite, if it height were 1.0. This is the same as
the aspect ratio.

## draw_sprite(player, sprite, x, y, scale)

This draws ```sprite``` onto ```player```'s screen, where the center of the sprite is given by
```x```, ```y```, and ```scale``` is the height of the sprite. The width can be queried using
```get_sprite_width```.

## draw_text(player, font, x, y, scale, r, g, b, text)

```draw_text``` draws ```text``` onto ```player```'s screen, using ```font```. The center of the
text is given by ```x```, ```y```, and ```scale``` is the height of the text. ```r```, ```g```,
and ```b``` are the RGB color values (each ranging from 0 to 255) which describe the color of the
text.

## kick(player)

This function removes ```player``` from the game.

## play_sound(player, sound, volume, channel?)

Starts playing ```sound``` for ```player```. Volume must be between 0 and 128. If ```channel``` is
given (must be between 0 and 7), the sound is played on that channel, overriding any sound playing
on that channel before. If not, the sound is played independently of the channel system. No more
than 8 sounds can be played if no channel is given.

## stop_sound(player, channel)

Stops the sound currently playing on ```channel``` for ```player```, which must be between 0 and 7.

## stop_all_sounds(player)

Stops all sounds currently playing for ```player```, even if they were not started with an explicit
channel number.
