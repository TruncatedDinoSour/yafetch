{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
    buildInputs = with pkgs; [
        clang gnumake
        lua5_4 pkg-config
    ];
    nativeBuildInputs = [ pkgs.pkg-config ];
    shellHook = ''
        ./configure --use-clang --use-harden --use-strip --use-lto --use-extreme-strip --use-install
        make
    '';
}
