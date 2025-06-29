{ pkgs ? import <nixpkgs> {} }:

let 
	easerlib = pkgs.stdenv.mkDerivation {
		name = "easer";
  		src = pkgs.fetchFromGitHub {
			owner = "xayler4";
			repo = "easer";
			rev = "5552d50c67cbaac62327b43698fa1311946c1e14";
			sha256 = "sha256-6xchGr/pJ4ibiQ9YHLrGQ2RZ1eLtb7Rdr9JKNKCL160=";
		};
		nativeBuildInputs = [ pkgs.cmake ];
		buildInputs = [ pkgs.clang pkgs.boost ];
		cmakeFlags = ["-DCMAKE_BUILD_TYPE=Debug" "-DCMAKE_INSTALL_PREFIX=$out"];

	};

in
pkgs.mkShell {
	buildInputs = [
		pkgs.clang
		pkgs.cmake
		pkgs.gdb
		pkgs.boost
		easerlib
	];
	
	# shellHook = ''
	# '';
}
