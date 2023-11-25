with import <nixpkgs> {};

gcc13Stdenv.mkDerivation {
    name = "left-write-dev-env";
    buildInputs = [
        gcc13
        clang-tools
    ];
}
