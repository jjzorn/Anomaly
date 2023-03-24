# Content

Content is everything that is part of a game, apart from the engine itself. All content must be in
a specific directory, depending on the content type.

The directory layout must look like this:

```
Content/
    Fonts/
    Images/
    Scripts/
    Sounds/
```

Every content file that is in one of those folders is loaded automatically when the server starts
or on reload. When referencing another content file, e.g. the 'Content/Images/' prefix is added
automatically, and must not be used by the developer. Content is automatically distributed to all
connected players.

The only script that is executed is 'main.lua', however, this script can load other scripts from
the 'Content/Scripts/' directory by using the normal Lua 'require' function.
