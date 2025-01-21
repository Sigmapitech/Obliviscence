{
  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";

  outputs = {
    self,
    nixpkgs,
  }: let
    forAllSystems = function:
      nixpkgs.lib.genAttrs [
        "x86_64-linux"
        "aarch64-linux"
      ] (system: function nixpkgs.legacyPackages.${system});
  in {
    devShells = forAllSystems (pkgs: {
      default = pkgs.mkShell {
        hardeningDisable = ["fortify"];
        packages = with pkgs; [
          gcc
          compiledb
          gcovr
        ];
      };
    });

    formatter = forAllSystems (pkgs: pkgs.alejandra);
    packages = forAllSystems (pkgs: let
      obli = self.packages.${pkgs.system}.obliviscence;
    in {
      obliviscence = pkgs.callPackage ./obliviscence.nix {};
      default = pkgs.writeShellScriptBin "run" ''
        LD_PRELOAD=${obli}/lib/libobliviscence.so $*
      '';
    });
  };
}
