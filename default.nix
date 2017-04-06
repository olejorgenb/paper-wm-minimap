# { stdenv, glib, cairo, pkgconfig, xorg }: 
with import <nixpkgs> {};

stdenv.mkDerivation rec {
  version = "0.x";
  name = "paper-wm-minimap-${version}";

  src = ./.;

  buildInputs =
    [ glib cairo pkgconfig clutter ] 
    ++ (with xorg; [  libX11 libXrender libXcomposite ]);

  installFlags = "PREFIX=\${out}";

}
