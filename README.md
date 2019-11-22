**Highly experimental, work in progress**

TODO: options without any prefix. It is valid to define a `my-option` option (instead of e.g. `--my-option`). A call `./program my-option my-value` is then equivalent to the would-be `./program --my-option my-value`. There is no real reason to prepend options with special markers.

TODO: merged flags. You can define both `-help` option, and individal `-h`, `-e`, `-l`, `-p` options or flags; `-help` will relate to the full help option while any other combination triggers corresponding one-letter flags.
