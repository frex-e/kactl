# Local TeX tree

Vendored [Inconsolata](https://ctan.org/pkg/inconsolata) (zi4 1.121) for `pdflatex`.
The notebook uses it as the typewriter font so `1` / `l` / `I` and `0` / `O` stay
distinct at listing size.

`make` sets `TEXMFHOME` to this directory and `kactlpkg.sty` loads the map with
`\pdfmapfile{+zi4.map}`. Do not drop unrelated packages in here.

Licenses are under `doc/fonts/inconsolata/`: SIL OFL for Regular, Apache 2.0 for Bold.
Source: CTAN `fonts/inconsolata` (Type1 + metrics; OpenType/docs omitted).
