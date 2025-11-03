{ pkgs ? import <nixpkgs> {} }:

let 
	easerlib = pkgs.stdenv.mkDerivation {
		name = "easer";
  		src = pkgs.fetchFromGitHub {
			owner = "xayler4";
			repo = "easer";
			rev = "main";
			sha256 = "sha256-oB3bBghpwciKZln8BFXFmupUT3L7Xz96uMSDTDUkCKs=";
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
