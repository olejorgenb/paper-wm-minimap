# { stdenv, glib, cairo, pkgconfig, xorg }: 
with import <nixpkgs> {};

stdenv.mkDerivation rec {
  version = "0.x";
  name = "mini-clutter-wm-${version}";

  src = ./.;

  buildInputs =
    [ glib cairo pkgconfig clutter ] 
    ++ (with xorg; [  libX11 libXrender libXcomposite ]);

  installFlags = "PREFIX=\${out}";

}
